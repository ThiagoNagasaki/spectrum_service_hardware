add_library(receiver_core
keyboard/keyboard_receiver.cpp
mcb/mcb_receiver.cpp
)

target_include_directories(receiver_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/keyboard>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/mcb>
)

target_link_libraries(receiver_core
    PUBLIC
        spdlog::spdlog
        fmt::fmt
    PRIVATE
        utils

)
