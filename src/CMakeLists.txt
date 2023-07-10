cmake_minimum_required(VERSION 3.16)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network Gui)

if(ENABLE_QML)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Qml)
endif()

add_library(SNetworkManager STATIC
    router.h router.cpp
    networkresponse.cpp networkresponse.h
    networkaccessmanager.cpp networkaccessmanager.h
)


set(TARGET_LIBS Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Gui)

if(ENABLE_QML)
    list(APPEND TARGET_LIBS Qt${QT_VERSION_MAJOR}::Qml)
endif()

target_link_libraries(SNetworkManager PRIVATE
${TARGET_LIBS})


target_include_directories(SNetworkManager INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
