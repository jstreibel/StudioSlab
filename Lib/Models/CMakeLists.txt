

set ( KG_SOURCES
		KleinGordon/KG-Recipe.cpp
		KleinGordon/KG-Recipe.h
		KleinGordon/KG-Solver.h
		KleinGordon/KG-State.h
        KleinGordon/__archive__/KG-Langevin.h
        KleinGordon/RtoR-Montecarlo/Montecarlo-Lorentz-2ndOrder.h

        KleinGordon/RtoR/LinearStepping/KG-RtoR-Recipe.cpp
        KleinGordon/RtoR/LinearStepping/KG-RtoREnergyCalculator.cpp
        KleinGordon/RtoR/LinearStepping/KG-RtoREquationState.cpp
        KleinGordon/RtoR/LinearStepping/KG-RtoRBoundaryCondition.cpp
        KleinGordon/RtoR/LinearStepping/Output/SimHistory.h KleinGordon/RtoR/LinearStepping/Output/SimHistory.cpp
        KleinGordon/RtoR/LinearStepping/Output/SimHistory_Fourier.h KleinGordon/RtoR/LinearStepping/Output/SimHistory_Fourier.cpp
        KleinGordon/RtoR/LinearStepping/Output/SnapshotOutput.h KleinGordon/RtoR/LinearStepping/Output/SnapshotOutput.cpp
        KleinGordon/RtoR/LinearStepping/Output/SnapshotOutput.h KleinGordon/RtoR/LinearStepping/Output/SnapshotOutput.cpp

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

		KleinGordon/KG-LangevinSolver.h KleinGordon/KG-LangevinSolver.cpp

        KleinGordon/RtoR/LinearStepping/Output/DFTSnapshotOutput.cpp
        KleinGordon/RtoR/LinearStepping/Output/DFTSnapshotOutput.h

		KleinGordon/RtoR/Graphics/Panels/RtoRScenePanel.cpp
		KleinGordon/RtoR/Graphics/Panels/RtoRScenePanel.h
		KleinGordon/RtoR/Graphics/Panels/RtoRHistoryPanel.cpp
		KleinGordon/RtoR/Graphics/Panels/RtoRHistoryPanel.h

        KleinGordon/RtoR/LinearStepping/Output/CenterTimeDFTOutput.cpp
        KleinGordon/RtoR/LinearStepping/Output/CenterTimeDFTOutput.h

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
		KleinGordon/KG-NumericConfig.cpp
		KleinGordon/KG-NumericConfig.h
		Stochastic-Path-Integral/SPINumericConfig.cpp
		Stochastic-Path-Integral/SPINumericConfig.h
)

if(STUDIOSLAB_VTK_SUPPORT)
	set(MODELS_VTK_SOURCES KleinGordon/R2toR/Graphics/LastOutputVtkVisualizer.cpp)
endif ()

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

        MolecularDynamics/MolecularDynamicsRecipe.h MolecularDynamics/MolecularDynamicsRecipe.cpp
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

		${MODELS_VTK_SOURCES}

		DynamicsNumericConfig.cpp
		DynamicsNumericConfig.h
		KleinGordon/R2toR/Output/KG2DSnapshotOutput.cpp
		KleinGordon/R2toR/Output/KG2DSnapshotOutput.h
		Stochastic-Path-Integral/SPI-Recipe.cpp
		Stochastic-Path-Integral/SPI-Recipe.h
		Stochastic-Path-Integral/SPI-State.cpp
		Stochastic-Path-Integral/SPI-State.h
		Stochastic-Path-Integral/SPI-Solver.cpp
		Stochastic-Path-Integral/SPI-Solver.h
		Stochastic-Path-Integral/SPI-BC.cpp
		Stochastic-Path-Integral/SPI-BC.h
)

target_link_libraries(${LIB_LOCAL_NAME}
		Slab-Core
		Slab-Math
		Slab-Graphics

		${VTK_LIBRARIES}
)
message(NOTICE "Added library " ${LIB_LOCAL_NAME})