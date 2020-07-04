#include "rtcmdecode.h"

namespace stream
{
    RtcmDecode::RtcmDecode(const VERSION &ver) :m_rtcm(std::make_shared<rtcm_t>())
    {
        init_rtcm(m_rtcm.get());
        strncpy(m_rtcm->opt, "-RT_INP", 7);

        switch (ver)
        {
            case VERSION::VER_2:
                input_rtcm = input_rtcm2;
                break;
            case VERSION::VER_3:
                input_rtcm = input_rtcm3;
                break;
        default:
            break;
        }
    }

    RtcmDecode::~RtcmDecode()
    {
        free_rtcm(m_rtcm.get());
    }

    void RtcmDecode::rtcm2obs(const rtcm_t &rtcm, GNSSData &data)
    {
        data.datatype = GNSSData::GNSSDATA_TYPE::TYPE_OBS;
        ObsData &obs = data.obs;
        obs.t = rtcm.time;
        for (int i = 0; i != rtcm.obs.n; ++i)
            obs.data.emplace_back(std::move(rtcm.obs.data[i]));
    }

    void RtcmDecode::rtcm2eph(const rtcm_t &rtcm, GNSSData &data)
    {
        data.datatype = GNSSData::GNSSDATA_TYPE::TYPE_EPH;
        int sys = 0, prn = 0;
        sys = satsys(rtcm.ephsat, &prn);
        data.sat_id = rtcm.ephsat;

        switch (sys)
        {
            case SYS_GPS:
            case SYS_CMP:
            case SYS_GAL:
                data.eph = rtcm.nav.eph[rtcm.ephsat - 1];
                break;
            case SYS_GLO:
                data.geph = rtcm.nav.geph[prn - 1];
                break;
        }
    }

    void RtcmDecode::rtcm2sta(const rtcm_t &rtcm, GNSSData &data)
    {
        data.datatype = GNSSData::GNSSDATA_TYPE::TYPE_STA;
        data.sta = rtcm.sta;
    }

    DECODE_RET RtcmDecode::decode(const unsigned char *buff, size_t size, size_t &rsz, GNSSData &data)
    {
        DECODE_RET ret = DECODE_RET::TYPE_NONE;

        int iret = 0;

        data = {};

        for (rsz = 0; rsz < size; ++rsz)
        {
            iret = input_rtcm(m_rtcm.get(), buff[rsz]);
            
            switch (iret)
            {
                case 1:
                    ret = DECODE_RET::TYPE_OBS;
                    rtcm2obs(*m_rtcm, data);
                    return ret;
                case 2:
                    ret = DECODE_RET::TYPE_EPH;
                    rtcm2eph(*m_rtcm, data);
                    return ret;
                case 5:
                    ret = DECODE_RET::TYPE_STA;
                    rtcm2sta(*m_rtcm, data);
                    return ret;
                default:
                    break;
            }
        }

        return ret;
    }
}