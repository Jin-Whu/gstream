#include <event2/event_struct.h>
#include "streamcontext.h"
#include "streamudp.h"

namespace stream
{
    void StreamUdp::connect()
    {
        if (!m_context)
            return;

        if (!m_context->m_evbase)
            return;

        evutil_addrinfo hints;
        memset(&hints, 0, sizeof(hints));

        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = IPPROTO_UDP;
        hints.ai_flags = EVUTIL_AI_ADDRCONFIG;

        char port_buf[6] = {};
        evutil_snprintf(port_buf, sizeof(port_buf), "%d", static_cast<int>(m_context->m_info.port));
        
        if (evutil_getaddrinfo(m_context->m_info.host.c_str(), port_buf, &hints, &m_addr) != 0)
        {
            evutil_freeaddrinfo(m_addr);
            return;
        }


        evutil_socket_t fd = socket(m_addr->ai_family, m_addr->ai_socktype, m_addr->ai_protocol);

        if (fd < 0)
            return;
        
        evutil_make_socket_closeonexec(fd);

        if (evutil_make_socket_nonblocking(fd) < 0)
        {
            evutil_closesocket(fd);
            return;
        }

        if (sendto(fd, "0", 1, 0, m_addr->ai_addr, m_addr->ai_addrlen) < 0)
        {
            evutil_closesocket(fd);
            return;
        }

        event *ev = event_new(m_context->m_evbase, fd, EV_TIMEOUT | EV_READ | EV_PERSIST, event_cb, this);

        if (!ev)
        {
            evutil_closesocket(fd);
            return;
        }

        timeval tv {30, 0};
        event_add(ev, &tv);
        m_ev = ev;
    }

    void StreamUdp::disconnect()
    {
        if (m_context->m_reconnect_timer)
        {
            event_free(m_context->m_reconnect_timer);
            m_context->m_reconnect_timer = nullptr;
        }

        if (m_addr)
        {
            evutil_freeaddrinfo(m_addr);
            m_addr = nullptr;
        }

        if (m_ev)
        {
            evutil_closesocket(event_get_fd(m_ev));
            event_free(m_ev);
            m_ev = nullptr;
        }

        m_context->m_status = StreamContext::STATUS::STATUS_CLOSED;
        LOG_INFO("stream udp: [id]:{} [STATUS]:{} [ACTION]:DISCONNECT [RESULT]:DISCONNECTED", m_context->id(), m_context->status());
    }

    void StreamUdp::read_cb(evutil_socket_t fd, short events, void *arg)
    {
        StreamUdp *conn = static_cast<StreamUdp *>(arg);

        if (!conn)
            return;

        conn->m_context->m_retry = 0;

        conn->m_context->m_buffsz = recvfrom(fd, conn->m_context->m_buff, sizeof(conn->m_context->m_buff), 0, conn->m_addr->ai_addr, &(conn->m_addr->ai_addrlen));

        conn->m_context->decode();
    }

    void StreamUdp::event_cb(evutil_socket_t fd, short events, void *arg)
    {
        StreamUdp *conn = static_cast<StreamUdp *>(arg);

        if (events & EV_READ)
        {
            conn->m_context->m_status = StreamContext::STATUS::STATUS_CONNECTED;
            read_cb(fd, events, conn);
        }
        else if (events & EV_TIMEOUT)
        {
            conn->disconnect();

            long t = 2 << conn->m_context->m_retry;
            timeval tv{t, 0};
            event *ev = evtimer_new(conn->m_context->m_evbase, reconnect_timer_cb, conn);
            evtimer_add(ev, &tv);
            conn->m_context->m_reconnect_timer = ev;
            conn->m_context->m_retry = conn->m_context->m_retry < MAX_RETRY ? conn->m_context->m_retry + 1 : MAX_RETRY;

            LOG_INFO("stream udp: [id]:{} [STATUS]:{} [ACTION]:EVENT [RESULT]:TIMEOUT, RECONNECT AFTER {} seconds", conn->m_context->id(), conn->m_context->status(), t);
        }
    }

    void StreamUdp::reconnect_timer_cb(evutil_socket_t fd, short events, void *arg)
    {
        StreamUdp *conn = static_cast<StreamUdp *>(arg);

        if (!conn)
            return;

        if (conn->m_context->m_reconnect_timer)
        {
            event_free(conn->m_context->m_reconnect_timer);
            conn->m_context->m_reconnect_timer = nullptr;
        }

        if (conn->m_context->m_status == StreamContext::STATUS::STATUS_CLOSED)
        {
            LOG_INFO("stream udp: [id]:{} [STATUS]:{} [ACTION]:RECONNECT [RESULT]:RECONNECTING", conn->m_context->id(), conn->m_context->status());
            conn->connect();
        }
    }
} // namespace stream