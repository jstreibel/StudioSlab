add_subdirectory(CompatibilityTests)

add_executable ( Tests
		TestsMain.cpp
		TestsApp.cpp
		WindowPanelTest.cpp

		GLFreeTypeTests.h     GLFreeTypeTests.cpp
        FourierTestWindow.h   FourierTestWindow.cpp
        BezierTests.h         BezierTests.cpp
        ModernGLTests.h       ModernGLTests.cpp
        NuklearTests.h        NuklearTests.cpp
        Graph3DTests.h        Graph3DTests.cpp
		VShapeExpansionTest.cpp
		VShapeExpansionTest.h
		LaTeXTests.cpp
		ImGuiUITest.cpp
		ImGuiUITest.h
)
target_link_libraries ( Tests ${SlabLibs} Slab )
message(NOTICE "Added executable Tests")

add_executable(SlabTests
		SlabTestsMain.cpp
		../../Slab/Studios/Common/VisualHost.cpp
		../../Slab/Studios/Common/NumericsV2TaskUtils.cpp
		../../Slab/Studios/Common/V2SimulationRunners.cpp
		../../Slab/Studios/Common/SessionLiveViewStatsV2.cpp
		../../Slab/Studios/Common/Simulations/V2/MetropolisSliceV2.cpp
		../../Slab/Studios/Common/Simulations/V2/MolecularDynamicsSliceV2.cpp
		../../Slab/Studios/Common/Monitors/V2/MetropolisFieldSnapshotListenerV2.cpp
		../../Slab/Studios/Common/Monitors/V2/MetropolisPassiveMonitorWindowV2.cpp
		../../Slab/Studios/Common/Monitors/V2/MolecularDynamicsPassiveMonitorWindowV2.cpp
)
target_link_libraries(SlabTests ${SlabLibs} Slab)
message(NOTICE "Added executable SlabTests")

add_executable(OdeintInteropTest SlabOdeintInteropTest.cpp)
target_link_libraries ( OdeintInteropTest ${SlabLibs} Slab )
message(NOTICE "Added executable OdeintInteropTest")

add_executable( SlabImGUIWindowManagerTest
		ImGuiWindowManagerTests/SlabImGUITestsMain.cpp
		BezierTests.cpp
)
target_link_libraries( SlabImGUIWindowManagerTest Slab )
message(NOTICE "Added executable SlabImGUIWindowManagerTest")
