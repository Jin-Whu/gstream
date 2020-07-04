#include <spdlog/pattern_formatter.h>
#include "ephstore.h"
#include "rtcmdecode.h"
#include "streamtcp.h"
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
            m_binlog = spdlog::daily_logger_mt(m_id, "bins/" + m_id + ".bin", 0, 0);
            auto formatter = std::make_unique<spdlog::pattern_formatter>(spdlog::pattern_time_type::local, "");
            formatter->set_pattern("%v");
            m_binlog->set_formatter(std::move(formatter));
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
                int sys = satsys(data.sat_id, nullptr);
                if (sys == SYS_GLO) EphStore::put(std::move(data.geph));
                else EphStore::put(std::move(data.eph));

                if (streamev) streamev->event_cb(data);
            }
            else if (ret == DECODE_RET::TYPE_OBS)
            {
                data.obs_id = m_info.mnt;

                if (streamev) streamev->event_cb(data);
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