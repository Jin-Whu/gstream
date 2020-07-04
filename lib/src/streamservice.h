#ifndef STREAM_STREAMSERVICE_H_
#define STREAM_STREAMSERVICE_H_

#include <memory>
#include <string>
#include <map>

#include "serviceinfo.h"
#include "streammanager.h"


namespace stream
{
    class StreamService
    {
        enum class STATUS
        {
            TYPE_RUNNING,
            TYPE_STOPPED
        };

    public:
        enum class ACTION
        {
            TYPE_START,
            TYPE_STOP
        };

    public:
        StreamService(const ServiceInfo &info);
        ~StreamService();

    public:
        std::string id() const;
        void start();
        void stop();
        std::shared_ptr<StreamManager> get(const std::string &id);
    
    private:
        void init();
    
    private:
        ServiceInfo m_info;
        std::string m_id;
        std::map<std::string, std::shared_ptr<StreamManager>> m_managers;
        STATUS m_status;
    };
}

#endif
