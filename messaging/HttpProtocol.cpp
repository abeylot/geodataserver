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

int64_t HttpProtocol::putMessage(std::string& message, std::shared_ptr<TcpConnection> s)
{
    return s->write(message.c_str(), message.length());
}

int64_t HttpProtocol::getMessage(std::string& message,std::shared_ptr<TcpConnection> s)
{
    uint64_t rcv = 0;

    message = "";
    size_t iFound = std::string::npos;

    try
    {
        char buff[64000];
        int64_t rc = 0;
        bool complete = false;
        uint64_t iContentLength = 0;
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
                    if(iContentLength > MAX_CONTENT_LENGTH) return 0;
                } else return rcv;
            }
            if (iFound != std::string::npos)
            {
                complete = (rcv >= iFound + 4 + iContentLength) || (rcv > 640000);
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << "\n";
        return -1;
    }
    //std::cout <<"lus" << rcv << "\n";
    return rcv;
}
