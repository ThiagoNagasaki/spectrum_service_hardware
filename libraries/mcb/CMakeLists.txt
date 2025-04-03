add_library(mcb_core
    protocols/mcb_protocol.cpp
    decoder/mcb_decoder.cpp
)

target_include_directories(mcb_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/protocols>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/config>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/decoder>
)

# Se mcb_protocol usa logger (spdlog, fmt) ou utils_logger, linke:
target_link_libraries(mcb_core
    PRIVATE
        utils_logger
)
