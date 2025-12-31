find_package(yaml-cpp REQUIRED)

# Les headers des plugins sont accessibles à tous
add_library(PipeCom INTERFACE)

target_include_directories(PipeCom
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

# Options communes aux plugins
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(PipeComSo SHARED
    PipeComSo_interface.cpp
    PipeComSo.cpp
    PipeComSo.h
    PipeComSo_config.cpp
    PipeComSo_config.h
)

target_link_libraries(PipeComSo
    PRIVATE ${yaml-cpp}
)

target_include_directories(PipeComSo
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..
)

set_target_properties(PipeComSo PROPERTIES
    OUTPUT_NAME "PipeComSo"
)
