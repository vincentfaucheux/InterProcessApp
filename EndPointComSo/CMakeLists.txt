cmake_minimum_required(VERSION 3.10)

project(ClientComSo LANGUAGES CXX)

# Options communes aux plugins
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(EndPointComSo SHARED
    EndPointComSo.cpp
    EndPointComSo.h
)

target_include_directories(EndPointComSo
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_include_directories(EndPointComSo
    PRIVATE ${CMAKE_SOURCE_DIR}/../PipeComSo
)

target_link_libraries(EndPointComSo
    PRIVATE PipeComSo
)

set_target_properties(EndPointComSo PROPERTIES
    OUTPUT_NAME "EndPointComSo"
)
