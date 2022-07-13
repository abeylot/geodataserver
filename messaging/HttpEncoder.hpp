/*
 * File:   HttpEncoder.hpp
 */

#include <string>
#include "Msg.hpp"

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

    Msg* encode(std::string* in);
    std::string* decode(Msg*) const;
    std::string getMessageID(Msg* msg) const;
    static void build200Header(Msg* msg, const std::string& mimType="application/xml");
    static void build404Header(Msg* msg);
    static void build500Header(Msg* msg);
    static void addContent(Msg* msg,const std::string &content);
    static std::string urlDecode(const std::string& in);

private:
    int iIdent;
};


#endif  /* HTTPENCODER_HPP */
