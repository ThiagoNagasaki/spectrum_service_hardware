add_library(service_manager service_manager.cpp)
target_include_directories(service_manager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(service_manager PRIVATE mcb_communication)
