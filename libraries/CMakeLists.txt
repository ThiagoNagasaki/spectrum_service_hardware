add_library(mcb_communication
    communication/protocols/mcb_protocol.cpp
    communication/serial/rs232_transport.cpp
    communication/serial/rs485_transport.cpp
    communication/serial/tcp_transport.cpp
    communication/serial/serial_transport.cpp
)

target_include_directories(mcb_communication PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(mcb_communication PRIVATE spdlog::spdlog)
