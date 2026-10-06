cmake_minimum_required (VERSION 3.8)

project ("Lab2_OS")

add_executable (Lab2_OS "Lab2_OS.cpp")

target_compile_definitions(Lab2_OS PRIVATE _CRT_SECURE_NO_WARNINGS)

set_target_properties(Lab2_OS PROPERTIES 
    CMAKE_MBCS_SET_LOCALE TRUE
)