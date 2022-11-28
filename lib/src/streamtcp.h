#ifndef STREAM_STREAMTCP_H_
#define STREAM_STREAMTCP_H_

#include <event2/bufferevent.h>
#include "istream.h"

namespace stream
{
    class StreamTcp : public IStream
    {
    public:
        StreamTcp(StreamContext *context) :IStream(context), m_bev(NULL) {}
        ~StreamTcp() = default;

    public:
        void connect();
        void disconnect();
    
    private:
        static void read_cb(bufferevent *bev, void *arg);
        static void event_cb(bufferevent *bev, short events, void *arg);
        static void reconnect_timer_cb(evutil_socket_t fd, short events, void *arg);

    private:
        bufferevent *m_bev;
    };
}

#endif