
project( molekul-play )
cmake_minimum_required( VERSION 3.8 )


set(CUDA_DIR /opt/cuda)
enable_language(CUDA)


find_package(OpenMP)
find_package(SFML 2 COMPONENTS graphics window system)
find_package(Boost COMPONENTS program_options)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fopenmp -O3")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -fopenmp -O3")

include_directories (
        ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
        ${CUDA_DIR}/include
        ${PWD}
        )

set (SOURCES
		main.cpp
		Simulation/Simulation.cpp

		Hamiltonians/SoftDisk/SoftDisk.cpp
		Hamiltonians/SoftDisk/Langevin.cpp

		Hamiltonians/Lennard-Jones/LennardJones.cpp

		Hamiltonians/GPU/nbody-gpu.cpp
		Hamiltonians/GPU/nbody.cu

		Hamiltonians/Hash/MoleculeSpaceHash.cpp
		Hamiltonians/Hash/MoleculeSpaceHash.h

		Hamiltonians/Types.cpp

		IO/BackendSFML.cpp
		IO/UserInput.cpp
		IO/KeyPressListener.h
		IO/KeyPressListener.cpp

		Hamiltonians/SoftDisk/SoftDiskParams.h IO/IOParams.h Simulation/SimParams.h Hamiltonians/Hash/HashParams.h Hamiltonians/NewtonMechanics.cpp Hamiltonians/NewtonMechanics.h Hamiltonians/NewtonMechanicsParams.h Hamiltonians/Hash/FixedSizeMoleculeContainer.cpp Hamiltonians/Hash/FixedSizeMoleculeContainer.h)

add_executable ( dynamic ${SOURCES} )
target_link_libraries ( dynamic GL glut boost_program_options boost_random sfml-system sfml-window sfml-graphics)

set_target_properties(dynamic PROPERTIES CUDA_ARCHITECTURES "72")
