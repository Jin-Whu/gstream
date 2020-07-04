#include "dataevent.h"

namespace stream
{
    void DataEvent::event_cb(const GNSSData &data)
    {
        if (data.datatype == GNSSData::GNSSDATA_TYPE::TYPE_OBS)
        {
            const auto &obs = data.obs;
            int week = 0;
            double ws = 0.;
            ws = time2gpst(obs.t, &week);
            printf("%s %d %f %zu\n", data.obs_id.c_str(), week, ws, obs.size());
        }
        else if (data.datatype == GNSSData::GNSSDATA_TYPE::TYPE_EPH)
        {
            char satid[4] = {};
            satno2id(data.sat_id, satid);
            printf("%s\n", satid);
        }
    }
}