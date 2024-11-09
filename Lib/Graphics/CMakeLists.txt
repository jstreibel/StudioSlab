
set ( LIB_GRAPHICS ${STUDIOSLAB_BASE_LIB_NAME}-Graphics )


# ************* NANOTEX *****************

set ( NANOTEX_DIR ../3rdParty/NanoTeX)

option(NANOTEX_HAVE_LOG "If enable log" ON)
if (NANOTEX_HAVE_LOG)
	add_definitions(-DHAVE_LOG)
endif ()

option(NANOTEX_GRAPHICS_DEBUG "If enable graphics debug" OFF)
if (NANOTEX_GRAPHICS_DEBUG)
	add_definitions(-DGRAPHICS_DEBUG)
endif ()

option(NANOTEX_MEM_CHECK "If compile for memory check only" OFF)
if (NANOTEX_MEM_CHECK)
	add_definitions(-DMEM_CHECK)
endif ()

find_package(Fontconfig REQUIRED)
find_package(Fontconfig REQUIRED)
#pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-3.0)
#pkg_check_modules(GSVMM REQUIRED IMPORTED_TARGET gtksourceviewmm-3.0)
pkg_check_modules(CairoMM REQUIRED IMPORTED_TARGET cairomm-1.0)
pkg_check_modules(PangoMM REQUIRED IMPORTED_TARGET pangomm-1.4)

set ( NANOTEX_LIBS
		tinyxml2
		Fontconfig::Fontconfig
		PkgConfig::CairoMM
		PkgConfig::PangoMM)

set ( NANOTEX_SRC
		# atom folder
		${NANOTEX_DIR}/src/atom/atom_basic.cpp
		${NANOTEX_DIR}/src/atom/atom_char.cpp
		${NANOTEX_DIR}/src/atom/atom_impl.cpp
		${NANOTEX_DIR}/src/atom/atom_matrix.cpp
		${NANOTEX_DIR}/src/atom/atom_row.cpp
		${NANOTEX_DIR}/src/atom/atom_space.cpp
		${NANOTEX_DIR}/src/atom/colors_def.cpp
		${NANOTEX_DIR}/src/atom/unit_conversion.cpp
		# box folder
		${NANOTEX_DIR}/src/box/box.cpp
		${NANOTEX_DIR}/src/box/box_factory.cpp
		${NANOTEX_DIR}/src/box/box_group.cpp
		${NANOTEX_DIR}/src/box/box_single.cpp
		# core folder
		${NANOTEX_DIR}/src/core/core.cpp
		${NANOTEX_DIR}/src/core/formula.cpp
		${NANOTEX_DIR}/src/core/formula_def.cpp
		${NANOTEX_DIR}/src/core/glue.cpp
		${NANOTEX_DIR}/src/core/localized_num.cpp
		${NANOTEX_DIR}/src/core/macro.cpp
		${NANOTEX_DIR}/src/core/macro_def.cpp
		${NANOTEX_DIR}/src/core/macro_impl.cpp
		${NANOTEX_DIR}/src/core/parser.cpp
		# fonts folder
		${NANOTEX_DIR}/src/fonts/alphabet.cpp
		${NANOTEX_DIR}/src/fonts/font_basic.cpp
		${NANOTEX_DIR}/src/fonts/font_info.cpp
		${NANOTEX_DIR}/src/fonts/fonts.cpp
		# utils folder
		${NANOTEX_DIR}/src/utils/string_utils.cpp
		${NANOTEX_DIR}/src/utils/utf.cpp
		${NANOTEX_DIR}/src/utils/utils.cpp
		# res folder
		${NANOTEX_DIR}/src/res/builtin/formula_mappings.res.cpp
		${NANOTEX_DIR}/src/res/builtin/symbol_mapping.res.cpp
		${NANOTEX_DIR}/src/res/builtin/tex_param.res.cpp
		${NANOTEX_DIR}/src/res/builtin/tex_symbols.res.cpp
		${NANOTEX_DIR}/src/res/font/bi10.def.cpp
		${NANOTEX_DIR}/src/res/font/bx10.def.cpp
		${NANOTEX_DIR}/src/res/font/cmbsy10.def.cpp
		${NANOTEX_DIR}/src/res/font/cmbx10.def.cpp
		${NANOTEX_DIR}/src/res/font/cmbxti10.def.cpp
		${NANOTEX_DIR}/src/res/font/cmex10.def.cpp
		${NANOTEX_DIR}/src/res/font/cmmi10.def.cpp
		${NANOTEX_DIR}/src/res/font/cmmi10_unchanged.def.cpp
		${NANOTEX_DIR}/src/res/font/cmmib10.def.cpp
		${NANOTEX_DIR}/src/res/font/cmmib10_unchanged.def.cpp
		${NANOTEX_DIR}/src/res/font/cmr10.def.cpp
		${NANOTEX_DIR}/src/res/font/cmss10.def.cpp
		${NANOTEX_DIR}/src/res/font/cmssbx10.def.cpp
		${NANOTEX_DIR}/src/res/font/cmssi10.def.cpp
		${NANOTEX_DIR}/src/res/font/cmsy10.def.cpp
		${NANOTEX_DIR}/src/res/font/cmti10.def.cpp
		${NANOTEX_DIR}/src/res/font/cmti10_unchanged.def.cpp
		${NANOTEX_DIR}/src/res/font/cmtt10.def.cpp
		${NANOTEX_DIR}/src/res/font/dsrom10.def.cpp
		${NANOTEX_DIR}/src/res/font/eufb10.def.cpp
		${NANOTEX_DIR}/src/res/font/eufm10.def.cpp
		${NANOTEX_DIR}/src/res/font/i10.def.cpp
		${NANOTEX_DIR}/src/res/font/moustache.def.cpp
		${NANOTEX_DIR}/src/res/font/msam10.def.cpp
		${NANOTEX_DIR}/src/res/font/msbm10.def.cpp
		${NANOTEX_DIR}/src/res/font/r10.def.cpp
		${NANOTEX_DIR}/src/res/font/r10_unchanged.def.cpp
		${NANOTEX_DIR}/src/res/font/rsfs10.def.cpp
		${NANOTEX_DIR}/src/res/font/sb10.def.cpp
		${NANOTEX_DIR}/src/res/font/sbi10.def.cpp
		${NANOTEX_DIR}/src/res/font/si10.def.cpp
		${NANOTEX_DIR}/src/res/font/special.def.cpp
		${NANOTEX_DIR}/src/res/font/ss10.def.cpp
		${NANOTEX_DIR}/src/res/font/stmary10.def.cpp
		${NANOTEX_DIR}/src/res/font/tt10.def.cpp
		${NANOTEX_DIR}/src/res/parser/font_parser.cpp
		${NANOTEX_DIR}/src/res/parser/formula_parser.cpp
		${NANOTEX_DIR}/src/res/reg/builtin_font_reg.cpp
		${NANOTEX_DIR}/src/res/reg/builtin_syms_reg.cpp
		${NANOTEX_DIR}/src/res/sym/amsfonts.def.cpp
		${NANOTEX_DIR}/src/res/sym/amssymb.def.cpp
		${NANOTEX_DIR}/src/res/sym/base.def.cpp
		${NANOTEX_DIR}/src/res/sym/stmaryrd.def.cpp
		${NANOTEX_DIR}/src/res/sym/symspecial.def.cpp

		${NANOTEX_DIR}/src/latex.cpp
		${NANOTEX_DIR}/src/render.cpp
		OpenGL/CairoInterface.cpp
		OpenGL/CairoInterface.h
		OpenGL/WriterOpenGL.cpp
		OpenGL/WriterOpenGL.h
)

