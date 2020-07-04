#include <iostream>
#include <thread>
#include "stream.h"
#include "dataevent.h"


int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "useage: streamclient services.xml" << std::endl;
        return -1;
    }

    stream::Stream stream;
    if (!stream.init(argv[1]))
    {
        return -1;
    }

    const auto& id = stream.id();
    for (const auto &service_id : id)
    {
        for (const auto &manager_id : service_id.second)
        {
            std::shared_ptr<stream::DataEvent> ev = std::make_shared<stream::DataEvent>();
            stream.set_event(service_id.first, manager_id, ev);
        }
    }
    stream.start();

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}