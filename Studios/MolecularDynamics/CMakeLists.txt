include_directories( ./ )

set (SOURCES
		main.cpp
		App.h			App.cpp
        Recipe.h Recipe.cpp
        VerletStepper.h VerletStepper.cpp
        Particle.h 		Particle.cpp
		Monitor.h		Monitor.cpp

		Hamiltonians/NewtonMechanics.h					Hamiltonians/NewtonMechanics.cpp

		Hamiltonians/SoftDisk/SoftDisk.h				Hamiltonians/SoftDisk/SoftDisk.cpp
		Hamiltonians/Langevin.h 						Hamiltonians/Langevin.cpp
		Hamiltonians/SoftDisk/SoftDiskParams.h

		Hamiltonians/Lennard-Jones/LennardJones.h		Hamiltonians/Lennard-Jones/LennardJones.cpp

		Hamiltonians/Hash/MoleculeSpaceHash.h			Hamiltonians/Hash/MoleculeSpaceHash.cpp
		Hamiltonians/Hash/FixedSizeMoleculeContainer.h	Hamiltonians/Hash/FixedSizeMoleculeContainer.cpp
		Hamiltonians/Hash/HashParams.h

		#Hamiltonians/GPU/nbody-gpu.cpp
		#Hamiltonians/GPU/nbody.cu
)

add_executable ( MolDynamic ${SOURCES} )
target_link_libraries ( MolDynamic
		Slab
		boost_random
)