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

        StreamInfo() :StreamData(DATATYPE::TYPE_STREAMINFO), stype(StreamType::TYPE_NONE), encode(StreamEncode::TYPE_NONE), port(0), nmea(0), lat(0.), lon(0.), alt(0.) {}

        StreamType stype;      /// < stream type
        StreamEncode encode;  /// < stream encode
        std::string id;       /// < stream id
        std::string mnt;      /// < stream mount point
        std::string host;     /// < stream host
        unsigned port;    /// < stream port
        std::string user;     /// < stream user
        std::string pwd;      /// < stream password
        int nmea;             /// < nmea
        double lat;      /// < stream lat
        double lon;      /// < stream lon
        double alt;     /// < stream alt
    };
}

#endif
