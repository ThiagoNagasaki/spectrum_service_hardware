# Em spectrum_service_hardware/utils/CMakeLists.txt
add_library(utils
    logger/logger.cpp
    translator/response_translator.cpp
)

target_include_directories(utils PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/enum_>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/logger>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/translator>
)

target_link_libraries(utils
    PUBLIC
        spdlog::spdlog
        fmt::fmt
)
