#ifndef STREAM_STREAMSYSTEM_H_
#define STREAM_STREAMSYSTEM_H_


#include <memory>
#include <map>

#include "streamservice.h"

namespace stream
{
    class StreamSystem
    {
        enum class STATUS
        {
            TYPE_RUNNING,
            TYPE_STOPPED
        };

    public:
        explicit StreamSystem(const SystemInfo &info);
        ~StreamSystem();

    public:
        void start();
        void stop();
        std::shared_ptr<StreamService> get(const std::string &id);
    
    private:
        void init();
    
    private:
        SystemInfo m_info;
        std::map<std::string, std::shared_ptr<StreamService>> m_services;
        STATUS m_status;
    };
}

#endif