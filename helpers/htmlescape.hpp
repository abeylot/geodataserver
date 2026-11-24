#ifndef HTMLESCAPE_HPP
#define HTMLESCAPE_HPP
#include <string>

/** Escape a string so it can be safely inserted in HTML text or in a quoted attribute. */
inline std::string html_escape(const std::string& in)
{
    std::string out;
    out.reserve(in.size());
    for (char c : in)
    {
        switch (c)
        {
            case '&':  out += "&amp;";  break;
            case '<':  out += "&lt;";   break;
            case '>':  out += "&gt;";   break;
            case '"':  out += "&quot;"; break;
            case '\'': out += "&#39;";  break;
            default:   out += c;
        }
    }
    return out;
}
#endif // HTMLESCAPE_HPP
