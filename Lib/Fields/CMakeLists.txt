

set(FIELDS_BASE_LIB_NAME ${STUDIOSLAB_BASE_LIB_NAME}-Fields)

add_subdirectory(Mappings)

set ( OpenGL_GL_PREFERENCE GLVND )
find_package(OpenMP)
find_package(Boost COMPONENTS program_options REQUIRED)
find_package(GLUT)
find_package(OpenGL)

include_directories (
		./
		../
		#${CMAKE_SOURCE_DIR}
		#${CMAKE_CURRENT_BINARY_DIR}
		#${PWD}
)


#add_library(Fields-Core STATIC ${SOURCES})
# target_link_libraries (Fields GL glut boost_program_options boost_timer)
#set_property(TARGET Fields-Core PROPERTY CUDA_ARCHITECTURES 61)


