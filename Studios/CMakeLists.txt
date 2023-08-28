
set ( SlabLibs
		Slab-Utils

		Slab-Core-App
		Slab-Core-Backend
		Slab-Core-Controller
		Slab-Core-Graphics
		Slab-Core-Tools

		Slab-Math

		Slab-Models
		)


include_directories(../Lib)

add_subdirectory(MolecularDynamics)
add_subdirectory(XY)
add_subdirectory(Fields)
add_subdirectory(Tests)
add_subdirectory(Prototype)





# ************************************************************************************************************


# add_executable ( Thermo-XY XY/main.cpp XY/XYApp.cpp)
# target_link_libraries( Thermo-XY
# 		Slab-Math
# 		Slab-Studios-Backend
# 		Slab-Studios-Controller
# 		Slab-Studios-App )


# ************************************************************************************************************