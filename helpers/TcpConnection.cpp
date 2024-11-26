/*
 * TcpConnection.cpp
 *
 *
 *  This file describes tcp connection
 *
 *
 */
#include <stdio.h>
#include "TcpConnection.hpp"
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <unistd.h>
#include <iostream>
#include <thread>
using namespace std::literals;
int64_t TcpConnection::read( char* buffer, uint64_t length )
{
    int iLen = 0;
    if(_IsAlive)
    {
    unsigned int errorcount = 0;
        while(iLen <= 0)
        {
            iLen = recv( _FileDescr, ( void* ) buffer, length, 0 );
            if(iLen <= 0)
            {
                if(errno != 11) errorcount++;
                std::this_thread::sleep_for(10ms);
                //_IsAlive = false;
                _LastError = strerror(errno);
            }
            if(errorcount > 20)
            {
                std::cout << strerror(errno) << " a read error occured \n";
                _IsAlive = false;
                return -1;
            }
        }
    }

    return iLen;
}

int64_t TcpConnection::write( const char* buff, uint64_t length )
{
    int64_t iWritten = 0;
    int64_t toWrite = length;
    unsigned int errorcount = 0;
    //std::cout << "write : " << length << "\n";
    while( (iWritten < toWrite) &&(_IsAlive) )
    {
        int64_t i = send( _FileDescr, buff + iWritten, toWrite, MSG_NOSIGNAL );
        iWritten += i;

        if(i < 0)
        {
            errorcount++;
            std::this_thread::sleep_for(10ms);
            std::cout << strerror(errno) << " a write error occured \n";
            _LastError = strerror(errno);
        }

        if( errorcount > 20 )
        {
            _IsAlive = false;
            iWritten = -1;
            break;
        }
    }
    //std::cout << "wrote : " << iWritten << "\n";
    return iWritten;
}

void TcpConnection::abort( void )
{
    if( _FileDescr )
    {
        shutdown(_FileDescr, 0);
        close( _FileDescr );
    }

    _IsAlive = false;
    _FileDescr = 0;
}

void TcpConnection::setTimeoutValue(const unsigned int iSec)
{
    struct timeval tv;
    tv.tv_sec = iSec / 1000;
    tv.tv_usec = iSec % 1000;
    setsockopt(_FileDescr, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));
}
