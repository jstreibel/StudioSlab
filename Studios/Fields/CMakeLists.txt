# ************************************************************************************************************


add_executable ( Signals-RtoR
		SignalProcess/SignalMain.cpp
		SignalProcess/Signal.cpp
		SignalProcess/JackServer.cpp
		SignalProcess/JackServer.h
		SignalProcess/OutGL.cpp
		SignalProcess/OutGL.h)
target_link_libraries ( Signals-RtoR ${SlabLibs} Slab-Fields-Maps-RtoR_GPU jack SOIL )
#set_property(TARGET fieldsRtoR PROPERTY CUDA_ARCHITECTURES 61)

message(NOTICE "Added executable Signals-RtoR")


# ************************************************************************************************************


add_executable ( Fields-RtoR
		RtoRPureSG/MainRtoR.cpp)
target_link_libraries ( Fields-RtoR ${SlabLibs} Slab-Fields-Maps-RtoR_GPU )
#set_property(TARGET fieldsRtoR PROPERTY CUDA_ARCHITECTURES 61)

message(NOTICE "Added executable Fields-RtoR")


# ************************************************************************************************************


add_executable (Fields-RtoR-Thermal
		RtoRThermalSG/MainRtoRThermal.cpp ${SOURCES}  )
target_compile_definitions( Fields-RtoR-Thermal PUBLIC PERIODIC_BC ) # isso define um macro de preprocessador
target_link_libraries (Fields-RtoR-Thermal ${SlabLibs} Slab-Fields-Maps-RtoR-Periodic_GPU )

message(NOTICE "Added executable Fields-RtoR-Thermal")


# ************************************************************************************************************


add_executable (Fields-Montecarlo
		Montecarlo1+1/MontecarloMain.cpp
		Montecarlo1+1/RtoROutGLMontecarlo.cpp
		${SOURCES}  )
target_compile_definitions( Fields-Montecarlo PUBLIC PERIODIC_BC ) # isso define um macro de preprocessador
target_link_libraries (Fields-Montecarlo ${SlabLibs} Slab-Fields-Maps-RtoR-Periodic_GPU )

message(NOTICE "Added executable Fields-Montecarlo")


# ************************************************************************************************************


if( STUDIOSLAB_CUDA_SUPPORT )
	add_executable ( Fields-R2toR
			R2toR/MainR2toR.cpp
			R2toR/LeadingDelta/LeadingDelta.cpp
			R2toR/LeadingDelta/RingDeltaFunc.cpp
			R2toR/LeadingDelta/DrivenEquation.cpp
			R2toR/LeadingDelta/Allocator.cpp
			R2toR/LeadingDelta/Monitor.cpp
			R2toR/LeadingDelta/RingDeltaFunc.cu
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