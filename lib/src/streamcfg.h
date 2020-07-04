#ifndef STREAM_STREAMCFG_H_
#define STREAM_STREAMCFG_H_

#include "streaminfo.h"
#include "systeminfo.h"

namespace stream
{
    class StreamCfg
    {
    public:
        StreamCfg() = default;
        ~StreamCfg() = default;

    public:
        bool load(const std::string &fn, SystemInfo &info);
    
    private:
        bool parse(const std::string &s, StreamInfo &info);
    };
}

#endif