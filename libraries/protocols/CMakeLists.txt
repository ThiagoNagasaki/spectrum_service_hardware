add_library(protocols_core
    mcb_keyboard/mcb_keyboard_protocol.cpp 
)

target_include_directories(protocols_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/mcb_keyboard> 
)

target_link_libraries(protocols_core
    PUBLIC
        spdlog::spdlog
        fmt::fmt
    PRIVATE
        utils

)
