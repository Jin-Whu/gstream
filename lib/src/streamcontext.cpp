#include "rtcmdecode.h"
#include "streamtcp.h"
#include "streamudp.h"
#include "streamntrip.h"
#include "streammanager.h"
#include "streamcontext.h"

namespace stream
{
    StreamContext::StreamContext(const std::string &id, const StreamInfo &info, const StreamManager *manager) :m_id(id), m_info(info), m_status(STATUS::STATUS_CLOSED), m_manager(manager), m_evbase(nullptr), m_reconnect_timer(nullptr), m_retry(0)
    {
        switch (m_info.stype)
        {
        case StreamInfo::StreamType::TYPE_TCP:
            m_stream = std::make_shared<StreamTcp>(this);
            break;
        case StreamInfo::StreamType::TYPE_NTRIP:
            m_stream = std::make_shared<StreamNtrip>(this);
            break;
        case StreamInfo::StreamType::TYPE_UDP:
            m_stream = std::make_shared<StreamUdp>(this);
            break;
        default:
            m_stream = nullptr;
            break;
        }

        switch (m_info.encode)
        {
        case StreamInfo::StreamEncode::TYPE_RTCM2:
            m_decoder = std::make_shared<RtcmDecode>(RtcmDecode::VERSION::VER_2);
            break;
        case StreamInfo::StreamEncode::TYPE_RTCM3:
            m_decoder = std::make_shared<RtcmDecode>(RtcmDecode::VERSION::VER_3);
            break;
        default:
            m_decoder = nullptr;
            break;
        }

        if ((StreamLog::flag() & StreamLog::BIN) == StreamLog::BIN)
        {
            m_binlog = std::make_shared<StreamBinLog>();
            m_binlog->init();
        }

        if (m_manager)
        {
            m_evbase = m_manager->evbase();
        }
    }

    StreamContext::~StreamContext()
    {
        stop();
    }

    void StreamContext::start()
    {
        if (m_status == STATUS::STATUS_CLOSED)
        {
            if (m_stream) m_stream->connect();
        }
    }

    void StreamContext::stop()
    {
        if (m_status != STATUS::STATUS_CLOSED)
        {
            if (m_stream) m_stream->disconnect();
        }
    }

    void StreamContext::decode()
    {
        std::shared_ptr<StreamEvent> streamev = nullptr;
        if (m_manager)
        {
            streamev = m_manager->streamev();
        }

        if (m_binlog)
        {
            m_binlog->info(std::string(m_buff, m_buff + m_buffsz));
        }

        const unsigned char *p = m_buff;
        size_t size = m_buffsz;
        size_t rsz;
        while (size)
        {
            GNSSData data;
            auto ret = m_decoder->decode(p, size, rsz, data);

            if (ret == DECODE_RET::TYPE_EPH)
            {
                if (streamev)
                    streamev->event_cb(data);
            }
            else if (ret == DECODE_RET::TYPE_OBS)
            {
                data.sta_id = m_info.mnt;

                if (streamev)
                    streamev->event_cb(data);
            }
            else if (ret == DECODE_RET::TYPE_STA)
            {
                if (streamev)
                    streamev->event_cb(data);
            }
            size -= rsz;
            p += rsz;
        }
    }

    std::string StreamContext::id() const
    {
        return m_id;
    }

    StreamContext::STATUS StreamContext::status() const
    {
        return m_status;
    }
}