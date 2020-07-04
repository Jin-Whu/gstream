#ifndef STREAM_STREAMDATA_H_
#define STREAM_STREAMDATA_H_

namespace stream
{
    struct StreamData
    {
        enum class DATATYPE
        {
            TYPE_NONE,
            TYPE_GNSSDATA,
            TYPE_SYSTEMINFO,
            TYPE_SERVICEINFO,
            TYPE_MANAGERINFO,
            TYPE_STREAMINFO
        };

        StreamData(const DATATYPE &type = DATATYPE::TYPE_NONE) :type(type) {}

        virtual ~StreamData() = default;

        DATATYPE type;
    };
}

#endif