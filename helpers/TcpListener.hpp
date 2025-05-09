/*
 * TcpListener.hpp
 *

 *
 *
 *  This file describes tcp listener
 *
 *
 */


#include <string>
#include <memory>
#include "TcpConnection.hpp"

#ifndef TcpListener_hpp
#define TcpListener_hpp

/**
 * \class TcpListener
 * \brief TcpListener implements iListener for Tcp connection
 */
class TcpListener

{
private:
#ifdef doMemoryCheck
    CheckedObject< TcpListener > bidon;
#endif
    /**< socket identifier */
    int _FileDescr;
    int _TimeOut;
public:
    ~TcpListener()
    {

    }

    TcpListener()
    {
        _FileDescr = 0;
        _TimeOut = 30000;
    }

    /** \brief wait for client connection on tcpsocket
     *
     * \return IConnection* a new connection
     *
     */
    std::shared_ptr<TcpConnection> waitForClient( void );

    /** \brief
     *
     * \param portNumber int ident of port to open
     * \param timeOut (in seconds)
     * \return int lesser than zero in case of error
     *
     */
    int init( int portNumber, int timeOut = 10 );

};
#endif
