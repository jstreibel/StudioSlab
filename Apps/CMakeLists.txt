
set ( SlabLibs
		Slab-Common

		Slab-Studios-App
		Slab-Studios-Backend
		Slab-Studios-Controller
		Slab-Studios-CoreMath

		#Slab-Studios-Graphics
		#Slab-Studios-Tools
		)


add_executable ( Fields-RtoR MainRtoR.cpp)
target_link_libraries ( Fields-RtoR	${SlabLibs} )
#set_property(TARGET fieldsRtoR PROPERTY CUDA_ARCHITECTURES 61)

add_executable ( Tests.app
		TestsMain.cpp
		Tests/TestsApp.cpp
		Tests/WindowPanelTest.cpp
		Tests/WindowTreeBuilderTest.cpp )

target_link_libraries ( Tests.app	${SlabLibs} Slab-Fields-Maps-RtoR_GPU )

add_executable (Fields-RtoR-PeriodicBC MainRtoRThermal.cpp ${SOURCES}  )
add_compile_definitions(TARGET Fields-RtoR-PeriodicBC PERIODIC_BC=true) # isso define um macro de preprocessador
target_link_libraries (Fields-RtoR-PeriodicBC ${SlabLibs} Slab-Fields-Maps-RtoR-Periodic_GPU )

#
#add_executable ( fieldsR2toR MainR2toR.cpp ${SOURCES} ${R2TOR_SOURCES} )
#target_link_libraries ( fieldsR2toR GL glut boost_program_options boost_timer ${VTK_LIBRARIES})
##set_property(TARGET fieldsR2toR PROPERTY CUDA_ARCHITECTURES 61)