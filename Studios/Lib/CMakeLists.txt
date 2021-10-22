cmake_minimum_required(VERSION 3.19)
project(StudiosLib)

set(CMAKE_CXX_STANDARD 14)

include_directories(StudiosLib)
include_directories(cegui-sfml)
include_directories(colormap)
include_directories(colormap/itadpt)
include_directories(GraphicsUtils)
include_directories(sfml-utils)
include_directories(ThermoUtils)

add_executable(StudiosLib
		cegui-sfml/imconfig-SFML.h
		cegui-sfml/imgui-SFML.h
		cegui-sfml/imgui-SFML_export.h
		colormap/itadpt/map_iterator_adapter.hpp
		colormap/color.hpp
		colormap/colormap.hpp
		colormap/grid.hpp
		colormap/map.hpp
		colormap/palettes.hpp
		colormap/pixmap.hpp
		GraphicsUtils/Colors.h
		sfml-utils/Graph.cpp
		sfml-utils/Graph.h
		ThermoUtils/RandUtils.cpp
		ThermoUtils/RandUtils.h
		ThermoUtils/ThermoUtils.cpp
		ThermoUtils/ThermoUtils.h
		glad.c
		glad.h
		Nuklear/nuklear.h
		Nuklear/nuklear_sfml.hpp)
