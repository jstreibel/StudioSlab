
set( GRAPHICS_SOURCES
		Animation.cpp
		ZoomPanRotate.cpp

		Styles/ColorMap.h 		Styles/ColorMap.cpp
		Styles/Colors.h       	Styles/Colors.cpp
		Styles/WindowStyles.h	Styles/WindowStyles.cpp

        Window/GUIWindow.cpp
		Window/Window.cpp
		Window/WindowContainer/WindowPanel.cpp
		Window/WindowTree/NodeWindow.cpp
		Window/WindowTree/WindowTreeBuilder.cpp

		Window/WindowContainer/WindowRow.cpp
		Window/WindowContainer/WindowRow.h
		Window/WindowContainer/WindowColumn.cpp
		Window/WindowContainer/WindowColumn.h

		SFML/Graph.cpp

		OpenGL/rougier/mat4.h					OpenGL/rougier/mat4.c
		OpenGL/rougier/ftgl-utils.h				OpenGL/rougier/ftgl-utils.c
		OpenGL/rougier/platform.h				OpenGL/rougier/platform.c
		OpenGL/rougier/vec234.h
		OpenGL/rougier/vector.h					OpenGL/rougier/vector.c
		OpenGL/rougier/vertex-attribute.c		OpenGL/rougier/vertex-attribute.c

		OpenGL/Utils.h							OpenGL/Utils.cpp
		OpenGL/GLUTUtils.h  					OpenGL/GLUTUtils.cpp
		OpenGL/Writer.h							OpenGL/Writer.cpp
		OpenGL/rougier/shader.h					OpenGL/rougier/shader.c
        OpenGL/OpenGL.h
        OpenGL/GLDebug.h						OpenGL/GLDebug.cpp
        # OpenGL/Texture.h				        OpenGL/Texture.cpp
		# OpenGL/Texture1D_Color.h 				OpenGL/Texture1D_Color.cpp
		# OpenGL/Texture2D.h						OpenGL/Texture2D.cpp
		# OpenGL/Texture2D_UByte.h				OpenGL/Texture2D_UByte.cpp
		# OpenGL/Texture2D_Color.h 				OpenGL/Texture2D_Color.cpp
		OpenGL/Texture2D_STIFF.h 				OpenGL/Texture2D_STIFF.cpp
		# OpenGL/Texture2D_Real.h					OpenGL/Texture2D_Real.cpp
		OpenGL/VertexBuffer.h					OpenGL/VertexBuffer.cpp
		OpenGL/Shader.h							OpenGL/Shader.cpp
		OpenGL/PixelData.h						OpenGL/PixelData.cpp
		OpenGL/Texture2D.cpp
		OpenGL/Texture2D.h
		OpenGL/Texture.cpp
		OpenGL/Texture.h

)


set ( LIB_LOCAL_NAME ${STUDIOS_LIB_BASE_NAME}-Graphics )

add_library(${LIB_LOCAL_NAME} STATIC ${GRAPHICS_SOURCES})
target_link_libraries(${LIB_LOCAL_NAME}
		Slab-Utils
		freeimage
		freetype-gl
		)

message(NOTICE "Added library " ${LIB_LOCAL_NAME})