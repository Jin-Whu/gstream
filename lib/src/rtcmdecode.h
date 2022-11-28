#ifndef STREAM_RTCMDECODE_H_
#define STREAM_RTCMDECODE_H_

#include <memory>

#include "streamdecode.h"

namespace stream
{

    class RtcmDecode : public StreamDecode
    {
    public:
        enum class VERSION
        {
            VER_2,
            VER_3
        };

    public:
        explicit RtcmDecode(const VERSION &ver);
        virtual ~RtcmDecode();

    public:
        DECODE_RET decode(const unsigned char *buff, size_t size, size_t &rsz, GNSSData &data);
    
    protected:
        void rtcm2obs(const rtcm_t &rtcm, GNSSData &data);
        void rtcm2eph(const rtcm_t &rtcm, GNSSData &data);
        void rtcm2sta(const rtcm_t &rtcm, GNSSData &data);

    protected:
        std::shared_ptr<rtcm_t> m_rtcm;
        int (*input_rtcm)(rtcm_t *, unsigned char);
    };

}

#endif