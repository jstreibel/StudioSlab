include_directories( ./ )

set (SOURCES
		main.cpp
		App.cpp
		App.h
		Builder.cpp
		Builder.h
        Stepper.cpp
		Stepper.h
        Particle.cpp
        Particle.h

		Hamiltonians/SoftDisk/SoftDisk.cpp
		Hamiltonians/SoftDisk/Langevin.cpp
		Hamiltonians/SoftDisk/SoftDiskParams.h

		Hamiltonians/Lennard-Jones/LennardJones.cpp

		#Hamiltonians/GPU/nbody-gpu.cpp
		#Hamiltonians/GPU/nbody.cu

		Hamiltonians/Hash/MoleculeSpaceHash.cpp
		Hamiltonians/Hash/MoleculeSpaceHash.h
		Hamiltonians/Hash/FixedSizeMoleculeContainer.cpp
		Hamiltonians/Hash/FixedSizeMoleculeContainer.h
		Hamiltonians/Hash/HashParams.h

		Hamiltonians/NewtonMechanics.cpp
		Hamiltonians/NewtonMechanics.h
		Hamiltonians/NewtonMechanicsParams.h
)

add_executable ( MolDynamic ${SOURCES} )
target_link_libraries ( MolDynamic
		Slab-Utils
		Slab-Base
		Slab-Phys
		boost_random
)