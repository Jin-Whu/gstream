project(stream)

add_library(${PROJECT_NAME} SHARED "")
target_sources(${PROJECT_NAME} PUBLIC
                               ${PROJECT_SOURCE_DIR}/src/stream.h
                               ${PROJECT_SOURCE_DIR}/src/streamevent.h
                               ${PROJECT_SOURCE_DIR}/src/gnssdata.h
                               PRIVATE
                               ${PROJECT_SOURCE_DIR}/src/stream.cpp
                               ${PROJECT_SOURCE_DIR}/src/streamsystem.h
                               ${PROJECT_SOURCE_DIR}/src/streamsystem.cpp
                               ${PROJECT_SOURCE_DIR}/src/streamservice.h
                               ${PROJECT_SOURCE_DIR}/src/streamservice.cpp
                               ${PROJECT_SOURCE_DIR}/src/streammanager.h
                               ${PROJECT_SOURCE_DIR}/src/streammanager.cpp
                               ${PROJECT_SOURCE_DIR}/src/streamio.h
                               ${PROJECT_SOURCE_DIR}/src/streamio.cpp
                               ${PROJECT_SOURCE_DIR}/src/streamcontext.h
                               ${PROJECT_SOURCE_DIR}/src/streamcontext.cpp
                               ${PROJECT_SOURCE_DIR}/src/istream.h
                               ${PROJECT_SOURCE_DIR}/src/streamtcp.h
                               ${PROJECT_SOURCE_DIR}/src/streamtcp.cpp
                               ${PROJECT_SOURCE_DIR}/src/streamudp.h
                               ${PROJECT_SOURCE_DIR}/src/streamudp.cpp
                               ${PROJECT_SOURCE_DIR}/src/streamntrip.h
                               ${PROJECT_SOURCE_DIR}/src/streamntrip.cpp
                               ${PROJECT_SOURCE_DIR}/src/streamcfg.h
                               ${PROJECT_SOURCE_DIR}/src/streamcfg.cpp
                               ${PROJECT_SOURCE_DIR}/src/streamlog.h
                               ${PROJECT_SOURCE_DIR}/src/streamlog.cpp
                               ${PROJECT_SOURCE_DIR}/src/streamdecode.h
                               ${PROJECT_SOURCE_DIR}/src/rtcmdecode.h
                               ${PROJECT_SOURCE_DIR}/src/rtcmdecode.cpp
                               ${PROJECT_SOURCE_DIR}/src/streamdata.h
                               ${PROJECT_SOURCE_DIR}/src/streaminfo.h
                               ${PROJECT_SOURCE_DIR}/src/serviceinfo.h
                               ${PROJECT_SOURCE_DIR}/src/systeminfo.h
                               ${PROJECT_SOURCE_DIR}/src/managerinfo.h)

target_link_libraries(${PROJECT_NAME} PRIVATE event pugixml spdlog::spdlog PUBLIC rtk)
target_include_directories(${PROJECT_NAME} INTERFACE ${PROJECT_SOURCE_DIR}/src)
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
