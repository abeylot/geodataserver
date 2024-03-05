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
using namespace std::literals;
/**************************************************************************************************************************/
/**************************************************************************************************************************/
/**************************************************************************************************************************/

ParmsXmlVisitor params;
volatile bool sigstop;
void sig_handler([[maybe_unused]]int sig)
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
                if(!queue->push(cnx))
                {
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
                            if(!queue->push(msg))
                            {
                                 std::cerr << "too much pending requests, message rejected !\n";
                                 delete msg;
                            }
                        }
                    }
                    catch (const std::exception& e)
                    {
                        std::cerr << e.what() << "\n";
                        delete s;
                    }
                }
            }
            std::this_thread::sleep_for(10ms);
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
                    //usleep(10000);
                    delete s;
                    delete m->getConnection();
                    delete m;
                }
                catch (const std::exception& e)
                {
                    std::cerr << e.what() << "\n";
                }
            }
            std::this_thread::sleep_for(10ms);
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
    std::vector<IndexDesc*>* idxList;
    std::map<std::string, std::string>* symbols;
    std::map<std::string, std::string>* charconvs;
    //std::map<std::string, std::string>* patterns;
    Exec(NonGrowableQueue<MSG*, MAX_PENDING_REQUESTS>* inqueue,NonGrowableQueue<MSG*, MAX_PENDING_REQUESTS>* outqueue,
         const std::string& file,
         uint microSleep,std::vector<IndexDesc*>* idxL,
         std::map<std::string, std::string>* symbs,
         std::map<std::string, std::string>* convs,
         CompiledDataManager& mger ) : inqueue(inqueue), outqueue(outqueue), file(file), microSleep(microSleep), mger(mger)
    {
        idxList = idxL;
        symbols = symbs;
        charconvs = convs;
        //patterns = patts;
    }
    int operator()()
    {
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
                        ServiceInterface* s = ServicesFactory::getService(url);
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
                    if(!outqueue->push(rep))
                    {
                         std::cerr << "too much pending responses, response rejected !\n";
                         delete rep;
                    }
                    delete m;
                }
                catch (const std::exception& e)
                {
                    std::cerr << e.what() << "\n";
                }
            }
            std::this_thread::sleep_for(microSleep * 1000ns);
        }
        std::cout << "Exec end\n";
        return 0;
    }

private:
    NonGrowableQueue<MSG*, MAX_PENDING_REQUESTS>* inqueue;
    NonGrowableQueue<MSG*, MAX_PENDING_REQUESTS>* outqueue;
    std::string file;
    uint microSleep;
    CompiledDataManager& mger;
};




int main(int argc, char *argv[])
{
    std::cout << "Version : " << VERSION << "\n";
    std::cout << "GeoPoint length:" << sizeof(GeoPoint) << "\n";
    std::cout << "GeoIndex length:" << sizeof(GeoIndex) << "\n";
    std::cout << "GeoPointIndex length:" << sizeof(GeoPointIndex) << "\n";
    std::cout << "GeoWayIndex length:" << sizeof(GeoWayIndex) << "\n";
    std::cout << "GeoMember length:" << sizeof(GeoMember) << "\n";
    std::cout << "GeoString length:" << sizeof(GeoString) << "\n";
    std::cout << "IndexEntry length:" << sizeof(IndexEntry) << "\n";
    std::cout << "IndexRange " << sizeof(IndexRange) << "\n";

    if(argc != 2)
    {
        std::cerr << "path argument is missing\n";
        exit(1);
    }


    FILE* config = fopen((std::string(argv[1]) + "/config.xml").c_str(),"r");
    if(!config)
    {
        std::cout << std::string(argv[1]) + "/config.xml" << " not found !\n";
        exit(1);
    }
    XmlFileParser<ParmsXmlVisitor>::parseXmlFile(config,params);
    fclose(config);

    ServicesFactory::init(params);

    std::vector<IndexDesc*> indexes;
    std::map<std::string, std::string> symbols;
    std::map<std::string, std::string> charconvs;

    XmlVisitor v(indexes, false, std::string(argv[1]));
    config = fopen((std::string(argv[1]) + "/config.xml").c_str(),"r");
    XmlFileParser<XmlVisitor>::parseXmlFile(config,v);
    symbols = v.symbols;
    charconvs = v.charconvs;
    fclose(config);


    NonGrowableQueue<Msg*, MAX_PENDING_REQUESTS> myInQueue;
    NonGrowableQueue<Msg*, MAX_PENDING_REQUESTS> myOutQueue;
    NonGrowableQueue<TcpConnection*, MAX_PENDING_REQUESTS> mySessionQueue;

    Listener listener(&mySessionQueue);
    uint microSleep = 30000;
    Reader<Msg> reader1(&myInQueue, &mySessionQueue);
    Reader<Msg> reader2(&myInQueue, &mySessionQueue);
    std::cout << "launching " << params.getNumParam("ExecThreads", 5) << " Exec threads \n";

    CompiledDataManager mger(std::string(argv[1]), &indexes, &symbols, &charconvs);


    for(int i=0; i < params.getNumParam("ExecThreads", 5); i++)
    {
        Exec<Msg> exec(&myInQueue,
                       &myOutQueue,
                       std::string(argv[1]),
                       microSleep,
                       &(indexes),
                       &(symbols),
                       &(charconvs), mger);
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
       std::this_thread::sleep_for(100ms);
    }

    ExtThreads::request_all_to_stop();
    ExtThreads::request_all_to_join();
    for(auto i : indexes) delete i;

}
