#include <algorithm>
#include "ephstore.h"

namespace stream
{
    std::vector<eph_t> EphStore::m_ephs;
    std::vector<geph_t> EphStore::m_gephs;
    std::mutex EphStore::m_mutex[2];

    void EphStore::put(eph_t &&eph)
    {
        std::lock_guard<std::mutex> lock(m_mutex[0]);
        auto it = std::find_if(m_ephs.begin(), m_ephs.end(), [&eph](const eph_t &item) {return item.sat == eph.sat;});

        if (it != m_ephs.end())
        {
            if (timediff(eph.toe, it->toe) > 0)
            {
                *it = eph;
            }
        }
        else
        {
            m_ephs.emplace_back(eph);
        }
    }

    void EphStore::put(geph_t &&eph)
    {
        std::lock_guard<std::mutex> lock(m_mutex[1]);
        auto it = std::find_if(m_gephs.begin(), m_gephs.end(), [&eph](const geph_t &item) {return item.sat == eph.sat;});

        if (it != m_gephs.end())
        {
            if (timediff(eph.toe, it->toe) > 0)
            {
                *it = eph;
            }
        }
        else
        {
            m_gephs.emplace_back(eph);
        }
    }

    bool EphStore::get(unsigned sat, eph_t **eph)
    {
        std::lock_guard<std::mutex> lock(m_mutex[0]);
        auto it = std::find_if(m_ephs.begin(), m_ephs.end(), [sat](const eph_t &item) {return item.sat == sat;});
        if (it != m_ephs.end())
        {
            *eph = &(*it);
            return true;
        }
        *eph = nullptr;
        return false;
    }

    bool EphStore::get(unsigned sat, geph_t **eph)
    {
        std::lock_guard<std::mutex> lock(m_mutex[1]);
        auto it = std::find_if(m_gephs.begin(), m_gephs.end(), [sat](const geph_t &item) {return item.sat == sat;});
        if (it != m_gephs.end())
        {
            *eph = &(*it);
            return true;
        }
        *eph = nullptr;
        return false;
    }

    size_t EphStore::get(eph_t **eph)
    {
        size_t sz = m_ephs.size();
        *eph =  sz ? m_ephs.data() : nullptr;
        return sz;
    }

    size_t EphStore::get(geph_t **eph)
    {
        size_t sz = m_gephs.size();
        *eph = sz ? m_gephs.data() : nullptr;
        return sz;
    }
}