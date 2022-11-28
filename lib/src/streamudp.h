#ifndef STREAM_STREAMUDP_H_
#define STREAM_STREAMUDP_H_

#include <event2/event.h>
#include "istream.h"

namespace stream
{
    class StreamUdp : public IStream
    {
    public:
        StreamUdp(StreamContext *context) :IStream(context), m_ev(NULL), m_addr(NULL) {}
        ~StreamUdp() = default;

    public:
        void connect();
        void disconnect();
    
    private:
        static void read_cb(evutil_socket_t fd, short events, void *arg);
        static void event_cb(evutil_socket_t fd, short events, void *arg);
        static void reconnect_timer_cb(evutil_socket_t fd, short events, void *arg);
    
    private:
        event *m_ev;
        evutil_addrinfo *m_addr;
    };
}

#endif