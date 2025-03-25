# Ajuste o nome do projeto/biblioteca conforme desejar
add_library(keyboard_protocol
    protocols/keyboard_protocol.cpp
    # Se você tiver .cpp adicional, adicione-os aqui
)

# Define onde buscar .h
#  - "protocols" para keyboard_protocol.h
#  - "config" para keyboard_constants.h
target_include_directories(keyboard_protocol PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/protocols>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/config>
)

# Linkar com as libs necessárias:
#  - transport_core (pois KeyboardProtocol usa ITransport e RS485Transport, etc.)
#  - utils_logger (pois usa Logger::instance())
#  - spdlog::spdlog e fmt::fmt, se você chama spdlog/fmt diretamente no .cpp
target_link_libraries(keyboard_protocol
    PRIVATE
        transport_core       # se seu "transport" é compilado em lib "transport_core"
        utils_logger         # para Logger Singleton
        spdlog::spdlog       # se houver chamadas diretas a spdlog no .cpp
        fmt::fmt             # se houver uso de fmt::format no .cpp
)
