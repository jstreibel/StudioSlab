//
// Created by joao on 20/04/2021.
//

#include "OgLplusStudy.h"

OGLplusStudy::OGLplusStudy() : buffs(2) {
    // Set the vertex shader source
    vs.Source(""
              "#version 330\n "
              "in vec3 Position; "
              "in vec3 Color; "
              "uniform mat4 Matrix; "
              "out vec4 vertColor; "
              "void main(void) "
              "{ "
              "    gl_Position = Matrix * vec4(Position, 1.0); "
              "    vertColor = vec4(Color, 1.0); "
              "} ");
    vs.Compile();
    fs.Source(""
              "#version 330\n "
              "in vec4 vertColor; "
              "out vec4 fragColor; "
              "void main(void) "
              "{ "
              "    fragColor = vertColor; "
              "} ");
    fs.Compile();
    prog.AttachShader(vs);
    prog.AttachShader(fs);
    prog.Link().Use();

    triangle.Bind();
    GLfloat triangle_verts[9] = {
            0.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f
    };

    buffs[0].Bind(oglplus::Buffer::Target::Array);

    oglplus::Buffer::Data(oglplus::Buffer::Target::Array, triangle_verts);
    oglplus::VertexArrayAttrib vert_attr(prog, "Position");
    vert_attr.Setup<oglplus::Vec3f>();
    vert_attr.Enable();

    GLfloat triangle_colors[9] = {
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f
    };
    // bind the VBO for the triangle colors
    buffs[1].Bind(oglplus::Buffer::Target::Array);
    // upload the data
    oglplus::Buffer::Data(oglplus::Buffer::Target::Array, triangle_colors);
    // setup the vertex attribs array
    oglplus::VertexArrayAttrib color_attr(prog, "Color");
    color_attr.Setup<oglplus::Vec3f>();
    color_attr.Enable();
// set the transformation matrix
    oglplus::Uniform<oglplus::Mat4f> u1(prog, "Matrix");
    u1.Set(oglplus::Mat4f(
            oglplus::Vec4f(2.0f, 0.0f, 0.0f, -1.0f),
            oglplus::Vec4f(0.0f, 2.0f, 0.0f, -1.0f),
            oglplus::Vec4f(0.0f, 0.0f, 2.0f, 0.0f),
            oglplus::Vec4f(0.0f, 0.0f, 0.0f, 1.0f)
    ));
    gl.ClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    gl.Disable(oglplus::Capability::DepthTest);
}

void OGLplusStudy::doYourThing() {
    gl.Clear().ColorBuffer();
    gl.DrawArrays(oglplus::PrimitiveType::Triangles, 0, 3);
}

