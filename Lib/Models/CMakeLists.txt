set ( MODELS_SOURCES
		KleinGordon/KGBuilder.h
		KleinGordon/KGSolver.h
		KleinGordon/KGState.h
		KleinGordon/KG-Langevin.h
		KleinGordon/Montecarlo-Lorentz-2ndOrder.h

		KleinGordon/RtoR/KG-RtoRBuilder.cpp
		KleinGordon/RtoR/KG-RtoREnergyCalculator.cpp
		KleinGordon/RtoR/KG-RtoREquationState.cpp
		KleinGordon/RtoR/KG-RtoRBoundaryCondition.cpp
		KleinGordon/RtoR/RtoRMonitor.cpp
		KleinGordon/RtoR/Output/SimHistory.h			KleinGordon/RtoR/Output/SimHistory.cpp
		KleinGordon/RtoR/Output/HistoryDisplay.h		KleinGordon/RtoR/Output/HistoryDisplay.cpp


		KleinGordon/R2toR/R2toRBuilder.cpp
		KleinGordon/R2toR/R2toRBuilder.h

		KleinGordon/R2toR/BoundaryConditions/R2ToRBoundaryCondition.cpp
		KleinGordon/R2toR/EquationSolver.h				KleinGordon/R2toR/EquationState.cpp
        LangevinKleinGordon/LangevinKGSolver.h	        LangevinKleinGordon/LangevinKGSolver.cpp
		KleinGordon/RtoR/Output/SimHistory_Fourier.h	KleinGordon/RtoR/Output/SimHistory_Fourier.cpp
)

if( STUDIOSLAB_CUDA_SUPPORT )
	set ( MODELS_SOURCES_CUDA KleinGordon/RtoR/KG-RtoRSystemGordonGPU.cu )
endif ()


set ( LIB_LOCAL_NAME ${STUDIOSLAB_BASE_LIB_NAME}-Models )

add_library( ${LIB_LOCAL_NAME} STATIC ${MODELS_SOURCES} ${MODELS_SOURCES_CUDA} )
target_link_libraries(${LIB_LOCAL_NAME}
		Slab-Utils
		Slab-Core-Backend
		Slab-Core-Controller
		Slab-Maps-RtoR
		Slab-Maps-RtoR_GPU
		Slab-Maps-RtoR-Periodic_GPU
		Slab-Maps-R2toR_GPU

		Slab-Math )
message(NOTICE "Added library " ${LIB_LOCAL_NAME})