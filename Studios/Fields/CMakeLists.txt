
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

add_executable( Fields-SPI
		QuantumSG/qsg_main.cpp)
target_link_libraries( Fields-SPI Slab )
message(NOTICE "Added executable Fields-SPI")

# ************************************************************************************************************

add_executable ( Fields-RtoR
		RtoR-PureSG/MainRtoR.cpp

		RtoR-PureSG/SingleFormations/input-perturbed.cpp
		RtoR-PureSG/SingleFormations/input-shockwave.cpp
		RtoR-PureSG/SingleFormations/InputSingleOscillon.cpp

		RtoR-PureSG/InteractingFormations/input-sym-oscillons.cpp
		RtoR-PureSG/InteractingFormations/input-general-oscillons.cpp
		RtoR-PureSG/Builder.cpp
		RtoR-PureSG/Builder.h
)
target_link_libraries ( Fields-RtoR Slab )
#set_property(TARGET Fields-RtoR PROPERTY CUDA_ARCHITECTURES 72)
message(NOTICE "Added executable Fields-RtoR")


# ************************************************************************************************************


add_executable (osc-viewer
		Viewers/main.cpp
		Viewers/HistoryFileLoader.cpp Viewers/HistoryFileLoader.h
)
target_link_libraries (osc-viewer Slab )

message(NOTICE "Added executable osc-viewer")


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
target_link_libraries (Fields-RtoR-Thermal Slab )

message(NOTICE "Added executable Fields-RtoR-Thermal")


# ************************************************************************************************************


add_executable ( Fields-RtoR-Montecarlo-Action Montecarlo1+1/Main-RtoRActionMontecarlo.cpp)
target_link_libraries (Fields-RtoR-Montecarlo-Action Slab )
message(NOTICE "Added executable Fields-RtoR-Montecarlo-Action")

add_executable ( Fields-RtoR-Montecarlo-Hamiltonian Montecarlo1+1/Main-RtoRHamiltonianMetropolis.cpp)
target_link_libraries (Fields-RtoR-Montecarlo-Hamiltonian Slab )
message(NOTICE "Added executable Fields-RtoR-Montecarlo-Hamiltonian")


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
	target_link_libraries ( Fields-R2toR Slab )

	set_property(TARGET Fields-R2toR PROPERTY CUDA_STANDARD_REQUIRED 20)
	#set_property(TARGET Fields-R2toR PROPERTY CUDA_ARCHITECTURES 61)

	message(NOTICE "Added executable Fields-R2toR")
else()
	message("Fields-R2toR NOT being generated: requires CUDA support.")
endif()