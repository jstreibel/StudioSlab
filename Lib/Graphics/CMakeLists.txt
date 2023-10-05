
set( GRAPHICS_SOURCES
		Animation.cpp
		ZoomPanRotate.cpp

		Types.h        Types.cpp
		Utils.cpp

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

		OpenGL/OpenGLMonitor.cpp

		OpenGL/GLUTUtils.h  					OpenGL/GLUTUtils.cpp
		OpenGL/Utils.h							OpenGL/Utils.cpp
		OpenGL/Writer.h							OpenGL/Writer.cpp
		OpenGL/rougier/shader.h					OpenGL/rougier/shader.c
        OpenGL/OpenGL.h
        OpenGL/GLDebug.h						OpenGL/GLDebug.cpp
        OpenGL/Texture.h				        OpenGL/Texture.cpp
		OpenGL/Texture1D_Color.h 				OpenGL/Texture1D_Color.cpp
		OpenGL/Texture2D.h						OpenGL/Texture2D.cpp
		# OpenGL/Texture2D_UByte.h				OpenGL/Texture2D_UByte.cpp
		OpenGL/Texture2D_Color.h 				OpenGL/Texture2D_Color.cpp
		OpenGL/Texture2D_Real.h					OpenGL/Texture2D_Real.cpp
		OpenGL/VertexBuffer.h					OpenGL/VertexBuffer.cpp
		OpenGL/Shader.h							OpenGL/Shader.cpp
		OpenGL/PixelData.h						OpenGL/PixelData.cpp
		OpenGL/TextureUtils.h					OpenGL/TextureUtils.cpp
        OpenGL/Artists/Overlay.h        		OpenGL/Artists/Overlay.cpp
		OpenGL/Artists/ColorBarArtist.h			OpenGL/Artists/ColorBarArtist.cpp

		Graph/Artists/Artist.cpp
		Graph/Graph.cpp
		Graph/LabelingHelper.h				Graph/LabelingHelper.cpp
		Graph/Graph_Axis.cpp
		Graph/Styles.cpp
		Graph/Graph_Events.cpp
		Graph/Graph_GetSet.cpp
		Graph/Artists/XHairArtist.h			Graph/Artists/XHairArtist.cpp
		Graph/Artists/AxisArtist.h			Graph/Artists/AxisArtist.cpp

		Graph/StylesManager.h				Graph/StylesManager.cpp

		Graph/ℝ²↦ℝ/FlatFieldDisplay.h		Graph/ℝ²↦ℝ/FlatFieldDisplay.cpp
		Graph/ℝ²↦ℝ/FlatField2DArtist.h		Graph/ℝ²↦ℝ/FlatField2DArtist.cpp

		Graph/ℝ↦ℝ/HistoryDisplay.h			Graph/ℝ↦ℝ/HistoryDisplay.cpp
		Graph/ℝ↦ℝ/GraphRtoR.h				Graph/ℝ↦ℝ/GraphRtoR.cpp
		Graph/ℝ↦ℝ/RtoRMonitor.cpp
		Graph3D/Graph3D.cpp
		Graph3D/Graph3D.h
		Graph3D/Actor.cpp
		Graph3D/Actor.h
		Graph3D/Field2DActor.cpp
		Graph3D/Field2DActor.h
		OpenGL/ShaderLoader.cpp
		OpenGL/ShaderLoader.h
        MeshUtils.cpp
        MeshUtils.h
)

set ( LIB_LOCAL_NAME ${STUDIOSLAB_BASE_LIB_NAME}-Graphics )

add_library(${LIB_LOCAL_NAME} STATIC ${GRAPHICS_SOURCES})
target_link_libraries(${LIB_LOCAL_NAME}
		Slab-Utils
		freeimage
		freetype-gl
		)

message(NOTICE "Added library " ${LIB_LOCAL_NAME})