# *********** END NANOTEX ***************

file(GLOB_RECURSE GRAPHIC_BACKENDS_SOURCES "./Backend/*.cpp")
file(GLOB_RECURSE GRAPHIC_MODULES_SOURCES "./Modules/*.cpp")

add_library(${LIB_GRAPHICS} STATIC
		${GRAPHIC_BACKENDS_SOURCES}
		${GRAPHIC_MODULES_SOURCES}

		Backend/SystemWindow.cpp
		Backend/SystemWindow.h

        Types2D.h Types2D.cpp
		Utils.cpp

		${NANOTEX_SRC}
		LaTeX/LaTeXWriter.cpp
		LaTeX/LaTeX.cpp
		LaTeX/LaTeX.h

		OpenGL/LegacyGL/FunctionRenderer.cpp OpenGL/LegacyGL/FunctionRenderer.h

		DataViewers/Viewers/Viewer.cpp
        DataViewers/MainViewer.cpp
		DataViewers/Viewers/FourierViewer.cpp

		Styles/ColorMap.h 		Styles/ColorMap.cpp
		Styles/Colors.h       	Styles/Colors.cpp
		Window/WindowStyles.h Window/WindowStyles.cpp

		Window/GUIWindow.cpp
		Window/SlabWindow.cpp
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
		Utils/Writer.h Utils/Writer.cpp
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

        Plot2D/Artists/Artist.cpp
		Plot2D/Plot2DWindow.cpp
        Plot2D/Artists/LabelsArtist.h Plot2D/Artists/LabelsArtist.cpp
		Plot2D/Plot2DWindow_Axis.cpp
        Plot2D/PlotStyle.cpp
		Plot2D/Plot2DWindow_Events.cpp
		Plot2D/Plot2DWindow_GetSet.cpp
        Plot2D/Artists/XHairArtist.h Plot2D/Artists/XHairArtist.cpp
        Plot2D/Artists/AxisArtist.h Plot2D/Artists/AxisArtist.cpp

        Plot2D/PlotThemeManager.h Plot2D/PlotThemeManager.cpp

        Plot2D/Artists/R2toRFunctionArtist.h Plot2D/Artists/R2toRFunctionArtist.cpp

        Plot2D/Util/HistoryTexture2DUploadHelper.cpp
        Plot2D/Util/HistoryTexture2DUploadHelper.h

		Plot3D/Scene3DWindow.cpp
		Plot3D/Scene3DWindow.h
		Plot3D/Actor.cpp
		Plot3D/Actor.h
		Plot3D/Actors/R2toRFunctionActor.cpp
		Plot3D/Actors/R2toRFunctionActor.h
		OpenGL/ShaderLoader.cpp
		OpenGL/ShaderLoader.h

		MeshUtils.cpp
		MeshUtils.h
		Plot3D/Camera.cpp
		Plot3D/Camera.h
		Plot3D/Actors/TestActor.cpp
		Plot3D/Actors/TestActor.h

		SFML/Tools/Graph_deprydepr.cpp
		SFML/Tools/GraphAndAverageCalc.cpp

        Plot2D/PlottingTheme.cpp
        Plot2D/PlottingTheme.h
        Plot2D/Artists/PointSetArtist.cpp
        Plot2D/Artists/PointSetArtist.h
        Plot2D/Artists/ParametricCurve2DArtist.cpp
        Plot2D/Artists/ParametricCurve2DArtist.h
        Plot2D/Util/PlotRegion.cpp
        Plot2D/Util/PlotRegion.h
        Plot2D/Artists/RtoRFunctionArtist.cpp
        Plot2D/Artists/RtoRFunctionArtist.h
        Plot2D/Artists/HistoryArtist.cpp
        Plot2D/Artists/HistoryArtist.h
        Plot2D/Plotter.cpp
        Plot2D/Plotter.h
        Plot2D/Artists/R2SectionArtist.cpp
        Plot2D/Artists/R2SectionArtist.h
        Window/WindowContainer/WindowContainer.cpp
        Window/WindowContainer/WindowContainer.h
		Types3D.cpp
		Types3D.h
		OpenGL/LegacyGL/SceneSetup.h
        Plot2D/Util/MathTool.cpp
        Plot2D/Util/MathTool.h
        Plot2D/Shapes/Shape.cpp
        Plot2D/Shapes/Shape.h
		OpenGL/LegacyGL/ShapeRenderer.cpp
		OpenGL/LegacyGL/ShapeRenderer.h
		OpenGL/LegacyGL/SceneSetup.cpp
        Plot2D/Util/PlottingRegion2D.cpp
        Plot2D/Util/PlottingRegion2D.h
        ImGui/ImGuiWindow.cpp
        ImGui/ImGuiWindow.h
        Plot2D/Util/PrintWindow.cpp
        Plot2D/Util/PrintWindow.h
        Plot2D/Util/FieldTextureKontraption.cpp
        Plot2D/Util/PlotStyleGUI.cpp
        Plot2D/Util/PlotStyleGUI.h
        Window/WindowProxy.cpp
        Window/WindowProxy.h
        Plot2D/Artists/Painters/Painter.cpp
        Plot2D/Artists/Painters/Painter.h
        Plot2D/Artists/Painters/Colormap1DPainter.cpp
        Plot2D/Artists/Painters/Colormap1DPainter.h
        Plot2D/Artists/Painters/RealValued2DPainter.cpp
        Plot2D/Artists/Painters/RealValued2DPainter.h
        Plot2D/Artists/Painters/HeightmapShadingPainter.cpp
        Plot2D/Artists/Painters/HeightmapShadingPainter.h
		DataViewers/Viewers/HistoryViewer.cpp
		DataViewers/Viewers/HistoryViewer.h
		DataViewers/Viewers/ModesHistoryViewer.cpp
		DataViewers/Viewers/ModesHistoryViewer.h
		SlabGraphics.cpp
		SlabGraphics.h
		GraphicsModule.cpp
		SlabGraphicsModule.h
		OpenGL/WriterOpenGL.cpp
		OpenGL/WriterOpenGL.h
		Modules/GraphicsModule.h
        Window/SlabWindowManager.cpp
        Window/SlabWindowManager.h
        Window/Decorator.cpp
        Window/Decorator.h
        Plot2D/Artists/BackgroundArtist.cpp
        Plot2D/Artists/BackgroundArtist.h
        Backend/GLFW/GLFWSystemWindow.cpp
        Backend/GLFW/GLFWSystemWindow.h
		OpenGL/Images.cpp
		OpenGL/Images.h
        Utils/ImageLoad.cpp
        Utils/ImageLoad.h
		Backend/SFML/SFMLSystemWindow.cpp
		Backend/SFML/SFMLSystemWindow.h
		Backend/Events/MouseState.cpp
		Modules/ImGui/Slab-ImGui-Interop.h
		Modules/ImGui/Slab-ImGui-Interop.cpp
)
target_link_libraries(${LIB_GRAPHICS}
		Slab-3rdParty-Imgui
		Slab-Utils
		Slab-Core
		Slab-Math

		${NANOTEX_LIBS}
)
target_include_directories(${LIB_GRAPHICS} PUBLIC ${NANOTEX_DIR}/src)

message(NOTICE "Added library " ${LIB_GRAPHICS})