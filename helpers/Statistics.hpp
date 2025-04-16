#ifndef STATISTICS_HPP
#define STATISTICS_HPP
#include <cstdint>
#include <string>
class Statistics
{
    private:
    public:
    void start_request(int socket_id); 
    void end_request(int socket_id);
    void set_request_tag(int socket_id, const std::string& tag);
    void abort_request(int socket_id, const std::string& reason);
    const std::string build_html_report(); 
};
#endif
