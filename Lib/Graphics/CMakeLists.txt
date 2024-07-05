

set ( LIB_GRAPHICS ${STUDIOSLAB_BASE_LIB_NAME}-Graphics )

add_library(${LIB_GRAPHICS} STATIC
        Types2D.h Types2D.cpp
		Utils.cpp

		OpenGL/LegacyGL/FunctionRenderer.cpp OpenGL/LegacyGL/FunctionRenderer.h

		Styles/ColorMap.h 		Styles/ColorMap.cpp
		Styles/Colors.h       	Styles/Colors.cpp
		Window/WindowStyles.h Window/WindowStyles.cpp

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

		BaseMonitor.cpp

		OpenGL/GLUTUtils.h OpenGL/GLUTUtils.cpp
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
		OpenGL/LegacyGL/PointSetRenderer.h    	OpenGL/LegacyGL/PointSetRenderer.cpp

		Graph/Artists/Artist.cpp
		Graph/PlottingWindow.cpp
		Graph/Artists/LabelsArtist.h Graph/Artists/LabelsArtist.cpp
		Graph/PlottingWindow_Axis.cpp
		Graph/PlotStyle.cpp
		Graph/PlottingWindow_Events.cpp
		Graph/PlottingWindow_GetSet.cpp
		Graph/Artists/XHairArtist.h			Graph/Artists/XHairArtist.cpp
		Graph/Artists/AxisArtist.h			Graph/Artists/AxisArtist.cpp

		Graph/PlotThemeManager.h Graph/PlotThemeManager.cpp

		Graph/Artists/R2toRFunctionArtist.h Graph/Artists/R2toRFunctionArtist.cpp

		Graph/Util/HistoryTexture2DUploadHelper.cpp
		Graph/Util/HistoryTexture2DUploadHelper.h

		Graph3D/Scene3DWindow.cpp
        Graph3D/Scene3DWindow.h
		Graph3D/Actor.cpp
		Graph3D/Actor.h
        Graph3D/Actors/R2toRFunctionActor.cpp
        Graph3D/Actors/R2toRFunctionActor.h
		OpenGL/ShaderLoader.cpp
		OpenGL/ShaderLoader.h

		MeshUtils.cpp
		MeshUtils.h
		Graph3D/Camera.cpp
		Graph3D/Camera.h
		Graph3D/Actors/TestActor.cpp
		Graph3D/Actors/TestActor.h

		SFML/Tools/Graph_deprydepr.cpp
		SFML/Tools/GraphAndAverageCalc.cpp

		SFML/ViewerUtils.cpp
		Graph/PlottingTheme.cpp
		Graph/PlottingTheme.h
		Graph/Artists/PointSetArtist.cpp
		Graph/Artists/PointSetArtist.h
		Graph/Artists/ParametricCurve2DArtist.cpp
		Graph/Artists/ParametricCurve2DArtist.h
		Graph/Util/PlotRegion.cpp
		Graph/Util/PlotRegion.h
		Graph/Artists/RtoRFunctionArtist.cpp
		Graph/Artists/RtoRFunctionArtist.h
		Graph/Artists/HistoryArtist.cpp
		Graph/Artists/HistoryArtist.h
		Graph/Plotter.cpp
		Graph/Plotter.h
		Graph/Artists/R2SectionArtist.cpp
		Graph/Artists/R2SectionArtist.h
        Window/WindowContainer/WindowContainer.cpp
        Window/WindowContainer/WindowContainer.h
		Types3D.cpp
		Types3D.h
		OpenGL/LegacyGL/SceneSetup.h
		Graph/Util/MathTool.cpp
		Graph/Util/MathTool.h
		Graph/Shapes/Shape.cpp
		Graph/Shapes/Shape.h
		OpenGL/LegacyGL/ShapeRenderer.cpp
		OpenGL/LegacyGL/ShapeRenderer.h
		OpenGL/LegacyGL/SceneSetup.cpp
		Graph/Util/PlottingRegion2D.cpp
		Graph/Util/PlottingRegion2D.h
        ImGui/ImGuiWindow.cpp
        ImGui/ImGuiWindow.h
		Graph/Util/PrintWindow.cpp
		Graph/Util/PrintWindow.h
		Graph/Util/FieldTextureKontraption.cpp
		Graph/Util/PlotStyleGUI.cpp
		Graph/Util/PlotStyleGUI.h

)
target_link_libraries(${LIB_GRAPHICS}
		Slab-Utils
		Slab-Core
		Slab-Math

)

message(NOTICE "Added library " ${LIB_GRAPHICS})