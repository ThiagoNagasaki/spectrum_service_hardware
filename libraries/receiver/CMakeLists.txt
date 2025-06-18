add_library(receiver_core
keyboard/keyboard_receiver.cpp
mcb/mcb_receiver.cpp
mcb/mcb_frame_awaiter.cpp
)

target_include_directories(receiver_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/keyboard>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/mcb>
    ${CMAKE_CURRENT_SOURCE_DIR}/keyboard
    ${CMAKE_CURRENT_SOURCE_DIR}/mcb
    ${CMAKE_SOURCE_DIR}/utils
)
target_include_directories(receiver_core
  PRIVATE
  ${CMAKE_SOURCE_DIR}/libraries
  ${CMAKE_SOURCE_DIR}/utils
)
target_link_libraries(receiver_core
    PUBLIC
        spdlog::spdlog
        fmt::fmt
    PRIVATE
        utils

)
