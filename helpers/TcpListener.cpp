/*
 * TcpConnector.cpp
 *
 *
 *  This file describes tcp connector
 *
 *
 */



#include "TcpListener.hpp"



#include <netdb.h>
#include <unistd.h>


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <string.h>


int TcpListener::init(int portNumber, int timeOut)
{
    struct linger linger; /* Linger struct       */
    int iSock; /* Socket file desccriptor */
    int iSocketReuseFlag;
    int iRC;
    bool useV6 = true;

    _TimeOut = timeOut;

    /* Prefer a dual-stack IPv6 socket, fall back to IPv4 only. */
    if ((iSock = socket(AF_INET6, SOCK_STREAM, 0)) < 0)
    {
        useV6 = false;
        if ((iSock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            return -1;
        }
    }

    /*
     * Set socket options
     */
    linger.l_onoff = 0;
    linger.l_linger = 0;
    setsockopt(iSock, SOL_SOCKET, SO_LINGER, (const char*) &linger, sizeof (linger));

    iSocketReuseFlag = 1;

    if ((iRC = setsockopt(iSock, SOL_SOCKET, SO_REUSEADDR, &iSocketReuseFlag, sizeof ( iSocketReuseFlag))) < 0)
    {
        close(iSock);
        return iRC;
    }

    if (useV6)
    {
        /* accept IPv4 clients too (as ::ffff:a.b.c.d) */
        int v6only = 0;
        setsockopt(iSock, IPPROTO_IPV6, IPV6_V6ONLY, &v6only, sizeof(v6only));

        struct sockaddr_in6 addr6;
        memset(&addr6, 0, sizeof(addr6));
        addr6.sin6_family = AF_INET6;
        addr6.sin6_addr = in6addr_any;
        addr6.sin6_port = htons(portNumber);
        iRC = bind(iSock, (struct sockaddr *) &addr6, sizeof(addr6));
    }
    else
    {
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(portNumber);
        iRC = bind(iSock, (struct sockaddr *) &addr, sizeof(addr));
    }
    if (iRC < 0)
    {
        close(iSock);
        return iRC;
    }

    /*
     * Listen for a client connection.
     */
    if ((iRC = listen(iSock, 100)) < 0)
    {
        close(iSock);
        return iRC;
    }

    _FileDescr = iSock;

    struct timeval tv;
    tv.tv_sec = _TimeOut / 1000;
    tv.tv_usec = (_TimeOut % 1000) * 1000;
    setsockopt(_FileDescr, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));
    setsockopt(_FileDescr, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(struct timeval));


    return 0;
}

std::shared_ptr<TcpConnection> TcpListener::waitForClient(void)
{
    int iClientSock;

    /*
     * Accept connection.
     */
    if ((iClientSock = accept(_FileDescr, nullptr, nullptr)) < 0)
    {
        return nullptr;
    }

    int flag = 1;
    setsockopt(iClientSock, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));

    auto cnx =  std::make_shared<TcpConnection>(iClientSock);
    cnx->setTimeoutValue(_TimeOut);
    return cnx;
}

