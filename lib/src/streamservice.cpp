#include "streamservice.h"


namespace stream
{
    StreamService::StreamService(const ServiceInfo &info) :m_info(info), m_id(info.id), m_status(STATUS::TYPE_STOPPED)
    {
        init();
    }

    StreamService::~StreamService()
    {
        stop();
    }

    std::string StreamService::id() const
    {
        return m_id;
    }

    void StreamService::start()
    {
        if (m_status == STATUS::TYPE_RUNNING) return;

        LOG_INFO("stream service: [id]:{} starting", m_id);

        for (auto &manager : m_managers)
            manager.second->start();
        m_status = STATUS::TYPE_RUNNING;

        LOG_INFO("stream service: [id]:{} started", m_id);
    }

    void StreamService::stop()
    {
        if (m_status == STATUS::TYPE_RUNNING)
        {
            LOG_INFO("stream service: [id]:{} stopping", m_id);

            for (auto &manager : m_managers)
               manager.second->stop();
            m_status = STATUS::TYPE_STOPPED;

            LOG_INFO("stream service: [id]:{} stopped", m_id);
        }
    }

    void StreamService::init()
    {
        for (const auto &manager : m_info.managers)
        {
            m_managers[manager.id] = std::make_shared<StreamManager>(manager);
        }
    }

    std::shared_ptr<StreamManager> StreamService::get(const std::string &id)
    {
        if (m_managers.count(id))
            return m_managers[id];
        else
            return nullptr;
    }
}