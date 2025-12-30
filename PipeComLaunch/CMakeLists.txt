find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(yaml-cpp REQUIRED)

add_executable(PipeComLaunch
    main.cpp
    mainwindow.cpp
    mainwindow.h
    main_config.h
    main_config.cpp
)

target_include_directories(PipeComLaunch
    PRIVATE ${CMAKE_SOURCE_DIR}/PipeComSo
)

target_link_libraries(PipeComLaunch
    Qt6::Widgets
    dl
)

target_link_libraries(PipeComLaunch
    yaml-cpp
)

set_target_properties(PipeComLaunch PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
