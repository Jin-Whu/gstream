#include <event2/buffer.h>
#include "streamcontext.h"
#include "streamntrip.h"

namespace stream
{
#define NTRIP_AGENT "STREAM/1.0"
#define NTRIP_RSP_OK_CLI "ICY 200 OK\r\n"         /* ntrip response: client */
#define NTRIP_RSP_OK_SVR "OK\r\n"                 /* ntrip response: server */
#define NTRIP_RSP_SRCTBL "SOURCETABLE 200 OK\r\n" /* ntrip response: source table */
#define NTRIP_RSP_TBLEND "ENDSOURCETABLE"
#define NTRIP_RSP_HTTP "HTTP/"  /* ntrip response: http */
#define NTRIP_RSP_ERROR "ERROR" /* ntrip response: error */
#define NTRIP_RSP_UNAUTH "HTTP/1.0 401 Unauthorized\r\n"
#define NTRIP_RSP_ERR_PWD "ERROR - Bad Pasword\r\n"
#define NTRIP_RSP_ERR_MNTP "ERROR - Bad Mountpoint\r\n"

    static int encbase64(char *str, const unsigned char *byte, int n)
    {
        const char table[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

        int i, j, k, b;

        for (i = j = 0; i / 8 < n;)
        {
            for (k = b = 0; k < 6; k++, i++)
            {
                b <<= 1;
                if (i / 8 < n)
                    b |= (byte[i / 8] >> (7 - i % 8)) & 0x1;
            }
            str[j++] = table[b];
        }
        while (j & 0x3)
            str[j++] = '=';
        str[j] = '\0';
        return j;
    }

    void StreamNtrip::connect()
    {
        if (!m_context)
            return;

        if (!m_context->m_evbase)
            return;
        
        bufferevent *bev = bufferevent_socket_new(m_context->m_evbase, -1, BEV_OPT_CLOSE_ON_FREE);

        if (!bev)
            return;

        bufferevent_setcb(bev, read_cb, nullptr, event_cb, this);
        bufferevent_enable(bev, EV_READ | EV_WRITE);

        timeval tv{30, 0};
        bufferevent_set_timeouts(bev, &tv, nullptr);

        reqntrip(bev);

        if (bufferevent_socket_connect_hostname(bev, nullptr, AF_INET, m_context->m_info.host.c_str(), m_context->m_info.port) < 0)
        {
            bufferevent_free(bev);
            return;
        }

        m_bev = bev;
    }

    void StreamNtrip::disconnect()
    {
        if (m_bev)
        {
            bufferevent_free(m_bev);
            m_bev = nullptr;
        }

        if (m_context->m_reconnect_timer)
        {
            event_free(m_context->m_reconnect_timer);
            m_context->m_reconnect_timer = nullptr;
        }

        m_context->m_status = StreamContext::STATUS::STATUS_CLOSED;
        LOG_INFO("stream ntrip: [id]:{} [STATUS]:{} [ACTION]:DISCONNECT [RESULT]:DISCONNECTED", m_context->id(), m_context->status());
    }

    void StreamNtrip::reqntrip(bufferevent *bev)
    {
        char buff[1024], user[512], *p = buff;

        p += sprintf(p, "GET /%s HTTP/1.0\r\n", m_context->m_info.mnt.c_str());
        p += sprintf(p, "User-Agent: NTRIP %s\r\n", NTRIP_AGENT);

        if (m_context->m_info.user.empty())
        {
            p += sprintf(p, "Accept: */*\r\n");
            p += sprintf(p, "Connection: close\r\n");
        }
        else
        {
            sprintf(user, "%s:%s", m_context->m_info.user.c_str(), m_context->m_info.pwd.c_str());
            p += sprintf(p, "Authorization: Basic ");
            p += encbase64(p, (unsigned char *)user, strlen(user));
            p += sprintf(p, "\r\n");
        }
        p += sprintf(p, "\r\n");

        evbuffer_add(bufferevent_get_output(bev), buff, p - buff);
        m_context->m_status = StreamContext::STATUS::STATUS_WAIT;
        LOG_INFO("stream ntrip: [id]:{} [STATUS]:{} [ACTION]:REQUEST [RESULT]:SEND AUTHORIZATION", m_context->id(), m_context->status());
    }

    void StreamNtrip::rspntrip()
    {
        const char *buff = (const char*)m_context->m_buff;
        const char *p = nullptr;

        if ((p = strstr(buff, NTRIP_RSP_OK_CLI))) //ok
        {
            m_context->m_status = StreamContext::STATUS::STATUS_CONNECTED;
            m_context->m_retry = 0;
            LOG_INFO("stream ntrip: [id]:{} [STATUS]:{} [ACTION]:RESPONSE [RESULT]:ICY 200 OK", m_context->id(), m_context->status());
        }
        else if ((p = strstr(buff, NTRIP_RSP_SRCTBL))) // source table
        {
            if (m_context->m_info.mnt.empty()) // source table request
            {
                m_context->m_status = StreamContext::STATUS::STATUS_CONNECTED;
                m_context->m_retry = 0;
                LOG_INFO("stream ntrip: [id]:{} [STATUS]:{} [ACTION]:RESPONSE [RESULT]:SOURCE TABLE", m_context->id(), m_context->status());
            }
            else
            {
                if (!m_context->m_retry) // first connect, no mountpoint
                {
                    LOG_INFO("stream ntrip: [id]:{} [STATUS]:{} [ACTION]:RESPONSE [RESULT]:NO MOUNTPOINT, DISCONNECT", m_context->id(), m_context->status());
                    disconnect();
                }
                else // some station will send source table when reconnected
                {
                    m_context->m_status = StreamContext::STATUS::STATUS_CONNECTED;
                    m_context->m_retry = 0;
                    LOG_INFO("stream ntrip: [id]:{} [STATUS]:{} [ACTION]:RESPONSE [RESULT]:SOURCE TABLE", m_context->id(), m_context->status());
                }
            }
        }
        else
        {
            LOG_INFO("stream ntrip: [id]:{} [STATUS]:{} [ACTION]:RESPONSE [RESULT]:ERROR, DISCONNECT", m_context->id(), m_context->status());
            disconnect();
        }
    }

    void StreamNtrip::event_cb(bufferevent *bev, short events, void *arg)
    {
        StreamNtrip *conn = static_cast<StreamNtrip *>(arg);

        if (events & (BEV_EVENT_TIMEOUT | BEV_EVENT_ERROR | BEV_EVENT_EOF))
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

    void StreamNtrip::read_cb(bufferevent *bev, void *arg)
    {
        StreamNtrip *conn = static_cast<StreamNtrip *>(arg);

        if (!conn)
            return;

        evbuffer *evbuff = bufferevent_get_input(bev);

        conn->m_context->m_buffsz = evbuffer_remove(evbuff, conn->m_context->m_buff, sizeof(conn->m_context->m_buff));

        if (conn->m_context->m_status == StreamContext::STATUS::STATUS_WAIT)
        {
            conn->rspntrip();
        }
        else if (conn->m_context->m_status == StreamContext::STATUS::STATUS_CONNECTED)
        {
            conn->m_context->decode();
        }
    }

    void StreamNtrip::reconnect_timer_cb(evutil_socket_t fd, short events, void *arg)
    {
        StreamNtrip *conn = static_cast<StreamNtrip*>(arg);

        if (!conn)
            return;

        if (conn->m_context->m_reconnect_timer)
        {
            event_free(conn->m_context->m_reconnect_timer);
            conn->m_context->m_reconnect_timer = nullptr;
        }

        if (conn->m_context->m_status == StreamContext::STATUS::STATUS_CLOSED)
        {
            LOG_INFO("stream ntrip: [id]:{} [STATUS]:{} [ACTION]:RECONNECT [RESULT]:RECONNECTING", conn->m_context->id(), conn->m_context->status());
            conn->connect();
        }
    }

}