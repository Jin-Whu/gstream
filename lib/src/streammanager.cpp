#include <functional>
#include "managerinfo.h"
#include "streamlog.h"
#include "streammanager.h"

namespace stream
{
    StreamManager::StreamManager(const ManagerInfo &info) :m_info(info), m_id(info.id), m_status(STATUS::TYPE_STOPPED), m_evbase(NULL)
    {
        init();
    }

    StreamManager::~StreamManager()
    {
        stop();

        if (m_evbase)
        {
            event_base_free(m_evbase);
            m_evbase = NULL;
        }
    }

    std::string StreamManager::id() const
    {
        return m_id;
    }

    void StreamManager::start()
    {
        if (m_status == STATUS::TYPE_RUNNING) return;

        LOG_INFO("stream manager: [id]:{} starting", m_id);
        
        // event base
        if (!m_evbase)
        {
            return;
        }

        m_thread = std::thread(std::bind(&StreamManager::run,this));

        m_status = STATUS::TYPE_RUNNING;

        LOG_INFO("stream manager: [id]:{} started", m_id);
    }

    void StreamManager::stop()
    {
        if (m_status == STATUS::TYPE_RUNNING)
        {
            LOG_INFO("stream manager: [id]:{} stoping", m_id);

            event_base_loopbreak(m_evbase);

            for (auto context = m_contexts_rover.begin(); context != m_contexts_rover.end(); ++context)
                context->second->stop();

            m_thread.join();

            m_status = STATUS::TYPE_STOPPED;

            LOG_INFO("stream manager: [id]:{} stopped", m_id);
        }
    }

    void StreamManager::init()
    {
        m_evbase = event_base_new();
        if (!m_evbase)
        {
            return;
        }

        std::string context_base_id;
        std::shared_ptr<StreamContext> context_base;
        for (auto rover = m_info.rovers.begin(); rover != m_info.rovers.end(); ++rover)
        {
            m_contexts_rover[rover->id] = std::make_shared<StreamContext>(rover->id, *rover, this);
        }
    }

    void StreamManager::run()
    {
        for (auto context = m_contexts_rover.begin(); context != m_contexts_rover.end(); ++context)
            context->second->start();
        event_base_dispatch(m_evbase);
    }

    event_base* StreamManager::evbase() const
    {
        return m_evbase;
    }

    std::shared_ptr<StreamEvent> StreamManager::streamev() const
    {
        return m_streamev;
    }

    void StreamManager::set_streamev(std::shared_ptr<StreamEvent> ev)
    {
        m_streamev = ev;
    }
}