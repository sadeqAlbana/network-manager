cmake_minimum_required(VERSION 3.14)

project(demo LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Network)
add_executable(demo
  main.cpp
  testclass.h testclass.cpp
)
target_link_libraries(demo Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network SNetworkManager)

install(TARGETS demo
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

