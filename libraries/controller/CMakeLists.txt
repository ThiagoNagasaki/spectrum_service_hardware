add_library(controller
    protocol_controller.cpp
    device_controller.cpp
    controller_factory.cpp
)

target_include_directories(controller PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>

)

target_link_libraries(controller
    PUBLIC
        spdlog::spdlog
        fmt::fmt
    PRIVATE
        utils

)
