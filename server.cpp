#include <atomic>
#include <iostream>
#include <string>
#include <unistd.h>
//#include <signal.h>
#include <stdlib.h>
#include <ctime>
#include "messaging/HttpProtocol.hpp"
#include "messaging/Msg.hpp"
#include "messaging/HttpEncoder.hpp"
#include "CompiledDataManager.hpp"
#include "services/ServiceInterface.hpp"
#include "services/ServicesFactory.hpp"
#include "helpers/TcpListener.hpp"
#include "helpers/NonGrowableQueue.hpp"
#include <csignal>
#include "helpers/ExtThread.hpp"

#define MAX_PENDING_REQUESTS 100

/**************************************************************************************************************************/
/**************************************************************************************************************************/
/**************************************************************************************************************************/

ParmsXmlVisitor params;
std::atomic<int> pending(0);
volatile bool sigstop;
void sig_handler(int sig)
{
	sigstop = true;
}

struct Listener
{
  explicit  Listener(NonGrowableQueue<TcpConnection*, MAX_PENDING_REQUESTS>* queue) : queue(queue)
    {
    }


    int operator()()
    {
        std::cout << "Listening on port : " << params.getNumParam("ServerPort", 8081) << "\n";
        listener.init(params.getNumParam("ServerPort", 8081),100);
        while(!ExtThreads::stop_requested())
        {
            TcpConnection *cnx = listener.waitForClient();
            if(cnx)
            {
                if(pending < MAX_PENDING_REQUESTS)
                {   
                    queue->push(cnx);
                } else {
                    std::cerr << "too much pending requests, client rejected !\n";
                    delete cnx;  
                }
            }
		}
        std::cout << "Listener end\n";
        return 0;
    }
private:
    NonGrowableQueue<TcpConnection*, MAX_PENDING_REQUESTS>* queue;
    TcpListener listener;
};


template<typename MSG> struct Reader
{
    Reader(NonGrowableQueue<MSG*, MAX_PENDING_REQUESTS>* queue,NonGrowableQueue<TcpConnection*, MAX_PENDING_REQUESTS>* queueS) : queueS(queueS),queue(queue)
    {
    }
    int operator()()
    {
        HttpEncoder encoder;
        while(!ExtThreads::stop_requested())
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
                        pending--;
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
    NonGrowableQueue<TcpConnection*, MAX_PENDING_REQUESTS>* queueS;
    NonGrowableQueue<MSG*, MAX_PENDING_REQUESTS>* queue;
};

template<typename MSG> struct Writer
{
    HttpProtocol p;
    HttpEncoder encoder;
    explicit Writer(NonGrowableQueue<MSG*, MAX_PENDING_REQUESTS>* queue) :  queue(queue)
    {
    }
    int operator()()
    {
        MSG* m;
        while(!ExtThreads::stop_requested())
        {
            while(queue->pop(m)&&!ExtThreads::stop_requested())
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
                    pending--;
                }
            }
            usleep(10000);
        }
        std::cout << "Writer end\n";
        return 0;
    }

private:
    NonGrowableQueue<MSG*, MAX_PENDING_REQUESTS>* queue;
};

