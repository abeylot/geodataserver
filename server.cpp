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
#include "helpers/Statistics.hpp"

#define MAX_PENDING_REQUESTS 100
using namespace std::literals;
/**************************************************************************************************************************/
/**************************************************************************************************************************/
/**************************************************************************************************************************/

ParmsXmlVisitor params;
Statistics stats;

volatile bool sigstop;
void sig_handler([[maybe_unused]]int sig)
{
    sigstop = true;
}

struct Listener
{
  explicit  Listener(NonGrowableQueue<std::shared_ptr<TcpConnection>, MAX_PENDING_REQUESTS>* queue) : queue(queue)
    {
    }


    int operator()()
    {
        std::cout << "Listening on port : " << params.getNumParam("ServerPort", 8081) << "\n";
        listener.init(params.getNumParam("ServerPort", 8081),100);
        while(!ExtThreads::stop_requested())
        {
            auto cnx = listener.waitForClient();
            if(cnx)
            {
                if(!queue->push(cnx))
                {
                    std::cerr << "too much pending requests, client rejected !\n";
                }
            }
        }
        std::cout << "Listener end\n";
        return 0;
    }
private:
    NonGrowableQueue<std::shared_ptr<TcpConnection>, MAX_PENDING_REQUESTS>* queue;
    TcpListener listener;
};


template<typename MSG> struct Reader
{
    Reader(NonGrowableQueue<std::shared_ptr<MSG>, MAX_PENDING_REQUESTS>* queue,NonGrowableQueue<std::shared_ptr<TcpConnection>, MAX_PENDING_REQUESTS>* queueS) : queueS(queueS),queue(queue)
    {
    }
    int operator()()
    {
        HttpEncoder encoder;
        while(!ExtThreads::stop_requested())
        {
            std::shared_ptr<TcpConnection> s = nullptr;
            bool res = queueS->pop_timed(s);
            if(res)
            {
                try
                {
                    HttpProtocol p;
                    std::string m;
                    int64_t len = p.getMessage(m,s);
                    if(len > 0)
                    {
                        stats.start_request(s->getFileDescr());
                        std::cout << "[" << s->getFileDescr() << "]";
                        std::shared_ptr<Msg> msg = encoder.encode(&m);
                        msg->setConnection(s);
                        if(!queue->push(msg))
                        {
                             std::cerr << "too much pending requests, message rejected !\n";
                             stats.abort_request(s->getFileDescr(), "Too much pending requests.");
                             //delete msg;
                        }
                    }
                }
                catch (const std::exception& e)
                {
                    stats.abort_request(s->getFileDescr(), e.what());
                    std::cerr << e.what() << "\n";
                }
            }
        }
        std::cout << "Reader end \n";
        return 0;
    }

private:
    NonGrowableQueue<std::shared_ptr<TcpConnection>, MAX_PENDING_REQUESTS>* queueS;
    NonGrowableQueue<std::shared_ptr<MSG>, MAX_PENDING_REQUESTS>* queue;
};

template<typename MSG> struct Writer
{
    HttpProtocol p;
    HttpEncoder encoder;
    explicit Writer(NonGrowableQueue<std::shared_ptr<MSG>, MAX_PENDING_REQUESTS>* queue, NonGrowableQueue<std::shared_ptr<TcpConnection>, MAX_PENDING_REQUESTS>* queue_s) :  queue(queue), queue_s(queue_s)
    {
    }
    int operator()()
    {
        std::shared_ptr<MSG> m;
        while(!ExtThreads::stop_requested())
        {
            if(queue->pop_timed(m)) try
            {
                std::cout <<"[" << m->getConnection()->getFileDescr() << "]";
                std::string* s = encoder.decode(m);
                p.putMessage(*s,m->getConnection());
                stats.end_request(m->getConnection()->getFileDescr());
                delete s;
                // see if connection still used
                if(m->getConnection()->isAlive())
                    // try to reuse connection by pushing it in queue,
                    // it will be closed and deleted in case of falilure
                    queue_s->push(m->getConnection());
            }
            catch (const std::exception& e)
            {
                stats.abort_request(m->getConnection()->getFileDescr(), e.what());
                std::cerr << e.what() << "\n";
            }
        }
        std::cout << "Writer end\n";
        return 0;
    }

private:
    NonGrowableQueue<std::shared_ptr<MSG>, MAX_PENDING_REQUESTS>* queue;
    NonGrowableQueue<std::shared_ptr<TcpConnection>, MAX_PENDING_REQUESTS>* queue_s;
};

