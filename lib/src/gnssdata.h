#ifndef STREAM_GNSSDATA_H_
#define STREAM_GNSSDATA_H_

#include <string>
#include <vector>

#include <rtklib.h>

#include "streamdata.h"

namespace stream
{
    struct ObsData
    {
        ObsData(size_t n = 0) :data(n), t{} {}
        ObsData(const obs_t &obs) :ObsData(obs.n)
        {
            for (int i = 0; i != obs.n; ++i)
            {
                data[i] = obs.data[i];
            }
        }

        size_t size() const {return data.size();}

        std::vector<obsd_t> data;
        gtime_t t;
    };

    struct GNSSData : public StreamData
    {
        enum class GNSSDATA_TYPE
        {
            TYPE_NONE,
            TYPE_OBS,
            TYPE_EPH,
            TYPE_STA
        };

        GNSSData() :StreamData(DATATYPE::TYPE_GNSSDATA) {}

        GNSSDATA_TYPE datatype = GNSSDATA_TYPE::TYPE_NONE;
        ObsData obs;
        eph_t eph;
        geph_t geph;
        unsigned sat_id;
        std::string obs_id;
        sta_t sta;
    };
}

#endif