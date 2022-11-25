#ifndef STREAM_STREAMCONTEXT_H_
#define STREAM_STREAMCONTEXT_H_

#include <fstream>
#include <string>

#include <event2/event.h>
#include <event2/event_struct.h>

#include "istream.h"
#include "streaminfo.h"
#include "streamdecode.h"
#include "streamevent.h"
#include "streamlog.h"

namespace stream
{
    class StreamManager;

    constexpr unsigned MAX_RETRY = 5;

    class StreamContext
    {
        friend class StreamTcp;
        friend class StreamUdp;
        friend class StreamNtrip;

        enum class STATUS
        {
            STATUS_CLOSED,
            STATUS_WAIT,
            STATUS_CONNECTED
        };

    public:
        enum class ACTION
        {
            TYPE_START,
            TYPE_STOP
        };

    public:
        StreamContext(const std::string &id, const StreamInfo &info, const StreamManager *manager);
        ~StreamContext();

    public:
        void start();
        void stop();
        std::string id() const;
        STATUS status() const;
    
    private:
        void decode();
    
    private:
        std::string m_id;
        const StreamManager *m_manager; /// < belong to manager
        std::shared_ptr<IStream> m_stream;   /// < stream
        std::shared_ptr<StreamBinLog> m_binlog; /// < stream binary logger
        std::shared_ptr<StreamDecode> m_decoder;  /// < decoder
        unsigned char m_buff[4096];       /// < buff
        size_t m_buffsz;                  /// < buff size
        event_base *m_evbase;  /// < evbase belong to manager
        event *m_reconnect_timer;      /// reconnect timer event
        StreamInfo m_info;  /// < stream info
        unsigned m_retry;   /// < stream reconnect retry
        STATUS m_status;    /// < stream status
    };
}

#endif