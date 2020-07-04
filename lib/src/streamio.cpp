#ifdef _WIN32
#include <winsock.h>
#endif
#include "streamlog.h"
#include "streamio.h"

namespace stream
{

    bool StreamIo::init()
    {
        #ifdef _WIN32
            // initializing winsock
            WSADATA wsadata;
            if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
            {
                return false;
            }
        #endif

        return true;
    }

    void StreamIo::uninit()
    {
        #ifdef _WIN32
            WSACleanup();
        #endif
    }
}