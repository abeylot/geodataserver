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
public:
    HttpProtocol ();
    virtual ~HttpProtocol ();
    virtual int64_t putMessage(std::string& msg, std::shared_ptr<TcpConnection> s);
    virtual int64_t getMessage (std::string& msg, std::shared_ptr<TcpConnection> s);

};
#endif // HTTPPROTOCOL_HPP
