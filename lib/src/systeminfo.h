#ifndef STREAM_SYSTEMINFO_H_
#define STREAM_SYSTEMINFO_H_

#include "serviceinfo.h"

namespace stream
{
    struct SystemInfo : public StreamData
    {
        SystemInfo() :StreamData(DATATYPE::TYPE_SYSTEMINFO) {}

        std::vector<ServiceInfo> services;
    };
}

#endif