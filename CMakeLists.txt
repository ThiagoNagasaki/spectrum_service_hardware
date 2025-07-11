cmake_minimum_required(VERSION 3.20)
project(spectrum_service_hardware LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(spdlog CONFIG REQUIRED)
get_target_property(_spdlog_includes spdlog::spdlog INTERFACE_INCLUDE_DIRECTORIES)
message(STATUS "spdlog includes: ${_spdlog_includes}")
find_package(fmt CONFIG REQUIRED)

add_subdirectory(utils)
add_subdirectory(libraries)
add_subdirectory(tests)
 

add_executable(spectrum_service main.cpp)
target_link_libraries(spectrum_service
    PUBLIC
        spdlog::spdlog
        fmt::fmt
    PRIVATE
    
        utils       
        command_core   
        protocols_core 
        receiver_core
        transport_core  
        controller   
    
)
