
set ( SlabLibs
		Slab-Utils

		Slab-Base-App
		Slab-Base-Backend
		Slab-Base-Controller
		Slab-Base-Graphics
		Slab-Base-Tools

		Slab-Phys

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
# 		Slab-Phys
# 		Slab-Studios-Backend
# 		Slab-Studios-Controller
# 		Slab-Studios-App )


# ************************************************************************************************************