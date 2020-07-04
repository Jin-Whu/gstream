#ifndef STREAM_STREAM_H_
#define STREAM_STREAM_H_


#include <memory>
#include <map>
#include <set>
#include <string>

#include "streamevent.h"

namespace stream
{
    class StreamSystem;

    class Stream
    {
        enum class STATUS
        {
            TYPE_RUNNING,
            TYPE_STOPPED
        };

        using StreamID = std::map<std::string, std::set<std::string>>;

    public:
        Stream();
        ~Stream();

    public:
        bool init(const std::string &xml);
        const StreamID& id() const;
        void set_event(const std::string &service_id, const std::string &manager_id, std::shared_ptr<StreamEvent> ev);
        void start();
        void stop();
    
    private:
        std::unique_ptr<StreamSystem> m_system;
        StreamID m_id; /// < service id -> manager ids
        STATUS m_status;
    };
}

#endif