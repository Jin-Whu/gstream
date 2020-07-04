#ifndef STREAM_STREAMDECODE_H_
#define STREAM_STREAMDECODE_H_

#include "gnssdata.h"

namespace stream
{
    enum class DECODE_RET
    {
        TYPE_NONE,
        TYPE_OBS,
        TYPE_EPH,
        TYPE_STA
    };

    class StreamDecode
    {
    public:
        StreamDecode() = default;
        virtual ~StreamDecode() = default;
    
    public:
        virtual DECODE_RET decode(const unsigned char *buff, size_t size, size_t &rsz, GNSSData &data) = 0;
    };
}

#endif