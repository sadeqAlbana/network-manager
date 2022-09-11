cmake_minimum_required(VERSION 3.16)

target_include_directories(network-manager INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_library(network-manager STATIC
    router.cpp router.h
    networkmanager.cpp networkmanager.h
    networkaccessmanager.cpp networkaccessmanager.h
    networkresponse.cpp networkresponse.h
)



target_link_libraries(network-manager PRIVATE Qt6::Core)
target_link_libraries(network-manager PRIVATE Qt6::Network)
target_link_libraries(network-manager PRIVATE Qt6::Gui)
