set ( MODELS_SOURCES
		KleinGordon/KGSolver.h
		KleinGordon/KGState.h
		KleinGordon/KG-Langevin.h
		KleinGordon/Montecarlo-Lorentz-2ndOrder.h

		KleinGordon/RtoR/KG-RtoRBuilder.cpp
		KleinGordon/RtoR/KG-RtoREnergyCalculator.cpp

		KleinGordon/RtoR/KG-RtoREquationState.cpp
		KleinGordon/RtoR/KG-RtoRBoundaryCondition.cpp

		KleinGordon/RtoR/RtoRMonitor.cpp

		KleinGordon/RtoR/KG-RtoRSystemGordonGPU.cu
		KleinGordon/KGBuilder.h)



set ( LIB_LOCAL_NAME ${STUDIOSLAB_BASE_LIB_NAME}-Models )

add_library( ${LIB_LOCAL_NAME} STATIC ${MODELS_SOURCES} )


target_link_libraries(${LIB_LOCAL_NAME}
		Slab-Utils
		Slab-Base-Backend
		Slab-Base-Controller
		Slab-Phys
		)