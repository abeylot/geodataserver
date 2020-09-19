#include <boost/thread/thread.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/lockfree/queue.hpp>

#include <boost/system/system_error.hpp>
#include <iostream>
#include <string>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <ctime>
#include "messaging/HttpProtocol.hpp"
#include "messaging/Msg.hpp"
#include "messaging/HttpEncoder.hpp"
#include "CompiledDataManager.hpp"
#include "services/ServiceInterface.hpp"
#include "services/ServicesFactory.hpp"
#include "helpers/TcpListener.hpp"


/**************************************************************************************************************************/
/**************************************************************************************************************************/
/**************************************************************************************************************************/

ParmsXmlVisitor params;
std::atomic<int> pending(0);

struct Listener
{
  explicit  Listener(boost::lockfree::queue<TcpConnection*>* queue) : queue(queue)
    {
    }


    int operator()()
    {
        std::cout << "Listening on port : " << params.getNumParam("ServerPort", 8081) << "\n";
        listener.init(params.getNumParam("ServerPort", 8081),100);
        while(!boost::this_thread::interruption_requested())
        {
            TcpConnection *cnx = listener.waitForClient();
            
            if(pending < 100)
            {   
                queue->push(cnx);
            } else {
                std::cerr << "too much pending requests, client rejected !\n";
                delete cnx;  
            }
        }
        std::cout << "Listener end\n";
        return 0;
    }
private:
    boost::lockfree::queue<TcpConnection*>* queue;
    TcpListener listener;
};


template<typename MSG> struct Reader
{
    Reader(boost::lockfree::queue<MSG*>* queue,boost::lockfree::queue<TcpConnection*>* queueS) : queueS(queueS),queue(queue)
    {
    }
    int operator()()
    {
        HttpEncoder encoder;
        while(!boost::this_thread::interruption_requested())
        {
            while(!queueS->empty())
            {
                TcpConnection* s;
                bool res = queueS->pop(s);
                if(res)
                {
                    try
                    {
                        HttpProtocol p;
                        std::string m;
                        uint32_t len = p.getMessage(m,s);
                        if(len)
                        {
                            Msg* msg = encoder.encode(&m);
                            msg->setConnection(s);
                            queue->push(msg);
                            pending++;
                        }
                    }
                    catch (const std::exception& e)
                    {
                        std::cerr << e.what() << "\n";
                        delete s;
                    }
                }
            }
            usleep(10000);
        }
        std::cout << "Reader end \n";
        return 0;
    }

private:
    boost::lockfree::queue<TcpConnection*>* queueS;
    boost::lockfree::queue<MSG*>* queue;
};

template<typename MSG> struct Writer
{
    HttpProtocol p;
    HttpEncoder encoder;
    explicit Writer(boost::lockfree::queue<MSG*>* queue) :  queue(queue)
    {
    }
    int operator()()
    {
        MSG* m;
        while(!boost::this_thread::interruption_requested())
        {
            while(queue->pop(m)&&!boost::this_thread::interruption_requested())
            {
                try
                {
                    std::string* s = encoder.decode(m);
                    p.putMessage(*s,m->getConnection());
					usleep(10000);
                    delete s;
                    delete m->getConnection();
                    delete m;
                    pending --;
                }
                catch (const std::exception& e)
                {
                    std::cerr << e.what() << "\n";
                }
            }
            usleep(10000);
        }
        std::cout << "Writer end\n";
        return 0;
    }

private:
    boost::lockfree::queue<MSG*>* queue;
};

