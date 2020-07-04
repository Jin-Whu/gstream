project(streamclient)

add_executable(${PROJECT_NAME} "")
target_sources(${PROJECT_NAME} PRIVATE
                               ${PROJECT_SOURCE_DIR}/src/main.cpp
                               ${PROJECT_SOURCE_DIR}/src/dataevent.h
                               ${PROJECT_SOURCE_DIR}/src/dataevent.cpp)
target_link_libraries(${PROJECT_NAME} stream)
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})