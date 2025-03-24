add_library(mcb_protocol
    protocols/mcb_protocol.cpp
   
)

target_include_directories(mcb_protocol PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/protocols>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/config>
)

# Se mcb_protocol usa logger (spdlog, fmt) ou utils_logger, linke:
target_link_libraries(mcb_protocol
    PRIVATE
        utils_logger
)
