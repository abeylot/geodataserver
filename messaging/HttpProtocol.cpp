#include "HttpProtocol.hpp"
#include <iostream>
#include <exception>
#include <unistd.h>


HttpProtocol::HttpProtocol()
{
}

HttpProtocol::~HttpProtocol()
{
}

int64_t HttpProtocol::putMessage(std::string& message, TcpConnection* s)
{
    std::cout << "end send\n" ;
    return s->write(message.c_str(), message.length());
}

int64_t HttpProtocol::getMessage(std::string& message,TcpConnection* s)
{
    uint64_t rcv = 0;
    int64_t rc = 0;

    message = "";
    char buff[64000];
    size_t iFound = std::string::npos;
    bool complete = false;
    uint64_t iContentLength = 0;

    try
    {
        while ((rc >= 0) && (!complete) && s->isAlive())
        {
            rc = s->read(buff,64000);
            if(rc>0)
            {
                message += std::string(buff,rc);
                rcv += rc;
            }
            if (rc <= 0) return -1;
            if (iFound == std::string::npos) iFound = message.find(std::string("\r\n\r\n"));
            if ((iFound != std::string::npos) && (iContentLength == 0))
            {
                size_t ictLength = message.find("Content-Length:");
                if (ictLength != std::string::npos)
                {
                    iContentLength = atoi(message.c_str() + ictLength + 16);
                }
            }
            if (iFound != std::string::npos)
            {
                complete = (rcv >= iFound + 4 + iContentLength);
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << "\n";
        return -1;
    }
    std::cout <<"lus" << rcv << "\n";
    return rcv;
}
