#include "Statistics.hpp"
#include <ctime>
void Statistics::start_request(int socket_id)
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    uint64_t ts = (long long) tp.tv_sec * 1000L + tp.tv_usec / 1000;
 
    std::unique_lock lk(stats_mtx);
    _pending_requests[socket_id] = {"",ts};
} 

void Statistics::end_request(int socket_id, size_t payload_size)
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    uint64_t ts = (long long) tp.tv_sec * 1000L + tp.tv_usec / 1000;

    std::unique_lock lk(stats_mtx);
    auto it = _pending_requests.find(socket_id);
    if(it != _pending_requests.end())
    {
        PerformedRequest r = {it->second.tag, ts - it->second.start_timestamp, true, "", payload_size};
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
        PerformedRequest r = {it->second.tag, ts - it->second.start_timestamp, false, reason, 0};
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

const std::string Statistics::build_html_report(const std::string& version)
{
    struct my_stats
    {
        std::string tag;
        uint64_t total_elapsed_ms;
        uint64_t max_elapsed_ms;
        uint64_t count;
        uint64_t failed_count;
        uint64_t total_payload_size;
        uint64_t max_payload_size;
    };

    std::map<std::string, my_stats> stats_map;

    {
        std::unique_lock lk(stats_mtx);
        for (const auto& req : _performed_requests)
        {
            std::string tag = req.tag.empty() ? "unknown" : req.tag;
            auto it = stats_map.find(tag);
            if (it == stats_map.end())
            {
                stats_map[tag] = {tag, req.elapsed_time, req.elapsed_time, 1,
                                  req.is_success ? 0u : 1u, req.payload_size, req.payload_size};
            }
            else
            {
                it->second.count++;
                it->second.total_elapsed_ms += req.elapsed_time;
                it->second.total_payload_size += req.payload_size;
                if (!req.is_success) it->second.failed_count++;
                if (req.elapsed_time > it->second.max_elapsed_ms) it->second.max_elapsed_ms = req.elapsed_time;
                if (req.payload_size > it->second.max_payload_size) it->second.max_payload_size = req.payload_size;
            }
        }
    }

    time_t now = time(nullptr);
    char time_buf[32];
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S UTC", gmtime(&now));

    auto fmt_bytes = [](uint64_t b) -> std::string {
        if (b >= 1024*1024) return std::to_string(b / (1024*1024)) + " MB";
        if (b >= 1024)      return std::to_string(b / 1024) + " KB";
        return std::to_string(b) + " B";
    };

    auto fail_class = [](uint64_t failed, uint64_t total) -> const char* {
        uint64_t rate = 100 * failed / total;
        if (rate == 0)  return "ok";
        if (rate < 20)  return "warn";
        return "bad";
    };

    std::string result = R"(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>GeoDataServer &mdash; Statistics</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,sans-serif;background:#f0f2f5;color:#333}
header{background:#1a1a2e;color:#fff;padding:18px 32px;display:flex;justify-content:space-between;align-items:center}
header h1{font-size:1.3rem;font-weight:600;letter-spacing:.3px}
.meta{font-size:.8rem;color:#aaa;text-align:right;line-height:1.6}
.meta .ver{color:#7eb8f7;font-weight:700}
main{padding:24px 32px}
.subtitle{font-size:.82rem;color:#777;margin-bottom:14px}
table{width:100%;border-collapse:collapse;background:#fff;border-radius:8px;overflow:hidden;box-shadow:0 1px 4px rgba(0,0,0,.12)}
thead tr{background:#2d3748;color:#e2e8f0}
thead th{padding:11px 16px;text-align:right;font-size:.8rem;font-weight:600;letter-spacing:.3px;white-space:nowrap}
thead th:first-child{text-align:left}
tbody tr:nth-child(even){background:#f8fafc}
tbody tr:hover{background:#ebf4ff}
td{padding:9px 16px;font-size:.86rem;text-align:right;border-bottom:1px solid #eee}
td:first-child{text-align:left;font-weight:500;color:#1a1a2e;font-family:monospace;font-size:.88rem}
.ok{color:#38a169;font-weight:600}
.warn{color:#d69e2e;font-weight:600}
.bad{color:#e53e3e;font-weight:600}
</style>
</head>
<body>
<header>
  <h1>GeoDataServer &mdash; Statistics</h1>
  <div class="meta">
    <div class="ver">)";
    result += version;
    result += R"(</div>
    <div>)";
    result += time_buf;
    result += R"(</div>
  </div>
</header>
<main>
  <p class="subtitle">Last )";
    result += std::to_string(MAX_RETAINED_MEASURES);
    result += R"( requests</p>
  <table>
    <thead><tr>
      <th>Service</th>
      <th>Requests</th>
      <th>Avg duration</th>
      <th>Max duration</th>
      <th>Failure rate</th>
      <th>Avg payload</th>
      <th>Max payload</th>
    </tr></thead>
    <tbody>
)";

    for (const auto& [tag, line] : stats_map)
    {
        uint64_t fail_rate = 100 * line.failed_count / line.count;
        result += "      <tr><td>" + line.tag
                + "</td><td>" + std::to_string(line.count)
                + "</td><td>" + std::to_string(line.total_elapsed_ms / line.count) + " ms"
                + "</td><td>" + std::to_string(line.max_elapsed_ms) + " ms"
                + "</td><td class=\"" + fail_class(line.failed_count, line.count) + "\">"
                + std::to_string(fail_rate) + "%"
                + "</td><td>" + fmt_bytes(line.total_payload_size / line.count)
                + "</td><td>" + fmt_bytes(line.max_payload_size)
                + "</td></tr>\n";
    }

    result += R"(    </tbody>
  </table>
</main>
</body>
</html>
)";
    return result;
}
