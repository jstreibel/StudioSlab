

set ( KG_SOURCES
        KleinGordon/KGBuilder.cpp
		KleinGordon/KGBuilder.h
		KleinGordon/KGSolver.h
		KleinGordon/KGState.h
		KleinGordon/KG-Langevin.h
		KleinGordon/Montecarlo-Lorentz-2ndOrder.h

		KleinGordon/RtoR/KG-RtoRBuilder.cpp
		KleinGordon/RtoR/KG-RtoREnergyCalculator.cpp
		KleinGordon/RtoR/KG-RtoREquationState.cpp
		KleinGordon/RtoR/KG-RtoRBoundaryCondition.cpp
		KleinGordon/RtoR/Output/SimHistory.h			KleinGordon/RtoR/Output/SimHistory.cpp
		KleinGordon/RtoR/Output/SimHistory_Fourier.h	KleinGordon/RtoR/Output/SimHistory_Fourier.cpp
		KleinGordon/RtoR/Output/SnapshotOutput.h		KleinGordon/RtoR/Output/SnapshotOutput.cpp
		KleinGordon/RtoR/Output/SnapshotOutput.h		KleinGordon/RtoR/Output/SnapshotOutput.cpp
		KleinGordon/R2toR/Graphics/R2toROutputOpenGLGeneric.cpp
		KleinGordon/RtoR/Graphics/RtoRMonitor.cpp
		KleinGordon/RtoR/Graphics/RtoRStatisticalMonitor.cpp
		KleinGordon/RtoR/Graphics/RtoRStatisticalMonitor.h
		KleinGordon/RtoR/Graphics/RtoRRealtimePanel.cpp
		KleinGordon/RtoR/Graphics/RtoRRealtimePanel.h
		KleinGordon/RtoR/Graphics/RtoRPanel.cpp
		KleinGordon/RtoR/Graphics/RtoRPanel.h
		KleinGordon/RtoR/Graphics/RtoRFourierPanel.cpp
		KleinGordon/RtoR/Graphics/RtoRFourierPanel.h
		KleinGordon/RtoR/Graphics/CorrelationsPanel.cpp
		KleinGordon/RtoR/Graphics/CorrelationsPanel.h

		KleinGordon/R2toR/R2toRBuilder.cpp
		KleinGordon/R2toR/R2toRBuilder.h
		KleinGordon/R2toR/BoundaryConditions/R2ToRBoundaryCondition.cpp
		KleinGordon/R2toR/EquationSolver.h				KleinGordon/R2toR/EquationState.cpp

        LangevinKleinGordon/LangevinKGSolver.h	        LangevinKleinGordon/LangevinKGSolver.cpp
		KleinGordon/RtoR/Output/DFTSnapshotOutput.cpp
		KleinGordon/RtoR/Output/DFTSnapshotOutput.h
)

set ( XY_SOURCES

	XY/SingleSim/SingleSimViewController.h XY/SingleSim/SingleSimViewController.cpp

	XY/ManyHistories/QuenchViewController.cpp
	XY/ManyHistories/HistoryViewController.cpp

)

if( STUDIOSLAB_CUDA_SUPPORT )
	set ( KG_SOURCES_CUDA KleinGordon/RtoR/KG-RtoRSystemGordonGPU.cu)
endif ()


set ( LIB_LOCAL_NAME ${STUDIOSLAB_BASE_LIB_NAME}-Models )

add_library( ${LIB_LOCAL_NAME} STATIC ${KG_SOURCES} ${KG_SOURCES_CUDA} ${XY_SOURCES} )
target_link_libraries(${LIB_LOCAL_NAME} Slab-Core Slab-Math Slab-Graphics )
message(NOTICE "Added library " ${LIB_LOCAL_NAME})