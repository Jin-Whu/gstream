#include "streamlog.h"


namespace stream
{
    std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> StreamLog::console_sink = nullptr;
    std::shared_ptr<spdlog::sinks::daily_file_sink_mt> StreamLog::file_sink = nullptr;
    std::shared_ptr<spdlog::logger> StreamLog::logger = nullptr;
    unsigned StreamLog::logflag = StreamLog::NONE;

    void StreamLog::init()
    {
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
    }

    void StreamLog::uninit()
    {
        spdlog::shutdown();
    }

    unsigned StreamLog::flag()
    {
        return logflag;
    }

    void StreamLog::set_flag(unsigned flag)
    {
        logflag = flag;
    }

}