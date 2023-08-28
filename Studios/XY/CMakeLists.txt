include_directories( ./ )

set (SOURCES
        main.cpp
        XYApp.h     XYApp.cpp

)

add_executable ( XY ${SOURCES} )
target_link_libraries ( XY
        #Slab-Utils
        Slab-Core
        Slab-Math

        sfml-graphics
        sfml-window
        sfml-system

        GLEW
        #OpenGL
        GL

        boost_random
)