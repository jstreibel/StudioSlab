

set ( RTOR_SOURCES
		RtoR/App/SimulationsAppRtoR.cpp

		RtoR/Core/RtoRModelAllocator.cpp RtoR/Core/RtoRModelAllocator.h

		RtoR/Controller/RtoRBCInterface.cpp RtoR/Controller/RtoRBCInterface.h

		RtoR/View/OutputStructureBuilderRtoR.cpp

		RtoR/Controller/SingleFormations/input-perturbed.cpp
		RtoR/Controller/InteractingFormations/input-sym-oscillons.cpp
		RtoR/Controller/SingleFormations/input-shockwave.cpp
		RtoR/Controller/InteractingFormations/input-general-oscillons.cpp RtoR/Controller/InteractingFormations/input-general-oscillons.h
		RtoR/Controller/ManyOscillons/InputManyOscillons.cpp RtoR/Controller/ManyOscillons/InputManyOscillons.h
		RtoR/Controller/ManyOscillons/InputMachineGun.cpp RtoR/Controller/ManyOscillons/InputMachineGun.h
		RtoR/Controller/ManyOscillons/InputStatistical.cpp RtoR/Controller/ManyOscillons/InputStatistical.h
		RtoR/Controller/ManyOscillons/RandUtils.cpp
		RtoR/Controller/SingleFormations/InputSingleOscillon.cpp RtoR/Controller/SingleFormations/InputSingleOscillon.h

		RtoR/Model/FunctionsCollection/analytic-breather.cpp
		RtoR/Model/FunctionsCollection/shockwave.cpp
		RtoR/Model/FunctionsCollection/AnalyticShockwave1D.cpp RtoR/Model/FunctionsCollection/AnalyticShockwave1D.h
		RtoR/Model/FunctionsCollection/AnalyticOscillon.cpp
		RtoR/Model/FunctionsCollection/AbsFunction.cpp
		RtoR/Model/FunctionsCollection/AbsPerturbedFunction.cpp
		RtoR/Model/FunctionsCollection/NoiseFunction.cpp RtoR/Model/FunctionsCollection/NoiseFunction.h
		RtoR/Model/FunctionsCollection/QuasiShockwave.cpp RtoR/Model/FunctionsCollection/QuasiShockwave.h

		RtoR/Model/RtoRFunctionArbitrary.cpp
		RtoR/Model/RtoRFunctionArbitraryCPU.cpp
		RtoR/Model/Derivatives/DerivativesCPU.cpp
		RtoR/Model/RtoRBoundaryCondition.cpp
		RtoR/Model/RtoRFuncArbResizable.cpp RtoR/Model/RtoRFuncArbResizable.h

		RtoR/View/EnergyCalculator.cpp RtoR/View/EnergyCalculator.h
		RtoR/View/OutputSnapshots.cpp
		RtoR/View/Util.cpp
		RtoR/View/Graphics/RtoROutputOpenGL.cpp
		RtoR/View/Graphics/RtoROutputOpenGlShockwave.cpp RtoR/View/Graphics/RtoROutputOpenGlShockwave.h
		RtoR/View/RtoROutputHistoryToFile.cpp RtoR/View/RtoROutputHistoryToFile.h

		)


set ( R2TOR_SOURCES

		R2toR/Controller/R2ToRInputRadialShockwave.cpp ../Apps/Maps/R2toR/Controller/R2ToRInputRadialShockwave.h
		R2toR/Controller/R2ToRInputShockwave.cpp
		R2toR/Controller/R2ToRInputShockwaveAtT0.cpp ../Apps/Maps/R2toR/Controller/R2ToRInputShockwaveAtT0.h
		R2toR/Model/FunctionsCollection/AnalyticShockwave2DRadialSymmetry.cpp ../Apps/Maps/R2toR/Model/FunctionsCollection/AnalyticShockwave2DRadialSymmetry.h
		R2toR/Model/Transform.cpp ../Apps/Maps/R2toR/Model/Transform.h
		R2toR/Model/Util/Interpolator.cpp ../Apps/Maps/R2toR/Model/Util/Interpolator.h
		R2toR/Model/RtoRSystemGordonRadialSymmetry.h
		R2toR/Model/FunctionsCollection/R2ToRRegularDelta.cpp ../Apps/Maps/R2toR/Model/FunctionsCollection/R2ToRRegularDelta.h
		R2toR/Model/R2toRFunctionArbitrary.cpp
		R2toR/Model/R2toRFunctionArbitraryCPU.cpp
		R2toR/Model/R2ToRBoundaryCondition.cpp
		R2toR/View/Filters/DimensionReductionFilter.cpp
		R2toR/View/R2toROutputOpenGLShockwave.cpp
		R2toR/View/R2toROutputOpenGLGeneric.cpp
		R2toR/View/R2ToROutputOpenGlShockwaveAtT0.cpp ../Apps/Maps/R2toR/View/R2ToROutputOpenGlShockwaveAtT0.h
		R2toR/View/OutputShockwaveZeros.cpp ../Apps/Maps/R2toR/View/OutputShockwaveZeros.h
		R2toR/View/OutputStructureBuilderR2ToR.cpp ../Apps/Maps/R2toR/View/OutputStructureBuilderR2ToR.h
		R2toR/SimulationsAppR2ToR.h
		R2toR/SimulationsAppR2ToR.cpp

		RtoR2/StraightLine.cpp

		R2toR2/R2toR2Function.h
		R2toR/Model/Builder/R2ToRModelBuilder.cpp R2toR/Model/Builder/R2ToRModelBuilder.h
		)
