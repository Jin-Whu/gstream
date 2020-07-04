#ifndef STREAM_ISTREAM_H_
#define STREAM_ISTREAM_H_

#include <memory>

namespace stream
{
    class StreamContext;

    class IStream
    {
    public:
        IStream(StreamContext *context) :m_context(context) {}
        virtual ~IStream() = default;

    public:
        virtual void connect() = 0;
        virtual void disconnect() = 0;
    
    protected:
        StreamContext *m_context;
    };
}

#endif