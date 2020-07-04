#include <iostream>
#include "streamlog.h"
#include "streamcfg.h"
#include "streamio.h"
#include "streamsystem.h"
#include "stream.h"


namespace stream
{
    Stream::Stream() :m_status(STATUS::TYPE_STOPPED), m_system(nullptr)
    {
    }

    Stream::~Stream()
    {
        stop();
        StreamIo::uninit();
        StreamLog::uninit();
    }

    void Stream::start()
    {
        if (m_status == STATUS::TYPE_RUNNING) return;

        if (!m_system) return;

        LOG_INFO("stream: stream starting");

        m_system->start();

        m_status = STATUS::TYPE_RUNNING;

        LOG_INFO("stream: stream started");
    }

    void Stream::stop()
    {
        if (m_status == STATUS::TYPE_RUNNING)
        {
            LOG_INFO("stream: stream stoping");
            m_system->stop();
            m_status = STATUS::TYPE_STOPPED;
            LOG_INFO("stream: stream stopped");
        }
    }

    bool Stream::init(const std::string &xml)
    {
        // load config
        StreamCfg cfg;
        SystemInfo system_info;
        if (!cfg.load(xml, system_info))
        {
            std::cerr << "load config failed" << std::endl;
            return false;
        }

        // init log
        StreamLog::init();

        // init io
        if (!StreamIo::init())
        {
            LOG_ERROR("stream io init failed");
            return false;
        }


        // load services
        for (const auto &service : system_info.services)
        {
            std::set<std::string> manager_ids;
            for (const auto &manager : service.managers)
            {
                manager_ids.emplace(manager.id);
            }

            m_id[service.id] = std::move(manager_ids);
        }

        m_system = std::make_unique<StreamSystem>(system_info);

        return true;
    }

    void Stream::set_event(const std::string &service_id, const std::string &manager_id, std::shared_ptr<StreamEvent> ev)
    {
        if (!ev)
            return;

        auto service = m_system->get(service_id);
        if (!service)
            return;

        auto manager = service->get(manager_id);
        if (!manager)
            return;

        manager->set_streamev(ev);
    }

    const Stream::StreamID& Stream::id() const
    {
        return m_id;
    }
}