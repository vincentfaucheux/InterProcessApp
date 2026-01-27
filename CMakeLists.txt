set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

cmake_minimum_required(VERSION 3.21)
project(DynamicPluginsQt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(PipeComSo)
add_subdirectory(EndPointComSo)
add_subdirectory(PipeComLaunch)
add_subdirectory(PythonBindings)
