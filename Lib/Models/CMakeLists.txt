

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

		KleinGordon/RtoR/Graphics/Viewers/HistogramsViewer_KG.cpp
		KleinGordon/RtoR/Graphics/RtoRMonitor.cpp
		KleinGordon/RtoR/Graphics/Panels/RtoRStatisticalMonitor.cpp
		KleinGordon/RtoR/Graphics/Panels/RtoRStatisticalMonitor.h
		KleinGordon/RtoR/Graphics/Panels/RtoRRealtimePanel.cpp
		KleinGordon/RtoR/Graphics/Panels/RtoRRealtimePanel.h
		KleinGordon/RtoR/Graphics/RtoRPanel.cpp
		KleinGordon/RtoR/Graphics/RtoRPanel.h
		KleinGordon/RtoR/Graphics/Panels/RtoRFourierPanel.cpp
		KleinGordon/RtoR/Graphics/Panels/RtoRFourierPanel.h
		KleinGordon/RtoR/Graphics/Panels/CorrelationsPanel.cpp
		KleinGordon/RtoR/Graphics/Panels/CorrelationsPanel.h

		KleinGordon/R2toR/Graphics/R2toROutputOpenGLGeneric.cpp
		KleinGordon/R2toR/R2toRBuilder.cpp
		KleinGordon/R2toR/R2toRBuilder.h
		KleinGordon/R2toR/BoundaryConditions/R2ToRBoundaryCondition.cpp
        KleinGordon/R2toR/EquationState.cpp

		KleinGordon/LangevinKGSolver.h KleinGordon/LangevinKGSolver.cpp

		KleinGordon/RtoR/Output/DFTSnapshotOutput.cpp
		KleinGordon/RtoR/Output/DFTSnapshotOutput.h

		KleinGordon/RtoR/Graphics/Panels/RtoRScenePanel.cpp
		KleinGordon/RtoR/Graphics/Panels/RtoRScenePanel.h
		KleinGordon/RtoR/Graphics/Panels/RtoRHistoryPanel.cpp
		KleinGordon/RtoR/Graphics/Panels/RtoRHistoryPanel.h

        KleinGordon/RtoR/Output/CenterTimeDFTOutput.cpp
        KleinGordon/RtoR/Output/CenterTimeDFTOutput.h

		KleinGordon/RtoR/Graphics/Viewers/EnergyViewer_KG.cpp
		KleinGordon/RtoR/Graphics/Viewers/EnergyViewer_KG.h
		KleinGordon/RtoR/Graphics/Viewers/KGViewer.cpp
		KleinGordon/RtoR/Graphics/Viewers/KGViewer.h
        KleinGordon/RtoR/Graphics/Viewers/KGMainViewer.cpp
        KleinGordon/RtoR/Graphics/Viewers/KGMainViewer.h
		KleinGordon/RtoR/Graphics/Viewers/HistoryViewer_KG.cpp	KleinGordon/RtoR/Graphics/Viewers/HistoryViewer_KG.h
        KleinGordon/RtoR/Graphics/Viewers/TwoPointCorrelationViewer_KG.cpp
        KleinGordon/RtoR/Graphics/Viewers/TwoPointCorrelationViewer_KG.h
		KleinGordon/RtoR/Analytic/TwoPointCorrelation.cpp
		KleinGordon/RtoR/Analytic/TwoPointCorrelation.h
        KleinGordon/RtoR/Graphics/Viewers/TimeFTViewer.cpp
        KleinGordon/RtoR/Graphics/Viewers/TimeFTViewer.h
        KleinGordon/KGNumericConfig.cpp
        KleinGordon/KGNumericConfig.h
)

set ( XY_SOURCES

	XY/SingleSim/SingleSimViewController.h
		XY/SingleSim/SingleSimViewController.cpp

	XY/ManyHistories/QuenchViewController.cpp
	XY/ManyHistories/HistoryViewController.cpp

)

set ( MOLDYN_SOURCES
		MolecularDynamics/MolDynNumericConfig.cpp

		MolecularDynamics/Hamiltonians/NewtonMechanics.h 					MolecularDynamics/Hamiltonians/NewtonMechanics.cpp

		MolecularDynamics/Hamiltonians/SoftDisk/SoftDisk.h 					MolecularDynamics/Hamiltonians/SoftDisk/SoftDisk.cpp
		MolecularDynamics/Hamiltonians/Langevin.h							MolecularDynamics/Hamiltonians/Langevin.cpp
		MolecularDynamics/Hamiltonians/SoftDisk/SoftDiskParams.h

		MolecularDynamics/Hamiltonians/Lennard-Jones/LennardJones.h			MolecularDynamics/Hamiltonians/Lennard-Jones/LennardJones.cpp

		MolecularDynamics/Hamiltonians/Hash/MoleculeSpaceHash.h				MolecularDynamics/Hamiltonians/Hash/MoleculeSpaceHash.cpp
		MolecularDynamics/Hamiltonians/Hash/FixedSizeMoleculeContainer.h 	MolecularDynamics/Hamiltonians/Hash/FixedSizeMoleculeContainer.cpp
		MolecularDynamics/Hamiltonians/Hash/HashParams.h

		MolecularDynamics/Recipe.h 				  MolecularDynamics/Recipe.cpp
		MolecularDynamics/VerletStepper.h 		  MolecularDynamics/VerletStepper.cpp
		MolecularDynamics/Hamiltonians/Particle.h MolecularDynamics/Hamiltonians/Particle.cpp
		MolecularDynamics/Monitor.h 			  MolecularDynamics/Monitor.cpp
		MolecularDynamics/Hamiltonians/MoleculesState.cpp
		MolecularDynamics/Hamiltonians/MoleculesState.h
)

if( STUDIOSLAB_CUDA_SUPPORT )
	# set ( KG_SOURCES_CUDA KleinGordon/RtoR/KG-RtoRSystemGordonGPU.cu)
endif ()


set ( LIB_LOCAL_NAME ${STUDIOSLAB_BASE_LIB_NAME}-Models )

add_library( ${LIB_LOCAL_NAME} STATIC
		${KG_SOURCES}
		${KG_SOURCES_CUDA}
		${XY_SOURCES}
		${MOLDYN_SOURCES}
		DynamicsNumericConfig.cpp
		DynamicsNumericConfig.h
)

target_link_libraries(${LIB_LOCAL_NAME} Slab-Core Slab-Math Slab-Graphics )
message(NOTICE "Added library " ${LIB_LOCAL_NAME})