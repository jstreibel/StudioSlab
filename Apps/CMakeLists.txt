
add_executable ( Fields-RtoR MainRtoR.cpp)

target_link_libraries ( Fields-RtoR
		Slab-Common

		Slab-Studios-App
		Slab-Studios-Backend
		Slab-Studios-Controller
		Slab-Studios-CoreMath

		Slab-Fields-Maps-RtoR_GPU

		#Slab-Studios-Graphics
		#Slab-Studios-Tools

		)

#set_property(TARGET fieldsRtoR PROPERTY CUDA_ARCHITECTURES 61)

#add_executable (fieldsRtoR-thermal MainRtoRThermal.cpp ${SOURCES}  )
#add_compile_definitions(TARGET fieldsRtoR-thermal PERIODIC_BC=true) # isso define um macro de preprocessador
#target_link_libraries (fieldsRtoR-thermal GL glut boost_program_options boost_timer)
##set_property(TARGET fieldsRtoR-thermal PROPERTY CUDA_ARCHITECTURES 61)
#
#add_executable ( fieldsR2toR MainR2toR.cpp ${SOURCES} ${R2TOR_SOURCES} )
#target_link_libraries ( fieldsR2toR GL glut boost_program_options boost_timer ${VTK_LIBRARIES})
##set_property(TARGET fieldsR2toR PROPERTY CUDA_ARCHITECTURES 61)