include_directories( ./ )

set (SOURCES
        main.cpp
        XYApp.h     XYApp.cpp

)

add_executable ( XY ${SOURCES} )
target_link_libraries ( XY
        Slab

        sfml-graphics
        sfml-window
        sfml-system

        GLEW
        #OpenGL
        GL

        boost_random
)