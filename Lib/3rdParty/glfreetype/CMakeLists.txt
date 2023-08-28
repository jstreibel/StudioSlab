project (glfreetype)

find_package(OpenGL REQUIRED)
# find_package(glfw3 3.2 REQUIRED)
find_package(Freetype REQUIRED)

file(GLOB_RECURSE glfreetype src/*.cpp include/glfreetype/*.hpp)

include_directories( ${OPENGL_INCLUDE_DIRS} )
include_directories( ${FREETYPE_INCLUDE_DIRS} )

# break above sub-folders into individual libraries
add_library(glfreetype_lib ${glfreetype})
target_link_libraries(wintest glfreetype_lib ${OPENGL_LIBRARIES} ${FREETYPE_LIBRARIES})

# compile options. Lots of redundancy here. Can prob clean up.
set(COMP_FLAGS -O3 -ffast-math -funroll-loops -Wno-ctor-dtor-privacy -fno-pic -Wno-deprecated)
target_compile_options(glfreetype_lib PUBLIC ${COMP_FLAGS})
target_compile_options(wintest PUBLIC ${COMP_FLAGS})
