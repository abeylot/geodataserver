#include "Statistics.hpp"
void Statistics::start_request(int socket_id)
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    uint64_t ts = (long long) tp.tv_sec * 1000L + tp.tv_usec / 1000;
 
    std::unique_lock lk(stats_mtx);
    _pending_requests[socket_id] = {"",ts};
} 

void Statistics::end_request(int socket_id)
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    uint64_t ts = (long long) tp.tv_sec * 1000L + tp.tv_usec / 1000;

    std::unique_lock lk(stats_mtx);
    auto it = _pending_requests.find(socket_id);
    if(it != _pending_requests.end())
    {
        PerformedRequest r = {it->second.tag, ts - it->second.start_timestamp, true, ""};
        _performed_requests.push_back(r);
        while(_performed_requests.size() > MAX_RETAINED_MEASURES) _performed_requests.pop_front();
        _pending_requests.erase(it);
    }
}

void Statistics::abort_request(int socket_id, const std::string& reason)
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    uint64_t ts = (long long) tp.tv_sec * 1000L + tp.tv_usec / 1000;

    std::unique_lock lk(stats_mtx);
    auto it = _pending_requests.find(socket_id);
    if(it != _pending_requests.end())
    {
        PerformedRequest r = {it->second.tag, ts - it->second.start_timestamp, false, reason};
        _performed_requests.push_back(r);
        while(_performed_requests.size() > MAX_RETAINED_MEASURES) _performed_requests.pop_front();
        _pending_requests.erase(it);
    }
}

void Statistics::set_request_tag(int socket_id, const std::string& tag)
{
    std::unique_lock lk(stats_mtx);
    auto it = _pending_requests.find(socket_id);
    if(it != _pending_requests.end())
    {
        it->second.tag = tag;
    }
}

const std::string Statistics::build_html_report()
{
    struct my_stats
    {
        std::string tag;
        uint64_t total_elapsed_ms;
        uint64_t max_elapsed_ms;
        uint64_t count;
        uint64_t failed_count;
    };

    std::map<std::string, my_stats> stats_map;
    
    std::unique_lock lk(stats_mtx);
    std::string result = "<!DOCTYPE html>\n<html><body>";
    for (auto it = _performed_requests.begin(); it != _performed_requests.end(); it++)
    {
        //result += it->tag + " " + std::to_string(it->elapsed_time);
        //result += "<br/>";
        std::string tag = it->tag;
        if(tag.empty()) tag = "unknown";
        auto it2 = stats_map.find(tag);
        if(it2 == stats_map.end())
        {
            if(it->is_success)
                stats_map[tag] = {tag, it->elapsed_time, it->elapsed_time, 1, 0};
            else
                stats_map[tag] = {tag, it->elapsed_time, it->elapsed_time, 1, 1};
        }
        else
        {
            it2->second.count++;
            it2->second.total_elapsed_ms += it->elapsed_time;
            if(!(it->is_success)) it2->second.failed_count++;
            if(it->elapsed_time > it2->second.max_elapsed_ms) it2->second.max_elapsed_ms = it->elapsed_time;
        }
    }
    result +="<p>statistics for last 10000 requests</p>";
    result += "<table style=\"border:solid;border-color:grey;border-width:2px\">";
    result += "<tr><th> Service </th> <th> count </th> <th> Average duration ms</th> <th> Max duration ms</th> <th> Failure rate %</th></tr>";
    bool is_odd = true;
    for(const auto& [tag, line] : stats_map)
    {
        if(is_odd) result += "<tr style=\"background-color:#E0E0E0; text-align:right\">";
        else  result +=      "<tr style=\"background-color:#F0F0F0; text-align:right\">";
        result += "<td>" + line.tag+ "</td> <td>"+ std::to_string(line.count)+ "</td> <td>"+ std::to_string(line.total_elapsed_ms / line.count)+ "</td> <td>"+ std::to_string(line.max_elapsed_ms)+ "</td> <td>" + std::to_string(100*line.failed_count/line.count) + "</td>";
        result += "</tr>";
        is_odd = is_odd ^ true;
    }
    result += "</table>";
    result += "</body></html>";
    return result;
} 
