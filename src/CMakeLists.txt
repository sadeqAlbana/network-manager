cmake_minimum_required(VERSION 3.16)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network Gui)

add_library(SNetworkManager STATIC
    router.h router.cpp
    networkresponse.cpp networkresponse.h

    networkmanager.cpp networkmanager.h
    networkaccessmanager.cpp networkaccessmanager.h
)

target_link_libraries(SNetworkManager PRIVATE
Qt${QT_VERSION_MAJOR}::Core
Qt${QT_VERSION_MAJOR}::Network
Qt${QT_VERSION_MAJOR}::Gui)


target_include_directories(SNetworkManager INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
