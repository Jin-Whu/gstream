#ifndef STREAM_MANAGERINFO_H_
#define STREAM_MANAGERINFO_H_

#include <string>
#include <vector>
#include "streaminfo.h"

namespace stream
{
    struct ManagerInfo : public StreamData
    {
        ManagerInfo() :StreamData(DATATYPE::TYPE_MANAGERINFO) {}
        std::string id;  /// < manager id
        std::vector<StreamInfo> rovers;  /// < rover stations
    };
}

#endif