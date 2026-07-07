cmake_minimum_required(VERSION 3.16)
project(SpellChecker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Core Widgets)
if(NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
    set(QT_MAJOR_VERSION 5)
else()
    set(QT_MAJOR_VERSION 6)
endif()

add_executable(SpellChecker
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

if(QT_MAJOR_VERSION EQUAL 6)
    target_link_libraries(SpellChecker Qt6::Core Qt6::Widgets)
else()
    target_link_libraries(SpellChecker Qt5::Core Qt5::Widgets)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/dictionary.txt
               ${CMAKE_CURRENT_BINARY_DIR}/dictionary.txt
               COPYONLY)