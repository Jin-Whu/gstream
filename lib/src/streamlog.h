#ifndef STREAM_STREAMLOG_H_
#define STREAM_STREAMLOG_H_

#include <string>

#ifdef STREAM_LOG
#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/pattern_formatter.h>
#endif

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
#ifdef STREAM_LOG
        static std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink;
        static std::shared_ptr<spdlog::sinks::daily_file_sink_mt> file_sink;
        static std::shared_ptr<spdlog::logger> logger;
#endif
        static unsigned logflag;
    };

    class StreamBinLog
    {
    public:
        StreamBinLog() = default;
        ~StreamBinLog() = default;

    public:
        void init();
        void info(const std::string &s);

#ifdef STREAM_LOG
        std::shared_ptr<spdlog::logger> m_binlog; /// < stream binary logger
#endif
    };

#ifdef STREAM_LOG
#define LOG_DEBUG SPDLOG_DEBUG
#define LOG_INFO SPDLOG_INFO
#define LOG_WARN SPDLOG_WARN
#define LOG_ERROR SPDLOG_ERROR
#else
#define LOG_DEBUG(info, ...) (info)
#define LOG_INFO(info, ...) (info)
#define LOG_WARN(info, ...) (info)
#define LOG_ERROR(info, ...) (info)
#endif
} // namespace stream

#endif
