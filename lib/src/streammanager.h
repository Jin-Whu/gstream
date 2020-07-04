/// \file streammanager.h
/// \brief Header file for StreamManager
///
/// StreamManager is manager of StreamContext.

#ifndef STREAM_STREAMMANAGER_H_
#define STREAM_STREAMMANAGER_H_

#include <future>
#include <memory>
#include <string>
#include <unordered_map>
#include <thread>

#include "managerinfo.h"
#include "streamcontext.h"
#include "streamevent.h"


namespace stream
{
    class StreamManager
    {
        enum class STATUS
        {
            TYPE_RUNNING,
            TYPE_STOPPED,
        };

    public:
       enum class ACTION
       {
           TYPE_START,
           TYPE_STOP
       };

    public:
        StreamManager(const ManagerInfo &info);
        ~StreamManager();

    public:
        std::string id() const;
        void start();
        void stop();
        event_base* evbase() const;
        void set_streamev(std::shared_ptr<StreamEvent> ev);
        std::shared_ptr<StreamEvent> streamev() const;
    
    private:
        void init();
        void run();
    
    private:
        std::string m_id;
        STATUS m_status;
        ManagerInfo m_info;
        std::future<void> m_future;
        event_base *m_evbase;
        std::shared_ptr<StreamEvent> m_streamev;
        std::unordered_map<std::string, std::shared_ptr<StreamContext>> m_contexts_rover; ///< all stream context (key: rover.id)
    };
}

#endif