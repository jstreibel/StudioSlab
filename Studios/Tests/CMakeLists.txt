add_subdirectory(CompatibilityTests)

add_executable ( Tests
		TestsMain.cpp
		TestsApp.cpp
		WindowPanelTest.cpp
		WindowTreeBuilderTest.cpp
		GLFreeTypeTests.cpp
		GLFreeTypeTests.h
        FourierTestWindow.cpp
        FourierTestWindow.h
        BezierTests.cpp
        BezierTests.h)

target_link_libraries ( Tests ${SlabLibs} Slab-Maps-RtoR_GPU Slab-Core freetype-gl )

message(NOTICE "Added executable Tests")