template<typename MSG> struct Exec
{
    HttpProtocol p;
    HttpEncoder encoder;
    std::vector<IndexDesc*>* idxList;
    std::map<std::string, std::string>* symbols;
    std::map<std::string, std::string>* patterns;
    Exec(boost::lockfree::queue<MSG*>* inqueue,boost::lockfree::queue<MSG*>* outqueue, std::string file,uint microSleep,std::vector<IndexDesc*>* idxL, std::map<std::string, std::string>* symbs, std::map<std::string, std::string>* patts ) : inqueue(inqueue), outqueue(outqueue), file(file), microSleep(microSleep)
    {
        idxList = idxL;
        symbols = symbs;
        patterns = patts;
    }
    int operator()()
    {
        CompiledDataManager mger(file, idxList, symbols, patterns);
        MSG* m;
        while(!boost::this_thread::interruption_requested())
        {
            while(inqueue->pop(m)&&!boost::this_thread::interruption_requested())
            {
                try
                {
                    Msg* rep;
                    HttpEncoder encoder;
                    if(m->getRecordCount() > 0)
                    {
                        std::string url = m->getRecord(0)->getNamedValue("URL");
                        printf("%s\n",url.c_str());
                        time_t ctt = time(0);
                        std::cout << asctime(localtime(&ctt)) << std::endl;
                        ServiceInterface* s = ServicesFactory::getService(url, params);
                        if(s)
                        {
                            rep = s->processRequest(m,mger);
                            ServicesFactory::releaseService(s);
                        }
                        else
                        {
                            rep = new Msg;
                            encoder.build404Header(rep);
                            encoder.addContent(rep,"<!DOCTYPE html><html> <head>  <meta charset=\"UTF-8\"></head> <body>Page "+url+" non trouvée.</body></html>");
                        }
                        ctt = time(0);
                        std::cout << asctime(localtime(&ctt)) << std::endl;
                    }
                    else
                    {
                        rep = new Msg;
                        encoder.build500Header(rep);
                    }
                    rep->setConnection(m->getConnection());
                    outqueue->push(rep);
                    delete m;
                }
                catch (const std::exception& e)
                {
                    std::cerr << e.what() << "\n";
                }
            }
            usleep(microSleep);
        }
        std::cout << "Exec end\n";
        return 0;
    }

private:
    boost::lockfree::queue<MSG*>* inqueue;
    boost::lockfree::queue<MSG*>* outqueue;
    std::string file;
    uint microSleep;

};




int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        std::cerr << "path argument is missing\n";
        exit(1);
    }


    FILE* config = fopen((std::string(argv[1]) + "/config.xml").c_str(),"r");
    XmlFileParser<ParmsXmlVisitor>::parseXmlFile(config,params);
    fclose(config);

    std::vector<IndexDesc*>* indexes = new std::vector<IndexDesc*>[params.getNumParam("ExecThreads", 5)];
    std::map<std::string, std::string>* symbols = new std::map<std::string, std::string>[params.getNumParam("ExecThreads", 5)];
    std::map<std::string, std::string>* patterns = new std::map<std::string, std::string>[params.getNumParam("ExecThreads", 5)];
    for(int i = 0; i < params.getNumParam("ExecThreads", 5); i++)
    {
        XmlVisitor v(indexes[i], false, std::string(argv[1]));
        config = fopen((std::string(argv[1]) + "/config.xml").c_str(),"r");
        XmlFileParser<XmlVisitor>::parseXmlFile(config,v);
        symbols[i] = v.symbols;
        patterns[i] = v.patterns;
        fclose(config);
    }


    boost::lockfree::queue<Msg*> myInQueue(3);
    boost::lockfree::queue<Msg*> myOutQueue(3);
    boost::lockfree::queue<TcpConnection*> mySessionQueue(3);

    Listener listener(&mySessionQueue);
    boost::thread_group g;
    uint microSleep = 30000;
    Reader<Msg> reader1(&myInQueue, &mySessionQueue);
    Reader<Msg> reader2(&myInQueue, &mySessionQueue);
    std::cout << "launching " << params.getNumParam("ExecThreads", 5) << " Exec threads \n";
    for(int i=0; i < params.getNumParam("ExecThreads", 5); i++)
    {
        Exec<Msg> exec(&myInQueue, &myOutQueue, argv[1], microSleep, /*&index*/ &(indexes[i]), &(symbols[i]), &(patterns[i]));
        microSleep *= 2;
        g.create_thread(exec);
    }
    std::cout << "launching " << params.getNumParam("WriterThreads", 10) << " Writer threads \n";
    for(int i=0; i < params.getNumParam("WriterThreads", 10); i++)
    {
        Writer<Msg> writer(&myOutQueue);
        g.create_thread(writer);
    }
    g.create_thread(reader1);
    g.create_thread(reader2);
    g.create_thread(listener);


    /*
     * Manage signals;
     */
    bool shallContinue = true;
    while(shallContinue)
    {
        usleep(1000);
    }

    g.interrupt_all();
    g.join_all();


}
