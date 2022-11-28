#include "streamlog.h"

namespace stream
{
#ifdef STREAM_LOG
    std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> StreamLog::console_sink = NULL;
    std::shared_ptr<spdlog::sinks::daily_file_sink_mt> StreamLog::file_sink = NULL;
    std::shared_ptr<spdlog::logger> StreamLog::logger = NULL;
#endif
    unsigned StreamLog::logflag = StreamLog::NONE;

    void StreamLog::init()
    {
#ifdef STREAM_LOG
        std::vector<spdlog::sink_ptr> sinks;
        if ((logflag & STDOUT) == STDOUT)
        {
            console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            sinks.push_back(console_sink);
        }

        if ((logflag & TXT) == TXT)
        {
            file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("logs/log.txt", 0, 0);
            sinks.push_back(file_sink);
        }

        logger = std::make_shared<spdlog::logger>("logger", std::begin(sinks), std::end(sinks));
        logger->set_pattern("%Y-%m-%d %T.%e [%^%l%$] %v");
        spdlog::set_default_logger(logger);
#endif
    }

    void StreamLog::uninit()
    {
#ifdef STREAM_LOG
        spdlog::shutdown();
#endif
    }

    unsigned StreamLog::flag()
    {
        return logflag;
    }

    void StreamLog::set_flag(unsigned flag)
    {
        logflag = flag;
    }

    void StreamBinLog::init()
    {
#ifdef STREAM_LOG
        m_binlog = spdlog::daily_logger_mt(m_id, "bins/" + m_id + ".bin", 0, 0);
        auto formatter = std::make_unique<spdlog::pattern_formatter>(spdlog::pattern_time_type::local, "");
        formatter->set_pattern("%v");
        m_binlog->set_formatter(std::move(formatter));
#endif
    }

    void StreamBinLog::info(const std::string &s)
    {
#ifdef STREAM_LOG
        m_binlog->info(s);
#endif
    }

}