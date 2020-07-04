#include "systeminfo.h"
#include "streamsystem.h"

namespace stream
{
    StreamSystem::StreamSystem(const SystemInfo &info) :m_info(info), m_status(STATUS::TYPE_STOPPED)
    {
        init();
    }

    StreamSystem::~StreamSystem()
    {
        stop();
    }

    void StreamSystem::start()
    {
        if (m_status == STATUS::TYPE_RUNNING) return;

        LOG_INFO("stream system: stream system starting");

        for (auto &service : m_services)
            service.second->start();
        
        m_status = STATUS::TYPE_RUNNING;

        LOG_INFO("stream system: stream system started");
    }

    void StreamSystem::stop()
    {
        if (m_status == STATUS::TYPE_RUNNING)
        {
            LOG_INFO("stream system: stream system stoping");
            for (auto &service : m_services)
                service.second->stop();
            m_status = STATUS::TYPE_STOPPED;
            LOG_INFO("stream system: stream system stopped");
        }
    }

    void StreamSystem::init()
    {
        for (const auto &service : m_info.services)
        {
            m_services[service.id] = std::make_shared<StreamService>(service);
        }
    }

    std::shared_ptr<StreamService> StreamSystem::get(const std::string &id)
    {
        if (m_services.count(id))
            return m_services[id];
        else
            return nullptr;
    }
}