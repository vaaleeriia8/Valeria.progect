cmake_minimum_required(VERSION 3.16)
project(EnglishTranslator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Ищем Qt6 (НЕ Qt5!)
find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

add_executable(EnglishTranslator
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

target_link_libraries(EnglishTranslator Qt6::Core Qt6::Widgets)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/en_ru.txt)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/en_ru.txt
                   ${CMAKE_CURRENT_BINARY_DIR}/en_ru.txt
                   COPYONLY)
endif()