template<typename MSG> struct Exec
{
    HttpProtocol p;
    std::vector<std::shared_ptr<IndexDesc>>* idxList;
    std::map<std::string, std::string>* symbols;
    std::map<std::string, std::string>* charconvs;
    //std::map<std::string, std::string>* patterns;
    Exec(NonGrowableQueue<std::shared_ptr<MSG>, MAX_PENDING_REQUESTS>* inqueue,NonGrowableQueue<std::shared_ptr<MSG>, MAX_PENDING_REQUESTS>* outqueue,
         const std::string& file,
         std::vector<std::shared_ptr<IndexDesc>>* idxL,
         std::map<std::string, std::string>* symbs,
         std::map<std::string, std::string>* convs,
         CompiledDataManager& mger ) : inqueue(inqueue), outqueue(outqueue), file(file), mger(mger)
    {
        idxList = idxL;
        symbols = symbs;
        charconvs = convs;
        //patterns = patts;
    }
    int operator()()
    {
        std::shared_ptr<MSG> m;
        while(!ExtThreads::stop_requested())
        {
            if(inqueue->pop_timed(m))
            {
                try
                {
                    std::shared_ptr<Msg> rep;
                    HttpEncoder encoder;
                    if(m->getRecordCount() > 0)
                    {
                        std::string url = m->getRecord(0)->getNamedValue("URL");
                        std::string etag = m->getRecord(0)->getNamedValue("If-None-Match");
                        auto s = ServicesFactory::getService(url);
                        if(s)
                        {
                            stats.set_request_tag(m->getConnection()->getFileDescr(), s->get_name());
                            rep = s->processRequest(m,mger);
                            auto new_rep = std::make_shared<Msg>();
                            Record* rcd = new Record;
                            Record* rcd2 = new Record;
                            rcd2->addBlock("");
                            new_rep->addRecord(rcd);
                            new_rep->addRecord(rcd2);
                            bool replace = false;;
                            if(!etag.empty())
                            {
                                std::string etag2=rep->getRecord(0)->getNamedValue("HTTPEtag");
                                if(etag == etag2)
                                {
                                    for(auto block : rep->getRecord(0)->getBlocks())
                                    {
                                        if(block.find("HTTPStatus=200") == 0 )
                                        {
                                            block = "HTTPStatus=304";
                                            replace = true;
                                        }
                                        rcd->addBlock(block);
                                    }
                                    if(replace)
                                    {
                                        std::string ctLength = std::string("content-length: ") + std::to_string(rep->getRecord(1)->getBlock(0)->size());
                                        new_rep->getRecord(0)->addBlock(ctLength);
                                        rep = new_rep;
                                    }
                                }
                            }
                        }
                        else if(url == "/stats")
                        {
                            rep = std::make_shared<Msg>();
                            encoder.build200Header(rep, "text/html");
                            encoder.addContent(rep,stats.build_html_report());
                            
                        }
                        else
                        {
                            rep = std::make_shared<Msg>();
                            encoder.build404Header(rep);
                            encoder.addContent(rep,"<!DOCTYPE html><html> <head>  <meta charset=\"UTF-8\"></head> <body>Page "+url+" NOT FOUND </body></html>");
                            stats.abort_request(m->getConnection()->getFileDescr(), "404");
                        }
                    }
                    else
                    {
                        rep = std::make_shared<Msg>();
                        encoder.build500Header(rep);
                        encoder.addContent(rep,"<!DOCTYPE html><html> <head>  <meta charset=\"UTF-8\"></head> <body>Page SERVER ERROR </body></html>");
                        stats.abort_request(m->getConnection()->getFileDescr(), "500");
                    }
                    rep->setConnection(m->getConnection());
                    if(!outqueue->push(rep))
                    {
                         std::cerr << "too much pending responses, response rejected !\n";
                         stats.abort_request(m->getConnection()->getFileDescr(), "Too much pending responses.");
                    }
                }
                catch (const std::exception& e)
                {
                    std::cerr << e.what() << "\n";
                    stats.abort_request(m->getConnection()->getFileDescr(), e.what());
                }
            }
        }
        std::cout << "Exec end\n";
        return 0;
    }

private:
    NonGrowableQueue<std::shared_ptr<MSG>, MAX_PENDING_REQUESTS>* inqueue;
    NonGrowableQueue<std::shared_ptr<MSG>, MAX_PENDING_REQUESTS>* outqueue;
    std::string file;
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

    std::vector<std::shared_ptr<IndexDesc>> indexes;
    std::map<std::string, std::string> symbols;
    std::map<std::string, std::string> charconvs;

    XmlVisitor v(indexes, false, std::string(argv[1]));
    config = fopen((std::string(argv[1]) + "/config.xml").c_str(),"r");
    XmlFileParser<XmlVisitor>::parseXmlFile(config,v);
    symbols = v.symbols;
    charconvs = v.charconvs;
    fclose(config);


    NonGrowableQueue<std::shared_ptr<Msg>, MAX_PENDING_REQUESTS> myInQueue;
    NonGrowableQueue<std::shared_ptr<Msg>, MAX_PENDING_REQUESTS> myOutQueue;
    NonGrowableQueue<std::shared_ptr<TcpConnection>, MAX_PENDING_REQUESTS> mySessionQueue;

    Listener listener(&mySessionQueue);
    Reader<Msg> reader1(&myInQueue, &mySessionQueue);
    Reader<Msg> reader2(&myInQueue, &mySessionQueue);
    std::cout << "launching " << params.getNumParam("ExecThreads", 5) << " Exec threads \n";

    CompiledDataManager mger(std::string(argv[1]), &indexes, &symbols, &charconvs);
    ServicesFactory::init(params, v.imageList);


    for(int i=0; i < params.getNumParam("ExecThreads", 5); i++)
    {
        Exec<Msg> exec(&myInQueue,
                       &myOutQueue,
                       std::string(argv[1]),
                       &(indexes),
                       &(symbols),
                       &(charconvs), mger);
        ExtThreads::launch_thread(exec);
    }
    std::cout << "launching " << params.getNumParam("WriterThreads", 10) << " Writer threads \n";
    for(int i=0; i < params.getNumParam("WriterThreads", 10); i++)
    {
        Writer<Msg> writer(&myOutQueue, &mySessionQueue);
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
    //for(auto i : indexes) delete i;

}
