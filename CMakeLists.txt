cmake_minimum_required(VERSION 3.20)
project(spectrum_service_hardware VERSION 1.0 LANGUAGES CXX)

include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)  # Importa as configurações do Conan

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Adiciona as dependências geradas pelo Conan
find_package(spdlog REQUIRED)

# Adiciona os subdiretórios para incluir suas respectivas configurações
add_subdirectory(libraries)
add_subdirectory(service)
add_subdirectory(tests)

# Define o executável principal (se necessário)
add_executable(spectrum_service main.cpp)
target_link_libraries(spectrum_service PRIVATE spdlog::spdlog)
