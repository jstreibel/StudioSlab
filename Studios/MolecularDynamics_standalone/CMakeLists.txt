
include_directories( ./ )

set (SOURCES
		main.cpp
		Simulation/Simulation.cpp

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

		Hamiltonians/Types.cpp

		IO/BackendSFML.cpp
		IO/UserInput.cpp
		IO/KeyPressListener.h
		IO/KeyPressListener.cpp
		IO/IOParams.h

		Simulation/SimParams.h
)

add_definitions(-D_GLIBCXX_USE_CXX11_ABI=1)

add_executable ( MolDynamic_standalone ${SOURCES} )
target_link_libraries ( MolDynamic_standalone
		sfml-graphics
		boost_random
)