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
#include <netinet/tcp.h>

#include <string.h>


int TcpListener::init(int portNumber, int timeOut)
{
    struct sockaddr_in addr; /* Local socket strust */
    struct linger linger; /* Linger struct       */
    int iSock; /* Socket file desccriptor */
    int iSocketReuseFlag;
    int iRC;

    _TimeOut = timeOut;

    if ((iSock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        return -1;
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
        return iRC;
    }

    memset((char *) &addr, 0, sizeof ( struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(portNumber);

    /*
     * Bind socket.
     */
    if ((iRC = bind(iSock, (struct sockaddr *) &addr, sizeof ( addr))) < 0)
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
    tv.tv_usec = _TimeOut % 1000;
    setsockopt(_FileDescr, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));


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
        //printf("accept error %d \n",iClientSock);
        return nullptr;
    }
    //printf("accept success %d \n",iClientSock);

    auto cnx =  std::make_shared<TcpConnection>(iClientSock);
    cnx->setTimeoutValue(_TimeOut);
    return cnx;
}

