#ifndef HTTPPROTOCOL_HPP
#define HTTPPROTOCOL_HPP

//#include <boost/asio/connect.hpp>
//#include <boost/asio/deadline_timer.hpp>
//#include <boost/asio/io_service.hpp>
//#include <boost/asio/ip/tcp.hpp>
//#include <boost/asio/read_until.hpp>
//#include <boost/asio/streambuf.hpp>
//#include <boost/asio/placeholders.hpp>
//#include <boost/asio/write.hpp>
#include "../helpers/TcpConnection.hpp"

/**
 * \class HttpProtocol
 * \brief whrite and read HTTP message to socket
 */
class HttpProtocol
{
private:
#ifdef doMemoryCheck
    Helpers::CheckedObject< HttpProtocol > bidon;
#endif
public:
    HttpProtocol ();
    virtual ~HttpProtocol ();
    virtual int64_t putMessage(std::string& msg, TcpConnection* s);
    virtual int64_t getMessage (std::string& msg, TcpConnection* s);

};
#endif // HTTPPROTOCOL_HPP
