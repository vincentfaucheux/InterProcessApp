find_package(pybind11 REQUIRED)

pybind11_add_module(pipecom
    pipecom_py.cpp
)

target_link_libraries(pipecom
    PRIVATE
        PipeComSo
        EndPointComSo
)

# Pour que le module trouve les .so au runtime
set_target_properties(pipecom PROPERTIES
    BUILD_RPATH "$ORIGIN"
)

# Dossier de sortie du module Python
set_target_properties(pipecom PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY
        ${CMAKE_BINARY_DIR}/python
)
