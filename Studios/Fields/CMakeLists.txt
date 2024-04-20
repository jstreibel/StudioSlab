
add_subdirectory(RtoR-Modes)

# ************************************************************************************************************


#add_executable ( Signals-RtoR
#		SignalProcess/SignalMain.cpp
#		SignalProcess/Signal.cpp
#		SignalProcess/JackServer.cpp
#		SignalProcess/JackServer.h
#		SignalProcess/OutGL.cpp
#		SignalProcess/OutGL.h)
#target_link_libraries ( Signals-RtoR ${SlabLibs} Slab-Maps-RtoR_GPU jack SOIL )
## set_property(TARGET fieldsRtoR PROPERTY CUDA_ARCHITECTURES 61)
#
#message(NOTICE "Added executable Signals-RtoR")


# ************************************************************************************************************

message(NOTICE "SlabLibs: ${SlabLibs}")

add_executable ( Fields-RtoR
		RtoR-PureSG/MainRtoR.cpp

		RtoR-PureSG/SingleFormations/input-perturbed.cpp
		RtoR-PureSG/SingleFormations/input-shockwave.cpp
		RtoR-PureSG/SingleFormations/InputSingleOscillon.cpp

		RtoR-PureSG/InteractingFormations/input-sym-oscillons.cpp
		RtoR-PureSG/InteractingFormations/input-general-oscillons.cpp
)
target_link_libraries ( Fields-RtoR ${SlabLibs} Slab-Maps-RtoR_GPU )
#set_property(TARGET Fields-RtoR PROPERTY CUDA_ARCHITECTURES 72)

message(NOTICE "Added executable Fields-RtoR")


# ************************************************************************************************************


add_executable (Fields-RtoR-Thermal
		RtoR-Thermal/MainRtoRThermal.cpp

		RtoR-Thermal/ThermalBuilder.h							RtoR-Thermal/ThermalBuilder.cpp
		#RtoR-Thermal/ThermalMonitor.h							RtoR-Thermal/ThermalMonitor.cpp

		RtoR-Thermal/Studies/ManyOscillonsBuilder.h				RtoR-Thermal/Studies/ManyOscillonsBuilder.cpp
		RtoR-Thermal/Studies/MachineGunBuilder.h				RtoR-Thermal/Studies/MachineGunBuilder.cpp
		RtoR-Thermal/Studies/StatisticalBuilder.h				RtoR-Thermal/Studies/StatisticalBuilder.cpp
		RtoR-Thermal/Studies/RandomEnergyOverDotPhiBuilder.h	RtoR-Thermal/Studies/RandomEnergyOverDotPhiBuilder.cpp
		)
target_link_libraries (Fields-RtoR-Thermal Slab-Graphics Slab-Models Slab-Maps-RtoR_GPU )

message(NOTICE "Added executable Fields-RtoR-Thermal")


# ************************************************************************************************************


#add_executable (Fields-Montecarlo
#		Montecarlo1+1/MontecarloMain.cpp
#		Montecarlo1+1/Monitor.cpp
#		${SOURCES}  )
#target_link_libraries (Fields-Montecarlo ${SlabLibs} Slab-Maps-RtoR-Periodic_GPU )
#
#message(NOTICE "Added executable Fields-Montecarlo")


# ************************************************************************************************************


if( STUDIOSLAB_CUDA_SUPPORT )
	add_executable ( Fields-R2toR
			R2toR/MainR2toR.cpp
			R2toR/LeadingDelta/LeadingDelta.cpp
			R2toR/LeadingDelta/RingDeltaFunc.cpp
			#R2toR/LeadingDelta/DrivenEquation.cpp
			#R2toR/LeadingDelta/Allocator.cpp
			R2toR/LeadingDelta/Monitor.cpp
			R2toR/LeadingDelta/RingDeltaFunc.cu
			)
	target_link_libraries ( Fields-R2toR
			Slab-Core-Controller
			Slab-Core-Backend
			Slab-Core-App
			Slab-Graphics
			Slab-Math
			Slab-Maps-R2toR_GPU
			Slab-Models
			)

	set_property(TARGET Fields-R2toR PROPERTY CUDA_STANDARD_REQUIRED 20)
	#set_property(TARGET Fields-R2toR PROPERTY CUDA_ARCHITECTURES 61)

	message(NOTICE "Added executable Fields-R2toR")
else()
	message("Fields-R2toR NOT being generated: requires CUDA support.")
endif()