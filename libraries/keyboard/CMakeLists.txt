# Ajuste o nome do projeto/biblioteca conforme desejar
add_library(keyboard_core
    protocols/keyboard_protocol.cpp
    decoder/keyboard_decoder.cpp
    receiver/keyboard_receiver.cpp
)

# Define onde buscar .h
target_include_directories(keyboard_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/protocols>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/config>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/decoder>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/receiver>
)

# Linkar com as libs necessárias:
target_link_libraries(keyboard_core
    PRIVATE
        transport_core       # se seu "transport" é compilado em lib "transport_core"
        utils_logger         # para Logger Singleton
        spdlog::spdlog       # se houver chamadas diretas a spdlog no .cpp
        fmt::fmt             # se houver uso de fmt::format no .cpp
)
