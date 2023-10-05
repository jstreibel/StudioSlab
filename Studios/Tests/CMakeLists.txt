add_subdirectory(CompatibilityTests)

add_executable ( Tests
		TestsMain.cpp
		TestsApp.cpp
		WindowPanelTest.cpp
		WindowTreeBuilderTest.cpp

		GLFreeTypeTests.h     GLFreeTypeTests.cpp
        FourierTestWindow.h   FourierTestWindow.cpp
        BezierTests.h         BezierTests.cpp
        ModernGLTests.h       ModernGLTests.cpp
        NuklearTests.h        NuklearTests.cpp
        Graph3DTests.h        Graph3DTests.cpp

)

target_link_libraries ( Tests ${SlabLibs} Slab-Maps-RtoR_GPU Slab-Core freetype-gl )

message(NOTICE "Added executable Tests")