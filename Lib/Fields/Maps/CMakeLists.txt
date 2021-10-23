include_directories(../../)

set ( RTOR_SOURCES
		../../Studios/MathTools/Maps/RtoR/App/SimulationsAppRtoR.cpp

		../../Studios/MathTools/Maps/RtoR/Core/RtoRModelAllocator.cpp

		../../Studios/MathTools/Maps/RtoR/Controller/RtoRBCInterface.cpp
		../../Studios/MathTools/Maps/RtoR/Controller/SingleFormations/input-perturbed.cpp
		../../Studios/MathTools/Maps/RtoR/Controller/InteractingFormations/input-sym-oscillons.cpp
		../../Studios/MathTools/Maps/RtoR/Controller/SingleFormations/input-shockwave.cpp
		../../Studios/MathTools/Maps/RtoR/Controller/InteractingFormations/input-general-oscillons.cpp
		../../Studios/MathTools/Maps/RtoR/Controller/ManyOscillons/InputManyOscillons.cpp
		../../Studios/MathTools/Maps/RtoR/Controller/ManyOscillons/InputMachineGun.cpp
		../../Studios/MathTools/Maps/RtoR/Controller/ManyOscillons/InputStatistical.cpp
		../../Studios/MathTools/Maps/RtoR/Controller/ManyOscillons/RandUtils.cpp
		../../Studios/MathTools/Maps/RtoR/Controller/SingleFormations/InputSingleOscillon.cpp

		../../Studios/MathTools/Maps/RtoR/Model/FunctionsCollection/analytic-breather.cpp
		../../Studios/MathTools/Maps/RtoR/Model/FunctionsCollection/shockwave.cpp
		../../Studios/MathTools/Maps/RtoR/Model/FunctionsCollection/AnalyticShockwave1D.cpp
		../../Studios/MathTools/Maps/RtoR/Model/FunctionsCollection/AnalyticOscillon.cpp
		../../Studios/MathTools/Maps/RtoR/Model/FunctionsCollection/AbsFunction.cpp
		../../Studios/MathTools/Maps/RtoR/Model/FunctionsCollection/AbsPerturbedFunction.cpp
		../../Studios/MathTools/Maps/RtoR/Model/FunctionsCollection/NoiseFunction.cpp
		../../Studios/MathTools/Maps/RtoR/Model/FunctionsCollection/QuasiShockwave.cpp
		../../Studios/MathTools/Maps/RtoR/Model/RtoRFunctionArbitrary.cpp
		../../Studios/MathTools/Maps/RtoR/Model/RtoRFunctionArbitraryCPU.cpp
		../../Studios/MathTools/Maps/RtoR/Model/Derivatives/DerivativesCPU.cpp
		../../Studios/MathTools/Maps/RtoR/Model/RtoRBoundaryCondition.cpp
		../../Studios/MathTools/Maps/RtoR/Model/RtoRFuncArbResizable.cpp

		../../Studios/MathTools/Maps/RtoR/View/OutputStructureBuilderRtoR.cpp
		../../Studios/MathTools/Maps/RtoR/View/RtoROutputHistoryToFile.cpp
		../../Studios/MathTools/Maps/RtoR/View/Graphics/RtoROutputOpenGlShockwave.cpp
		../../Studios/MathTools/Maps/RtoR/View/Graphics/RtoROutputOpenGL.cpp
		../../Studios/MathTools/Maps/RtoR/View/EnergyCalculator.cpp
		../../Studios/MathTools/Maps/RtoR/View/OutputSnapshots.cpp
		../../Studios/MathTools/Maps/RtoR/View/Util.cpp

		)

set ( R2TOR_SOURCES
		Maps/R2toR/Model/R2toRFunctionArbitrary.cpp
		Maps/R2toR/Model/R2toRFunctionArbitraryCPU.cpp

		R2toR/Controller/R2ToRInputRadialShockwave.cpp
		R2toR/Controller/R2ToRInputShockwave.cpp
		R2toR/Controller/R2ToRInputShockwaveAtT0.cpp
		R2toR/Model/FunctionsCollection/AnalyticShockwave2DRadialSymmetry.cpp
		R2toR/Model/FunctionsCollection/R2ToRRegularDelta.cpp
		R2toR/Model/Transform.cpp
		R2toR/Model/Util/Interpolator.cpp
		R2toR/Model/R2toRFunctionArbitrary.cpp
		R2toR/Model/R2toRFunctionArbitraryCPU.cpp
		R2toR/Model/R2ToRBoundaryCondition.cpp
		R2toR/View/Filters/DimensionReductionFilter.cpp
		R2toR/View/R2toROutputOpenGLShockwave.cpp
		R2toR/View/R2toROutputOpenGLGeneric.cpp
		R2toR/View/R2ToROutputOpenGlShockwaveAtT0.cpp
		R2toR/View/OutputShockwaveZeros.cpp
		R2toR/View/OutputStructureBuilderR2ToR.cpp
		R2toR/SimulationsAppR2ToR.cpp

		RtoR2/StraightLine.cpp

		R2toR/Model/Builder/R2ToRModelBuilder.cpp
		)

add_library(Fields-Map-RtoR STATIC ${RTOR_SOURCES})
set_property(TARGET Fields-Map-RtoR PROPERTY CUDA_ARCHITECTURES 61)