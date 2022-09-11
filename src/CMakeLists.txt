cmake_minimum_required(VERSION 3.16)

find_package(Qt6  COMPONENTS Core REQUIRED)
find_package(Qt6  COMPONENTS Network REQUIRED)
find_package(Qt6  COMPONENTS Gui REQUIRED)
add_library(network-manager STATIC
    router.h
    networkmanager.cpp networkmanager.h
    networkaccessmanager.cpp networkaccessmanager.h
    networkresponse.cpp networkresponse.h
)

target_link_libraries(network-manager PRIVATE Qt6::Core)
target_link_libraries(network-manager PRIVATE Qt6::Network)
target_link_libraries(network-manager PRIVATE Qt6::Gui)

target_include_directories(network-manager INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
