/*
 * TcpConnection.hpp
 *
 *  Created on: 19 aug 2011
 *      Author: Alain Beylot
 *
 *
 *  This file describes tcp connection
 *
 *
 */

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <string.h>
#include <string>

#ifndef TcpConnection_hpp
#define TcpConnection_hpp


const char identOfTcpConnection_hpp[] = "$Id: TcpConnection.hpp,v 1.9 2013/01/25 09:42:39 aby Exp $";



/**
 *  \class TcpConnection
 *  \brief this class provides a tcp connection (client or server side)
 */
class TcpConnection

{
private:
    /**< Tells us if connection is alive */
    bool _IsAlive;
    /**< stores socket identifier */
    int _FileDescr;
    std::string _LastError;
public:

    virtual ~TcpConnection ()
    {
        abort ();
    }

    /** \brief client side TcpConnection creation
     *
     * \param host
     * \param port
     * \return a new connection
     *
     */
    TcpConnection (std::string ip, int port)

    {
        _FileDescr = 0;
        _IsAlive = false;
        int sd = socket (AF_INET, SOCK_STREAM, 0);
        if (sd >= 0)
        {
            struct sockaddr_in sin;
            struct hostent *host = gethostbyname (ip.c_str ());

            memcpy (&sin.sin_addr.s_addr, host->h_addr, host->h_length);
            sin.sin_family = AF_INET;
            sin.sin_port = htons (port);

            if (connect (sd, (struct sockaddr *) &sin, sizeof ( sin)) == 0)
            {
                _FileDescr = sd;
                _IsAlive = true;
                //int flag = 1;
                //setsockopt (_FileDescr, /* socket affected */
                //            IPPROTO_TCP, /* set option at TCP level */
                //            TCP_NODELAY, /* name of option */
                //            (char *) &flag, /* the cast is historical
                //                                         cruft */
                //            sizeof ( int)); /* length of option value */
                //printf("connection success \n");
            }
        }


    }

    /** \brief server side TcpConnection creation
     *
     * \param fileDescr of socket
     * \return a new connection
     *
     */
    TcpConnection (int fileDescr)
    {
        _FileDescr = fileDescr;
        _IsAlive = true;

        //int flag = 1;
        //setsockopt (_FileDescr, /* socket affected */
        //            IPPROTO_TCP, /* set option at TCP level */
        //            TCP_NODELAY, /* name of option */
        //            (char *) &flag, /* the cast is historical
        //                                                 cruft */
        //            sizeof ( int)); /* length of option value */
        //setTimeoutValue (10);
    }

    /** \brief sends data in TcpConnection
     *
     * \param buf char* a buffer to send
     * \param length int data length
     * \return int	sent length or lesser tha zero if an error occured
     *
     */
    int64_t read (char* buf, uint64_t length);

    /** \brief
     *
     * \param buff char* user provided buffer to store data
     * \param length int maximum length to read
     * \return int	data length received
     *
     */
    int64_t write (const char* buff, uint64_t length);

    /** \brief
     *
     * \return bool false if socket has been broken
     *
     */
    bool isAlive (void)
    {
        return _IsAlive;
    }

    /** \brief terminates connection
     *
     *
     */
    void abort (void);

    std::string getLastError ()
    {
        return _LastError;
    }

    void setTimeoutValue (const unsigned int iSec);

};

/**
 * \class TcpClientConnector
 * \brief used to build tcp connections
 */
class TcpClientConnector

{
private:
    int _Port;
    std::string _Host;
    int _TimeOut;
public:

    TcpClientConnector (std::string host, int port, int timeOut = 1000)
    {
        _Port = port;
        _Host = host;
        _TimeOut = timeOut; //seconds
    }

    virtual ~TcpClientConnector ()
    {
    }

    /**
    * \brief builds a new connection
    * @return  a tcp connection
    */
    TcpConnection* connect ()
    {
        TcpConnection* cnx = new TcpConnection (_Host, _Port);
        cnx->setTimeoutValue (_TimeOut);
        return cnx;
    }
};
#endif
