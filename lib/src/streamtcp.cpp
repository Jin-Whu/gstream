#include <event2/buffer.h>
#include "streamcontext.h"
#include "streamtcp.h"

namespace stream
{
    void StreamTcp::connect()
    {
        if (!m_context)
            return;

        if (!m_context->m_evbase)
            return;

        bufferevent *bev = bufferevent_socket_new(m_context->m_evbase, -1, BEV_OPT_CLOSE_ON_FREE);

        if (!bev) return;

        bufferevent_setwatermark(bev, EV_READ, 128, 0);
        bufferevent_setcb(bev, read_cb, NULL, event_cb, this);
        bufferevent_enable(bev, EV_READ);

        timeval tv {30, 0};
        bufferevent_set_timeouts(bev, &tv, NULL);

        if (bufferevent_socket_connect_hostname(bev, NULL, AF_INET, m_context->m_info.host.c_str(), m_context->m_info.port) < 0)
        {
            bufferevent_free(bev);
            return;
        }

        m_bev = bev;
    }

    void StreamTcp::disconnect()
    {
        if (m_bev)
        {
            bufferevent_free(m_bev);
            m_bev = NULL;
        }

        if (m_context->m_reconnect_timer)
        {
            event_free(m_context->m_reconnect_timer);
            m_context->m_reconnect_timer = NULL;
        }

        m_context->m_status = StreamContext::STATUS::STATUS_CLOSED;
        LOG_INFO("stream tcp: [id]:{} [STATUS]:{} [ACTION]:DISCONNECT [RESULT]:DISCONNECTED", m_context->id(), m_context->status());
    }

    void StreamTcp::read_cb(bufferevent *bev, void *arg)
    {
        StreamTcp *conn = static_cast<StreamTcp*>(arg);

        if (!conn) return;

        conn->m_context->m_retry = 0;

        evbuffer *evbuff = bufferevent_get_input(bev);

        conn->m_context->m_buffsz = evbuffer_remove(evbuff, conn->m_context->m_buff, sizeof(conn->m_context->m_buff));
        conn->m_context->decode();
    }

    void StreamTcp::event_cb(bufferevent *bev, short events, void *arg)
    {
        StreamTcp *conn = static_cast<StreamTcp*>(arg);
        if (events & BEV_EVENT_CONNECTED)
        {
            conn->m_context->m_status = StreamContext::STATUS::STATUS_CONNECTED;
        }
        else if (events & (BEV_EVENT_TIMEOUT | BEV_EVENT_ERROR | BEV_EVENT_EOF))
        {
            conn->disconnect();

            long t = 2 << conn->m_context->m_retry;
            timeval tv{t, 0};
            event *ev = evtimer_new(conn->m_context->m_evbase, reconnect_timer_cb, conn);
            evtimer_add(ev, &tv);
            conn->m_context->m_reconnect_timer = ev;
            conn->m_context->m_retry = conn->m_context->m_retry < MAX_RETRY ? conn->m_context->m_retry + 1 : MAX_RETRY;

            if (events & BEV_EVENT_TIMEOUT)
            {
                LOG_INFO("stream ntrip: [id]:{} [STATUS]:{} [ACTION]:EVENT [RESULT]:TIMEOUT, RECONNECT AFTER {} seconds", conn->m_context->id(), conn->m_context->status(), t);
            }
            else if (events & BEV_EVENT_ERROR)
            {
                LOG_INFO("stream ntrip: [id]:{} [STATUS]:{} [ACTION]:EVENT [RESULT]:ERROR, RECONNECT AFTER {} seconds", conn->m_context->id(), conn->m_context->status(), t);
            }
            else if (events & BEV_EVENT_EOF)
            {
                LOG_INFO("stream ntrip: [id]:{} [STATUS]:{} [ACTION]:EVENT [RESULT]:EOF, RECONNECT AFTER {} seconds", conn->m_context->id(), conn->m_context->status(), t);
            }
        }
    }

    void StreamTcp::reconnect_timer_cb(evutil_socket_t fd, short events, void *arg)
    {
        StreamTcp *conn = static_cast<StreamTcp*>(arg);

        if (!conn)
            return;

        if (conn->m_context->m_reconnect_timer)
        {
            event_free(conn->m_context->m_reconnect_timer);
            conn->m_context->m_reconnect_timer = NULL;
        }

        if (conn->m_context->m_status == StreamContext::STATUS::STATUS_CLOSED)
        {
            LOG_INFO("stream ntrip: [id]:{} [STATUS]:{} [ACTION]:RECONNECT [RESULT]:RECONNECTING", conn->m_context->id(), conn->m_context->status());
            conn->connect();
        }
    }
}