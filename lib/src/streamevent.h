#ifndef STREAM_EVENT_H_
#define STREAM_EVENT_H_

#include "gnssdata.h"

namespace stream
{
    class StreamEvent
    {
    public:
        virtual ~StreamEvent() = default;
    
    public:
        virtual void event_cb(const GNSSData &data) = 0;
    };
}

#endif