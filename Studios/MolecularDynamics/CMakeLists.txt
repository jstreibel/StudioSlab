include_directories( ./ )

set (SOURCES
		main.cpp
		App.h			App.cpp

		#Hamiltonians/GPU/nbody-gpu.cpp
		#Hamiltonians/GPU/nbody.cu
)

add_executable ( MolDynamic ${SOURCES} )
target_link_libraries ( MolDynamic
		Slab
)