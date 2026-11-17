cmake_minimum_required(VERSION 3.10)

project(PasswordApp)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
# Или для Qt5:
# find_package(Qt5 REQUIRED COMPONENTS Widgets)

set(SOURCES
    main.cpp
    MainWindow.cpp
)

set(HEADERS
    MainWindow.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} Qt6::Widgets)
# Или для Qt5:
# target_link_libraries(${PROJECT_NAME} Qt5::Widgets)