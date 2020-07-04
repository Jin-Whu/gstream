#ifndef DATAEVENT_H_
#define DATAEVENT_H_

#include "streamevent.h"

namespace stream
{
    class DataEvent : public StreamEvent
    {
    public:
        void event_cb(const GNSSData &data);
    };
}

#endif