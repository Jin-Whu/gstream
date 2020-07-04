#ifndef STREAM_SERVICEINFO_H_
#define STREAM_SERVICEINFO_H_

#include "managerinfo.h"

namespace stream
{
    struct ServiceInfo : public StreamData
    {
        ServiceInfo() :StreamData(DATATYPE::TYPE_SERVICEINFO) {}

        std::string id; /// < user id
        std::vector<ManagerInfo> managers; /// < managers
    };
}

#endif