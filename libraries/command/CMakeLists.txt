set(COMMAND_MCB_SOURCES
    mcb/read_firmware_command.cpp
)
add_library(command_core
      ${COMMAND_MCB_SOURCES}
)

target_include_directories(command_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/keyboard>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/mcb>
)

target_link_libraries(command_core
    PUBLIC
        spdlog::spdlog
        fmt::fmt
    PRIVATE
        utils

)
