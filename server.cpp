//g++ -lboost_system -lboost_thread -lboost_atomic -std=c++11 testBoostQueue2.cpp
#define MAIN
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


struct Listener
{
    Listener(boost::lockfree::queue<TcpConnection*>* queue) : queue(queue)
    {
    }

    /*void handle_accept(boost::asio::ip::tcp::socket *socket,const boost::system::error_code& error )
    {
    	std::cout << "new client connected ! \n";
    	queue->push(socket);
    	trigger_accept();
    }

    void trigger_accept()
    {
    		//std::cout << "toto1 \n";
    	try {
    		boost::asio::ip::tcp::socket *socket;
    		socket = new  boost::asio::ip::tcp::socket(*io_service);
    		acceptor->async_accept(*socket,
    		boost::bind(&Listener::handle_accept, this, socket,
    		boost::asio::placeholders::error));
    		io_service->run();
       } catch (const std::exception& e) {
    	   std::cerr << e.what() << "\n";
       }

    }*/


    int operator()()
    {
        //acceptor = new boost::asio::ip::tcp::acceptor(*io_service);
        //boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), 8081);
        //acceptor->open(endpoint.protocol());
        //acceptor->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        //acceptor->bind(endpoint);
        //acceptor->listen();
        //trigger_accept();
        std::cout << "Listening on port : " << params.getNumParam("ServerPort", 8081) << "\n";
        listener.init(params.getNumParam("ServerPort", 8081),100);
        while(!boost::this_thread::interruption_requested())
        {
            TcpConnection *cnx = listener.waitForClient();
            queue->push(cnx);
        }
        //delete acceptor;
        std::cout << "Listener end\n";
        return 0;
    }
private:
    //boost::asio::ip::tcp::acceptor* acceptor;
    boost::lockfree::queue<TcpConnection*>* queue;
    //boost::asio::io_service* io_service;
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
                        //s->non_blocking(true);
                        HttpProtocol p;
                        std::string m;
                        uint32_t len = p.getMessage(m,s);
                        if(len)
                        {
                            Msg* msg = encoder.encode(&m);
                            msg->setConnection(s);
                            queue->push(msg);
                        }
                    }
                    catch (const std::exception& e)
                    {
                        std::cerr << e.what() << "\n";
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
    Writer(boost::lockfree::queue<MSG*>* queue) :  queue(queue)
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
    Exec(boost::lockfree::queue<MSG*>* inqueue,boost::lockfree::queue<MSG*>* outqueue, std::string file,uint microSleep,std::vector<IndexDesc*>* idxL ) : inqueue(inqueue), outqueue(outqueue), file(file), microSleep(microSleep)
    {
        idxList = idxL;
    }
    int operator()()
    {
        CompiledDataManager mger(file, idxList);
        MSG* m;
//		int j;
//		char buffer[2049];
        //FILE* fIn = fopen(file.c_str(),"r");
        while(!boost::this_thread::interruption_requested())
        {
            while(inqueue->pop(m)&&!boost::this_thread::interruption_requested())
            {
                //printf("message received !\n");
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
        //fclose(fIn);
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

    FILE* config = fopen((std::string(argv[1]) + "/config.xml").c_str(),"r");
    XmlFileParser<ParmsXmlVisitor>::parseXmlFile(config,params);
    fclose(config);

    std::vector<IndexDesc*>* indexes = new std::vector<IndexDesc*>[params.getNumParam("ExecThreads", 5)];
    //std::vector<IndexDesc*> index;
    for(int i = 0; i < params.getNumParam("ExecThreads", 5); i++)
    {
        XmlVisitor v(indexes[i], false, std::string(argv[1]));
        //XmlVisitor v(index, false, std::string(argv[1]));
        config = fopen((std::string(argv[1]) + "/config.xml").c_str(),"r");
        XmlFileParser<XmlVisitor>::parseXmlFile(config,v);
        fclose(config);
    }

//boost::asio::io_service io_service;

    boost::lockfree::queue<Msg*> myInQueue(3);
    boost::lockfree::queue<Msg*> myOutQueue(3);
    boost::lockfree::queue<TcpConnection*> mySessionQueue(3);

    Listener listener(&mySessionQueue);
//DataManager dtaMger;
//dtaMger.load("test.xml");
//ServicesFactory::setDataManager(&dtaMger);
    boost::thread_group g;
    uint microSleep = 30000;
    Reader<Msg> reader1(&myInQueue, &mySessionQueue);
//Writer<Msg> writer1(&myOutQueue);
    Reader<Msg> reader2(&myInQueue, &mySessionQueue);
    std::cout << "launching " << params.getNumParam("ExecThreads", 5) << " Exec threads \n";
    for(int i=0; i < params.getNumParam("ExecThreads", 5); i++)
    {
        Exec<Msg> exec(&myInQueue, &myOutQueue, argv[1], microSleep, /*&index*/ &(indexes[i]));
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
//g.create_thread(writer1);
    g.create_thread(reader2);
//g.create_thread(writer2);
    g.create_thread(listener);


    /*
     * Manage signals;
     */
    //boost::asio::signal_set signals(io_service, SIGINT, SIGTERM);
    bool shallContinue = true;
    //signals.async_wait(
    //	[&shallContinue] (const boost::system::error_code& error, int signal_number)
    //	{printf("shutdown requested\n");shallContinue = false;}
    //	);
    while(shallContinue)
    {
        usleep(1000);
    }

//io_service.stop();
    g.interrupt_all();
    g.join_all();


}
