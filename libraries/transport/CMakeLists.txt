add_library(transport_core
    network/tcp_transport.cpp
    serial/rs232_transport.cpp
    serial/rs485_transport.cpp
    serial/serial_transport.cpp
)

target_include_directories(transport_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/enum_>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/interface>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/network>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/serial>
)

target_link_libraries(transport_core
    PUBLIC
        spdlog::spdlog
        fmt::fmt
    PRIVATE
        utils

)
