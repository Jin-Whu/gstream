#ifndef STREAM_EPHSTORE_H_
#define STREAM_EPHSTORE_H_

#include <map>
#include <mutex>
#include <vector>
#include "gnssdata.h"

namespace stream
{
    class EphStore
    {
    public:
        static void put(eph_t &&eph);
        static void put(geph_t &&eph);
        static bool get(unsigned sat, eph_t **eph);
        static bool get(unsigned sat, geph_t **eph);
        static size_t get(eph_t **eph);
        static size_t get(geph_t **eph);
    
    private:
       static std::vector<eph_t> m_ephs;
       static std::vector<geph_t> m_gephs;
       static std::mutex m_mutex[2];
    };
}

#endif