
set ( SlabLibs
		Slab-Utils

		Slab-Base-App
		Slab-Base-Backend
		Slab-Base-Controller
		Slab-Base-Graphics

		Slab-Phys
		)


include_directories(../Lib)


# ************************************************************************************************************

add_executable ( Signals-RtoR Fields/SignalProcess/SignalMain.cpp Fields/SignalProcess/Signal.cpp
		Fields/SignalProcess/JackServer.cpp Fields/SignalProcess/JackServer.h Fields/SignalProcess/OutGL.cpp Fields/SignalProcess/OutGL.h)
target_link_libraries ( Signals-RtoR ${SlabLibs} Slab-Fields-Maps-RtoR_GPU jack SOIL )
#set_property(TARGET fieldsRtoR PROPERTY CUDA_ARCHITECTURES 61)

message(NOTICE "Added executable Signals-RtoR")

# ************************************************************************************************************


add_executable ( Fields-RtoR Fields/RtoRPureSG/MainRtoR.cpp)
target_link_libraries ( Fields-RtoR ${SlabLibs} Slab-Fields-Maps-RtoR_GPU )
#set_property(TARGET fieldsRtoR PROPERTY CUDA_ARCHITECTURES 61)

message(NOTICE "Added executable Fields-RtoR")


# ************************************************************************************************************


add_executable ( Tests
		Tests/TestsMain.cpp
		Tests/TestsApp.cpp
		Tests/WindowPanelTest.cpp
		Tests/WindowTreeBuilderTest.cpp)

target_link_libraries ( Tests	${SlabLibs} Slab-Fields-Maps-RtoR_GPU )

message(NOTICE "Added executable Tests")

# ************************************************************************************************************


add_executable (Fields-RtoR-Thermal Fields/RtoRThermalSG/MainRtoRThermal.cpp ${SOURCES}  )
target_compile_definitions( Fields-RtoR-Thermal PUBLIC PERIODIC_BC ) # isso define um macro de preprocessador
target_link_libraries (Fields-RtoR-Thermal ${SlabLibs} Slab-Fields-Maps-RtoR-Periodic_GPU )

message(NOTICE "Added executable Fields-RtoR-Thermal")

# ************************************************************************************************************


add_executable (Fields-Montecarlo
		Fields/Montecarlo1+1/MontecarloMain.cpp
		Fields/Montecarlo1+1/RtoROutGLMontecarlo.cpp
		${SOURCES}  )
target_compile_definitions( Fields-Montecarlo PUBLIC PERIODIC_BC ) # isso define um macro de preprocessador
target_link_libraries (Fields-Montecarlo ${SlabLibs} Slab-Fields-Maps-RtoR-Periodic_GPU )

message(NOTICE "Added executable Fields-Montecarlo")

# ************************************************************************************************************


if( STUDIOSLAB_CUDA_SUPPORT )
	add_executable ( Fields-R2toR
			Fields/R2toR/MainR2toR.cpp
			Fields/R2toR/LeadingDelta/LeadingDelta.cpp
			Fields/R2toR/LeadingDelta/RingDeltaFunc.cpp
			Fields/R2toR/LeadingDelta/DrivenEquation.cpp
			Fields/R2toR/LeadingDelta/Allocator.cpp
			Fields/R2toR/LeadingDelta/Monitor.cpp
			Fields/R2toR/LeadingDelta/RingDeltaFunc.cu
			)
	target_link_libraries ( Fields-R2toR
			Slab-Base-Controller
			Slab-Base-Backend
			Slab-Base-App
			Slab-Base-Graphics
			Slab-Fields-Maps-R2toR_GPU
			)

	set_property(TARGET Fields-R2toR PROPERTY CUDA_STANDARD_REQUIRED 20)
	set_property(TARGET Fields-R2toR PROPERTY CUDA_ARCHITECTURES 61)

	message(NOTICE "Added executable Fields-R2toR")
else()
	message("Fields-R2toR NOT being generated: requires CUDA support.")
endif()



# ************************************************************************************************************


# add_executable ( Thermo-XY XY/main.cpp XY/XYApp.cpp)
# target_link_libraries( Thermo-XY
# 		Slab-Phys
# 		Slab-Studios-Backend
# 		Slab-Studios-Controller
# 		Slab-Studios-App )


# ************************************************************************************************************