#ifndef STREAM_STREAMLOG_H_
#define STREAM_STREAMLOG_H_

#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace stream
{
    class StreamLog
    {
    public:
        enum LOGFLAG
        {
            NONE = 0x0,
            STDOUT = 0x1,
            TXT = 0x2,
            BIN = 0x4
        };

    public:
        static void init();
        static void uninit();
        static unsigned flag();
        static void set_flag(unsigned flag);

    private:
        static std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink;
        static std::shared_ptr<spdlog::sinks::daily_file_sink_mt> file_sink;
        static std::shared_ptr<spdlog::logger> logger;
        static unsigned logflag;
    };

    #define LOG_DEBUG SPDLOG_DEBUG
    #define LOG_INFO SPDLOG_INFO
    #define LOG_WARN SPDLOG_WARN
    #define LOG_ERROR SPDLOG_ERROR
} // namespace stream

#endif
