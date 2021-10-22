cmake_minimum_required(VERSION 3.16)
project(OpenGLStudies)

set(CMAKE_CXX_STANDARD 14)

set ( OpenGL_GL_PREFERENCE GLVND )
find_package(OpenGL)
find_package(Eigen3)

add_executable(ExampleNanogui exampleNanogui.cpp)

add_executable(OpenGLStudies
		main.cpp
		util/util.h

		OpenGLUtils/Program.h
		util/util.h OpenGLUtils/Program.h

		Studies/PendulumStudy.cpp
		Studies/PendulumStudy.h
		Studies/StudiesBase.cpp
		Studies/StudiesBase.h
		Studies/FieldStudy/FieldStudy.cpp
		Studies/FieldStudy/FieldStudy.h
		Studies/ChaosGame/LexyStudy.h
		Studies/ChaosGame/LexyStudy.cpp

		OpenGLUtils/Drawables/VertexBuffer.cpp
		OpenGLUtils/Drawables/VertexBuffer.h
		OpenGLUtils/Program.cpp
		OpenGLUtils/Utils.h
		OpenGLUtils/Utils.cpp

		util/util.cpp
		util/Clock.cpp
		util/Clock.h
		util/Vector.cpp
		util/Vector.h
		Studies/OgLplusStudy.cpp
		Studies/OgLplusStudy.h OpenGLUtils/Drawables/Drawable.cpp OpenGLUtils/Drawables/Drawable.h OpenGLUtils/Drawables/Mesh.cpp OpenGLUtils/Drawables/Mesh.h OpenGLUtils/Drawables/Plane.cpp OpenGLUtils/Drawables/Plane.h OpenGLUtils/Drawables/Primitives.h OpenGLUtils/Program.cpp OpenGLUtils/Drawables/PointSet.cpp OpenGLUtils/Drawables/PointSet.h Studies/ChaosGame/ChaosGameSet.cpp Studies/ChaosGame/ChaosGameSet.h)

target_link_libraries ( ExampleNanogui GL GLEW glfw nanogui)
target_link_libraries ( OpenGLStudies GL GLEW glfw nanogui dl) # dl is for stack stace
