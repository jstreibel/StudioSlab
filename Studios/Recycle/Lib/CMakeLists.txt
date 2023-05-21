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
		../../../Lib/Base/Graphics/SFML/imconfig-SFML.h
		../../../Lib/Base/Graphics/SFML/imgui-SFML.h
		../../../Lib/Base/Graphics/SFML/imgui-SFML_export.h
		colormap/itadpt/map_iterator_adapter.hpp
		colormap/color.hpp
		colormap/colormap.hpp
		colormap/grid.hpp
		colormap/map.hpp
		colormap/palettes.hpp
		colormap/pixmap.hpp
		../../../Lib/Base/Graphics/Colors.h
		../../../Lib/Base/Graphics/SFML/Graph.cpp
		../../../Lib/Base/Graphics/SFML/Graph.h
		../../../Lib/Phys/Thermal/Utils/RandUtils.cpp
		../../../Lib/Phys/Thermal/Utils/RandUtils.h
		../../../Lib/Phys/Thermal/Utils/ThermoUtils.cpp
		../../../Lib/Phys/Thermal/Utils/ThermoUtils.h
		../../../Lib/Base/Graphics/OpenGL/glad.c
		../../../Lib/Base/Graphics/OpenGL/glad.h
		../../Lib/Studios/Controller/Nuklear/nuklear.h
		../../../Lib/Base/Controller/Nuklear/_nuklear_sfml.hpp)
