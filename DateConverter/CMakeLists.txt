cmake_minimum_required(VERSION 3.16)
project(DateConverter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

add_executable(DateConverter
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    dates.txt
)

target_link_libraries(DateConverter Qt6::Core Qt6::Widgets)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/dates.txt)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/dates.txt
                   ${CMAKE_CURRENT_BINARY_DIR}/dates.txt
                   COPYONLY)
endif()