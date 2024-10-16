set (THIRDPARTY_LIB_BASE_NAME ${STUDIOSLAB_BASE_LIB_NAME}-3rdParty)

message(STATUS "*** freetype-gl [start] ***")
add_subdirectory(freetype-gl)
message(STATUS "*** freetype-gl [end] ***")
# add_subdirectory(NanoTeX)
# add_subdirectory(MeshLib)

set ( NUKLEAR_SOURCES
		NuklearInclude.h
		NuklearSource.h
)

set ( RESAMPLE_SOURCE
		libresample/include/*
		libresample/src/*
)

set ( IMGUI_SOURCE
		imgui/imgui.cpp
		imgui/imgui_draw.cpp
		imgui/imgui_tables.cpp
		imgui/imgui_widgets.cpp
		imgui/imgui_demo.cpp

		imgui/backends/imgui_impl_glut.cpp
		imgui/backends/imgui_impl_glfw.cpp
		imgui/backends/imgui_impl_opengl3.cpp

		imgui/misc/freetype/imgui_freetype.cpp)

add_library(${THIRDPARTY_LIB_BASE_NAME}-Imgui STATIC ${IMGUI_SOURCE})
add_compile_definitions(IMGUI_USE_WCHAR32)
target_link_libraries(${THIRDPARTY_LIB_BASE_NAME}-Imgui freetype GLU)
message(NOTICE "Added library ${THIRDPARTY_LIB_BASE_NAME}-Imgui" )
