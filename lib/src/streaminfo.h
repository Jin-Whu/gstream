#ifndef STREAM_STREAMINFO_H_
#define STREAM_STREAMINFO_H_

#include <string>
#include "streamdata.h"

namespace stream
{
    struct StreamInfo : public StreamData
    {
        enum class StreamType
        {
            TYPE_NONE,
            TYPE_TCP,
            TYPE_UDP,
            TYPE_NTRIP
        };

        enum class StreamEncode
        {
            TYPE_NONE,
            TYPE_RTCM2,
            TYPE_RTCM3
        };

        StreamInfo() :StreamData(DATATYPE::TYPE_STREAMINFO) {}

        StreamType stype = StreamType::TYPE_NONE;      /// < stream type
        StreamEncode encode = StreamEncode::TYPE_NONE;  /// < stream encode
        std::string id;       /// < stream id
        std::string mnt;      /// < stream mount point
        std::string host;     /// < stream host
        unsigned port = 0;    /// < stream port
        std::string user;     /// < stream user
        std::string pwd;      /// < stream password
        int nmea = 0;             /// < nmea
        double lat = 0.;      /// < stream lat
        double lon = 0.;      /// < stream lon
        double alt = 0.;     /// < stream alt
    };
}

#endif