template<typename MSG> struct Exec
{
    HttpProtocol p;
    HttpEncoder encoder;
    std::vector<IndexDesc*>* idxList;
    std::map<std::string, std::string>* symbols;
    std::map<std::string, std::string>* charconvs;
    //std::map<std::string, std::string>* patterns;
    Exec(NonGrowableQueue<MSG*, MAX_PENDING_REQUESTS>* inqueue,NonGrowableQueue<MSG*, MAX_PENDING_REQUESTS>* outqueue,
         std::string file,
         uint microSleep,std::vector<IndexDesc*>* idxL,
         std::map<std::string, std::string>* symbs,
         std::map<std::string, std::string>* convs ) : inqueue(inqueue), outqueue(outqueue), file(file), microSleep(microSleep)
    {
        idxList = idxL;
        symbols = symbs;
        charconvs = convs;
        //patterns = patts;
    }
    int operator()()
    {
        CompiledDataManager mger(file, idxList, symbols, charconvs);
        MSG* m;
        while(!ExtThreads::stop_requested())
        {
            while(inqueue->pop(m)&&!ExtThreads::stop_requested())
            {
                try
                {
                    Msg* rep;
                    HttpEncoder encoder;
                    if(m->getRecordCount() > 0)
                    {
                        std::string url = m->getRecord(0)->getNamedValue("URL");
                        //printf("%s\n",url.c_str());
                        //time_t ctt = time(0);
                        //std::cout << asctime(localtime(&ctt)) << std::endl;
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
                        //ctt = time(0);
                        //std::cout << asctime(localtime(&ctt)) << std::endl;
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
    NonGrowableQueue<MSG*, MAX_PENDING_REQUESTS>* inqueue;
    NonGrowableQueue<MSG*, MAX_PENDING_REQUESTS>* outqueue;
    std::string file;
    uint microSleep;

};




int main(int argc, char *argv[])
{
	std::cout << "GeoPoint " << sizeof(GeoPoint) << "\n";
	std::cout << "GeoIndex " << sizeof(GeoIndex) << "\n";
	std::cout << "GeoPointIndex " << sizeof(GeoPointIndex) << "\n";
	std::cout << "GeoWayIndex " << sizeof(GeoWayIndex) << "\n";
	std::cout << "GeoMember " << sizeof(GeoMember) << "\n";
	std::cout << "GeoString " << sizeof(GeoString) << "\n";
	std::cout << "IndexEntry " << sizeof(IndexEntry) << "\n";
	std::cout << "IndexRange " << sizeof(IndexRange) << "\n";

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
    std::map<std::string, std::string>* charconvs = new std::map<std::string, std::string>[params.getNumParam("ExecThreads", 5)];
    for(int i = 0; i < params.getNumParam("ExecThreads", 5); i++)
    {
        XmlVisitor v(indexes[i], false, std::string(argv[1]));
        config = fopen((std::string(argv[1]) + "/config.xml").c_str(),"r");
        XmlFileParser<XmlVisitor>::parseXmlFile(config,v);
        symbols[i] = v.symbols;
        charconvs[i] = v.charconvs;
        fclose(config);
    }


    NonGrowableQueue<Msg*, MAX_PENDING_REQUESTS> myInQueue;
    NonGrowableQueue<Msg*, MAX_PENDING_REQUESTS> myOutQueue;
    NonGrowableQueue<TcpConnection*, MAX_PENDING_REQUESTS> mySessionQueue;

    Listener listener(&mySessionQueue);
    uint microSleep = 30000;
    Reader<Msg> reader1(&myInQueue, &mySessionQueue);
    Reader<Msg> reader2(&myInQueue, &mySessionQueue);
    std::cout << "launching " << params.getNumParam("ExecThreads", 5) << " Exec threads \n";
    for(int i=0; i < params.getNumParam("ExecThreads", 5); i++)
    {
        Exec<Msg> exec(&myInQueue, &myOutQueue, argv[1], microSleep, /*&index*/ &(indexes[i]), &(symbols[i]), &(charconvs[i]));
        microSleep *= 2;
        ExtThreads::launch_thread(exec);
    }
    std::cout << "launching " << params.getNumParam("WriterThreads", 10) << " Writer threads \n";
    for(int i=0; i < params.getNumParam("WriterThreads", 10); i++)
    {
        Writer<Msg> writer(&myOutQueue);
        ExtThreads::launch_thread(writer);
    }
    ExtThreads::launch_thread(reader1);
    ExtThreads::launch_thread(reader2);
    ExtThreads::launch_thread(listener);


    /*
     * Manage signals;
     */
    sigstop = false; 
    std::signal(SIGINT,sig_handler);
    while(!sigstop)
    {
        usleep(1000);
    }

    ExtThreads::request_all_to_stop();
    ExtThreads::request_all_to_join();

}
