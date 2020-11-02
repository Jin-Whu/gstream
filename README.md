# gstream

gstream is GNSS-oriented data stream client library based on multithread and multiplexing, which can connect thounds of rover stream meantime. User can set event to stream, once GNSS data (include observation, ephemeris etc) is decoded, callback function is called.

# feature

* communication protocol: NTRIP, TCP, UDP.
* decoding: RTCM2, RTCM3.

# structure
* Stream: the class which manager the streams.
* DataEvent: the abstract class which is the data driven class.

# using
* **rover** is one rover stream.
* **manager** is a manager of many rovers.
* **service** is a service of many managers.
* **system** is a system of many servers.
