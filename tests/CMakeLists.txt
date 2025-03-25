add_executable(teste_tcp_transport teste_tcp_transport.cpp)

target_link_libraries(teste_tcp_transport
    PRIVATE
        utils_logger
        transport_core
        mcb_protocol
        fmt::fmt
        spdlog::spdlog
)
