
set( GRAPHICS_BACKEND_SOURCE
		Backends/Implementations.h

		Backends/GLUT/GLUTBackend.h                      Backends/GLUT/GLUTBackend.cpp
		Backends/GLUT/GLUTListener.h                     Backends/GLUT/GLUTListener.cpp
		Backends/GLUT/GLUTEventTranslator.h              Backends/GLUT/GLUTEventTranslator.cpp

		Backends/GLFW/GLFWBackend.h                      Backends/GLFW/GLFWBackend.cpp
		Backends/GLFW/GLFWBackend_callbacks.cpp
		Backends/GLFW/GLFWListener.h                     Backends/GLFW/GLFWListener.cpp
		Backends/GLFW/GLFWEventTranslator.h              Backends/GLFW/GLFWEventTranslator.cpp

		Backends/SFML/SFMLBackend.h                      Backends/SFML/SFMLBackend.cpp
		Backends/SFML/SFMLEventTranslator.h              Backends/SFML/SFMLEventTranslator.cpp
		Backends/SFML/SFMLListener.h                     Backends/SFML/SFMLListener.cpp

		Backends/VTK/VTKBackend.h                        Backends/VTK/VTKBackend.cpp

		Backends/GraphicBackend.h                        Backends/GraphicBackend.cpp

		Backends/Events/GUIEventListener.h               Backends/Events/GUIEventListener.cpp
		Backends/Events/KeyMap.h
		Backends/Events/GUIEventTranslator.h             Backends/Events/GUIEventTranslator.cpp

		Backends/Modules/ImGui/ImGuiModule.h             Backends/Modules/ImGui/ImGuiModule.cpp
		Backends/Modules/ImGui/ImGuiModuleGLUT.h         Backends/Modules/ImGui/ImGuiModuleGLUT.cpp
		Backends/Modules/ImGui/ImGuiModuleGLFW.h         Backends/Modules/ImGui/ImGuiModuleGLFW.cpp
		Backends/Modules/ImGui/ImGuiColorAndStyles.h     Backends/Modules/ImGui/ImGuiColorAndStyles.cpp
		Backends/Modules/RealTimeAnimation.h             Backends/Modules/RealTimeAnimation.cpp
		Backends/Modules/Nuklear/NuklearModule.h         Backends/Modules/Nuklear/NuklearModule.cpp
		Backends/Modules/Nuklear/NuklearSFMLModule.h     Backends/Modules/Nuklear/NuklearSFMLModule.cpp
		Backends/Modules/Nuklear/NuklearGLFWModule.h     Backends/Modules/Nuklear/NuklearGLFWModule.cpp
		Backends/Modules/ModernOpenGLModule.h            Backends/Modules/ModernOpenGLModule.cpp
)

set( GRAPHICS_SOURCES
		Animation_depr.cpp
		ZoomPanRotate.cpp

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
		Graph/ℝ²↦ℝ/R2toROutputOpenGLGeneric.cpp


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
		Graph/ℝ↦ℝ/HistoryTexture2DUploadHelper.cpp
		Graph/ℝ↦ℝ/HistoryTexture2DUploadHelper.h
        Graph/ℝ↦ℝ/CorrelationsPanel.cpp
        Graph/ℝ↦ℝ/CorrelationsPanel.h
		Graph/ℝ²↦ℝ/R2toRFunctionRenderer.cpp
		Graph/ℝ²↦ℝ/R2toRFunctionRenderer.h
		Backends/GraphicBackendManager.cpp
		Backends/GraphicBackendManager.h

		SFML/Tools/Graph.cpp
		SFML/Tools/GraphAndAverageCalc.cpp

		SFML/ViewerUtils.cpp
)

set ( LIB_LOCAL_NAME ${STUDIOSLAB_BASE_LIB_NAME}-Graphics )

add_library(${LIB_LOCAL_NAME} STATIC ${GRAPHICS_SOURCES} ${GRAPHICS_BACKEND_SOURCE})
target_link_libraries(${LIB_LOCAL_NAME}
		Slab-Utils
		Slab-Core
		Slab-Models
		Slab-Maps-RtoR
		Slab-Maps-RtoR_GPU
		Slab-Maps-R2toR_GPU

		Slab-3rdParty-Imgui
		Slab-3rdParty-GLFreetype

		freeimage
		freetype-gl

		glut
		GLEW

		glfw

		GL
		OpenGL

		sfml-graphics
		# sfml-window
		# sfml-system
		)

message(NOTICE "Added library " ${LIB_LOCAL_NAME})