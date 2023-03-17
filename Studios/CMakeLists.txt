
set ( SlabLibs
		Slab-Common

		Slab-Studios-App
		Slab-Studios-Backend
		Slab-Studios-Controller

		Slab-Phys

		#Slab-Studios-Graphics
		#Slab-Studios-Tools
		)


# ************************************************************************************************************


add_executable ( Fields-RtoR Fields/MainRtoR.cpp)
target_link_libraries ( Fields-RtoR	${SlabLibs} Slab-Fields-Maps-RtoR_GPU )
#set_property(TARGET fieldsRtoR PROPERTY CUDA_ARCHITECTURES 61)


# ************************************************************************************************************


add_executable ( Tests.app
		Tests/TestsMain.cpp
		Tests/TestsApp.cpp
		Tests/WindowPanelTest.cpp
		Tests/WindowTreeBuilderTest.cpp)

target_link_libraries ( Tests.app	${SlabLibs} Slab-Fields-Maps-RtoR_GPU )


# ************************************************************************************************************


add_executable (Fields-RtoR-Thermal Fields/MainRtoRThermal.cpp ${SOURCES}  )
add_compile_definitions(TARGET Fields-RtoR-Thermal PERIODIC_BC ) # isso define um macro de preprocessador
target_link_libraries (Fields-RtoR-Thermal ${SlabLibs} Slab-Fields-Maps-RtoR-Periodic_GPU )


# ************************************************************************************************************


add_executable (Fields-Montecarlo
		Fields/Montecarlo1+1/MontecarloMain.cpp
		Fields/Montecarlo1+1/Montecarlo.cpp
		Fields/Montecarlo1+1/RtoROutGLMontecarlo.cpp
		${SOURCES}  )
add_compile_definitions(TARGET Fields-Montecarlo PERIODIC_BC ) # isso define um macro de preprocessador
target_link_libraries (Fields-Montecarlo ${SlabLibs} Slab-Fields-Maps-RtoR-Periodic_GPU )


# ************************************************************************************************************


add_executable ( Fields-R2toR Fields/MainR2toR.cpp )
target_link_libraries ( Fields-R2toR Slab-Fields-Maps-R2toR_GPU )
set_property(TARGET Fields-R2toR PROPERTY CUDA_ARCHITECTURES 61)


# ************************************************************************************************************


# add_executable ( Thermo-XY XY/main.cpp XY/XYApp.cpp)
# target_link_libraries( Thermo-XY
# 		Slab-Phys
# 		Slab-Studios-Backend
# 		Slab-Studios-Controller
# 		Slab-Studios-App )


# ************************************************************************************************************