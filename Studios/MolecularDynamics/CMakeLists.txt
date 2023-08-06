include_directories( ./ )

set (SOURCES
		main.cpp
		App.cpp
		App.h
)

add_definitions(-D_GLIBCXX_USE_CXX11_ABI=1)

add_executable ( MolDynamic ${SOURCES} )
target_link_libraries ( MolDynamic
		Slab-Base
		#sfml-graphics
		boost_random
)