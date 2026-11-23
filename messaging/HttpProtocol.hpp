#ifndef HTTPPROTOCOL_HPP
#define HTTPPROTOCOL_HPP

#include <memory>
#include "../helpers/TcpConnection.hpp"

/**
 * \class HttpProtocol
 * \brief whrite and read HTTP message to socket
 */
class HttpProtocol
{
private:
    static constexpr int MAX_CONTENT_LENGTH = 32000;
    // maximum size of the request line + headers
    static constexpr size_t MAX_HEADER_SIZE = 65536;
    // hard limit on everything buffered for one request
    static constexpr size_t MAX_REQUEST_SIZE = MAX_HEADER_SIZE + MAX_CONTENT_LENGTH;
public:
    HttpProtocol ();
    virtual ~HttpProtocol ();
    virtual int64_t putMessage(std::string& msg, std::shared_ptr<TcpConnection> s);
    virtual int64_t getMessage (std::string& msg, std::shared_ptr<TcpConnection> s);

};
#endif // HTTPPROTOCOL_HPP
