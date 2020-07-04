#ifndef STREAM_STREAMNTRIP_H_
#define STREAM_STREAMNTRIP_H_


#include <event2/bufferevent.h>
#include "istream.h"

namespace stream
{
    class StreamNtrip : public IStream
    {
    public:
        StreamNtrip(StreamContext *context) :IStream(context), m_bev(nullptr) {}
        ~StreamNtrip() = default;

    public:
        void connect();
        void disconnect();
    
    private:
        void reqntrip(bufferevent *bev);
        void rspntrip();
        static void read_cb(bufferevent *bev, void *arg);
        static void event_cb(bufferevent *bev, short events, void *arg);
        static void reconnect_timer_cb(evutil_socket_t fd, short events, void *arg);
    
    private:
        bufferevent *m_bev;
    };
}

#endif