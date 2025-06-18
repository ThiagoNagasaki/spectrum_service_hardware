# Em spectrum_service_hardware/utils/CMakeLists.txt
add_library(test
    mcb_test.cpp
    mcb_testArray.cpp
   
)
find_package(GTest CONFIG REQUIRED)

add_executable(mcb_test mcb_test.cpp)

target_link_libraries(mcb_test
    PRIVATE
        controller
        command_core
        protocols_core
        receiver_core
        transport_core
        utils
        GTest::gtest_main
        fmt::fmt
        spdlog::spdlog
)
