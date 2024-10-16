/*
 * File:   HttpEncoder.hpp
 */

#include <string>
#include "Msg.hpp"
#include <memory>

#ifndef HTTPENCODER_HPP
#define HTTPENCODER_HPP
/**
 * \class HttpEncoder
 * \brief encode and decode http messages
 *
 */
class HttpEncoder
{
public:
    HttpEncoder ();
    virtual ~HttpEncoder ();

    std::shared_ptr<Msg> encode(std::string* in);
    std::string* decode(std::shared_ptr<Msg>) const;
    std::string getMessageID(std::shared_ptr<Msg> msg) const;
    static void build200Header(std::shared_ptr<Msg> msg, const std::string& mimType="application/xml");
    static void build303Header(std::shared_ptr<Msg> msg, const std::string& URL);
    static void build404Header(std::shared_ptr<Msg> msg);
    static void build500Header(std::shared_ptr<Msg> msg);
    static void addContent(std::shared_ptr<Msg> msg,const std::string &content);
    static std::string urlDecode(const std::string& in);
};


#endif  /* HTTPENCODER_HPP */
