cmake_minimum_required ( VERSION 3.14 FATAL_ERROR )

set (STUDIOS_LIB_BASE_NAME ${STUDIOSLAB_BASE_LIB_NAME}-Studios)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY /home/joao/Developer/StudioSlab/Slab/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY /home/joao/Developer/StudioSlab/Slab/bin)

add_subdirectory(Tools)
add_subdirectory(Controller)
add_subdirectory(Backend)
add_subdirectory(App)

add_subdirectory(Graphics)

add_subdirectory(CoreMath) # depends on Backend (Integrator is a Backend::Program) and Controller (for Interface)

