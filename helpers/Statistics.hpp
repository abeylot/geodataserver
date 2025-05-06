#ifndef STATISTICS_HPP
#define STATISTICS_HPP
#include <cstdint>
#include <string>
#include <deque>
#include <map>
#include <mutex>
#include <sys/time.h>

constexpr int MAX_RETAINED_MEASURES = 10000;

struct PendingRequest
{
    std::string tag;
    uint64_t start_timestamp;
};

struct PerformedRequest
{
    std::string tag;
    uint64_t elapsed_time;
    bool is_success;
    std::string failure_reason;
};



class Statistics
{
    private:
    std::mutex stats_mtx;
    std::map<int,PendingRequest> _pending_requests;
    std::deque<PerformedRequest> _performed_requests;
    public:
    void start_request(int socket_id); 
    void end_request(int socket_id);
    void set_request_tag(int socket_id, const std::string& tag);
    void abort_request(int socket_id, const std::string& reason);
    const std::string build_html_report(); 
};
#endif
