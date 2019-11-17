/*
 * TcpConnector.cpp
 *
 *  Created on: 19 aug 2011
 *      Author: Alain Beylot
 *
 *
 *  This file describes tcp connector
 *
 *
 */
const char identOfTCPConnector_cpp[] = "$Id: TcpListener.cpp,v 1.3 2013/01/14 15:50:30 aby Exp $";



#include "TcpListener.hpp"



//#include <fcntl.h>
//#include <net/if.h>
#include <netdb.h>
#include <unistd.h>
//#include <stropts.h>
//#include <sys/ioctl.h>


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

    return 0;
}

TcpConnection* TcpListener::waitForClient(void)
{
    int iClientSock;

    /*
     * Accept connection.
     */
    if ((iClientSock = accept(_FileDescr, NULL, NULL)) < 0)
    {
        printf("accept error %d \n",iClientSock);
        return NULL;
    }
    //printf("accept success %d \n",iClientSock);

    TcpConnection* cnx =  new TcpConnection(iClientSock);
    cnx->setTimeoutValue(_TimeOut);
    return cnx;
}

