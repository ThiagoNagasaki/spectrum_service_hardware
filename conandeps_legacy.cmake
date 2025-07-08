message(STATUS "Conan: Using CMakeDeps conandeps_legacy.cmake aggregator via include()")
message(STATUS "Conan: It is recommended to use explicit find_package() per dependency instead")

find_package(GTest)
find_package(spdlog)
find_package(fmt)

set(CONANDEPS_LEGACY  gtest::gtest  spdlog::spdlog  fmt::fmt )