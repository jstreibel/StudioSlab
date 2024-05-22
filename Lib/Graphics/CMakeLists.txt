

set ( LIB_LOCAL_NAME ${STUDIOSLAB_BASE_LIB_NAME}-Graphics )

add_library(${LIB_LOCAL_NAME} STATIC
		Types.h     	Types.cpp
		Utils.cpp

		Animator.cpp	Animator.h

		OpenGL/LegacyGL/FunctionRenderer.cpp OpenGL/LegacyGL/FunctionRenderer.h

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
		Graph/LabelingHelper.h				Graph/LabelingHelper.cpp
		Graph/PlottingWindow_Axis.cpp
		Graph/PlotStyle.cpp
		Graph/PlottingWindow_Events.cpp
		Graph/PlottingWindow_GetSet.cpp
		Graph/Artists/XHairArtist.h			Graph/Artists/XHairArtist.cpp
		Graph/Artists/AxisArtist.h			Graph/Artists/AxisArtist.cpp

		Graph/PlotThemeManager.h Graph/PlotThemeManager.cpp

		Graph/Artists/R2toRFunctionArtist.h Graph/Artists/R2toRFunctionArtist.cpp
		Graph/ℝ²↦ℝ/R2toROutputOpenGLGeneric.cpp

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
		Graph3D/Camera.cpp
		Graph3D/Camera.h
		Graph3D/TestActor.cpp
		Graph3D/TestActor.h
		Graph/ℝ↦ℝ/RtoRStatisticalMonitor.cpp
		Graph/ℝ↦ℝ/RtoRStatisticalMonitor.h
		Graph/ℝ↦ℝ/RtoRRealtimePanel.cpp
		Graph/ℝ↦ℝ/RtoRRealtimePanel.h
		Graph/ℝ↦ℝ/RtoRPanel.cpp
		Graph/ℝ↦ℝ/RtoRPanel.h
		Graph/ℝ↦ℝ/RtoRFourierPanel.cpp
		Graph/ℝ↦ℝ/RtoRFourierPanel.h
		Graph/Artists/HistoryTexture2DUploadHelper.cpp
		Graph/Artists/HistoryTexture2DUploadHelper.h
		Graph/ℝ↦ℝ/CorrelationsPanel.cpp
		Graph/ℝ↦ℝ/CorrelationsPanel.h
		Graph/ℝ²↦ℝ/R2toRFunctionRenderer.cpp
		Graph/ℝ²↦ℝ/R2toRFunctionRenderer.h

		SFML/Tools/Graph.cpp
		SFML/Tools/GraphAndAverageCalc.cpp

		SFML/ViewerUtils.cpp
		Graph/PlottingTheme.cpp
		Graph/PlottingTheme.h
		Graph/Artists/PointSetArtist.cpp
		Graph/Artists/PointSetArtist.h
		Graph/Artists/ParametricCurve2DArtist.cpp
		Graph/Artists/ParametricCurve2DArtist.h
		Graph/PlotRegion.cpp
		Graph/PlotRegion.h
		Graph/Artists/RtoRFunctionArtist.cpp
		Graph/Artists/RtoRFunctionArtist.h
		Graph/Artists/HistoryArtist.cpp
		Graph/Artists/HistoryArtist.h
		Graph/Plotter.cpp
		Graph/Plotter.h
		Graph/Artists/R2SectionArtist.cpp
		Graph/Artists/R2SectionArtist.h

)
target_link_libraries(${LIB_LOCAL_NAME}
		Slab-Utils
		Slab-Core
		Slab-Models
)

message(NOTICE "Added library " ${LIB_LOCAL_NAME})