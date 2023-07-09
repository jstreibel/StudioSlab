add_executable ( Tests
		TestsMain.cpp
		TestsApp.cpp
		WindowPanelTest.cpp
		WindowTreeBuilderTest.cpp)

target_link_libraries ( Tests	${SlabLibs} Slab-Fields-Maps-RtoR_GPU )

message(NOTICE "Added executable Tests")