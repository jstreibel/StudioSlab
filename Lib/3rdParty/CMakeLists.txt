cmake_minimum_required ( VERSION 3.14 FATAL_ERROR )

set (THIRDPARTY_LIB_BASE_NAME ${STUDIOSLAB_BASE_LIB_NAME}-3rdParty)

set ( IMGUI_SOURCE
		imgui/imgui.cpp
		imgui/imgui_draw.cpp
		imgui/imgui_tables.cpp
		imgui/imgui_widgets.cpp
		imgui/imgui_demo.cpp

		imgui/backends/imgui_impl_glut.cpp
		imgui/backends/imgui_impl_opengl3.cpp

		imgui/misc/freetype/imgui_freetype.cpp)



add_library(${THIRDPARTY_LIB_BASE_NAME}-Imgui STATIC ${IMGUI_SOURCE})
target_link_libraries(${THIRDPARTY_LIB_BASE_NAME}-Imgui freetype)



#target_link_libraries(${STUDIOS_LIB_BASE_NAME}-Tools
#		resample
#		boost_timer
#		Slab-Common
#		)
