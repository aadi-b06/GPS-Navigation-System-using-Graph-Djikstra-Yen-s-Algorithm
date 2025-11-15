cmake_minimum_required(VERSION 3.10)

add_executable(pune-gps-backend
    main.cpp
    graph.cpp
    dijkstra.cpp
    yen.cpp
)

target_include_directories(pune-gps-backend PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/asio-asio-1-30-2/asio/include"
)

target_link_libraries(pune-gps-backend ws2_32 iphlpapi mswsock)
