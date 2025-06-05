add_library(mcb_core
    protocols/mcb_protocol.cpp
    decoder/mcb_decoder.cpp
    receiver/mcb_receiver.cpp
)

target_include_directories(mcb_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/protocols>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/config>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/decoder>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/receiver>
)

# Se mcb_protocol usa logger (spdlog, fmt) ou utils_logger, linke:
target_link_libraries(mcb_core
    PRIVATE
    transport_core       # se seu "transport" é compilado em lib "transport_core"
    utils_logger         # para Logger Singleton
    spdlog::spdlog       # se houver chamadas diretas a spdlog no .cpp
    fmt::fmt             # se houver uso de fmt::format no .cpp
)
