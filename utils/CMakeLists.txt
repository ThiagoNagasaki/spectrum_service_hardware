# Em spectrum_service_hardware/utils/CMakeLists.txt
add_library(utils_logger
    logger.cpp
)

target_include_directories(utils_logger PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/enum_>
)

target_link_libraries(utils_logger
    PUBLIC
        spdlog::spdlog
        fmt::fmt
)
