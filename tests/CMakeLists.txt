cmake_minimum_required(VERSION 3.20)
project(tests LANGUAGES CXX)

# Procura pelo Google Test
find_package(GTest CONFIG REQUIRED)

# Crie seu executável de testes
add_executable(teste_esteira teste_esteira.cpp)

target_link_libraries(teste_esteira
    PRIVATE
        utils_logger        # Sua biblioteca de logger
        transport_core      # Sua biblioteca de transportes
        mcb_core        # Biblioteca do protocolo MCB, se for necessário
        keyboard_core
        GTest::gtest_main   # Linka com o main padrão do GTest
        fmt::fmt
        spdlog::spdlog
)

include(GoogleTest)
#gtest_discover_tests(test_all_commands)
