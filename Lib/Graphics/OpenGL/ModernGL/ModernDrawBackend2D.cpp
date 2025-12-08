//
// Modern OpenGL backend implementing IDrawBackend2D
//

#include "ModernDrawBackend2D.h"

#include "Graphics/OpenGL/ShaderLoader.h"
#include "Graphics/Utils.h"
#include "Core/Tools/Resources.h"
#include "Core/Tools/Log.h"

#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace Slab::Graphics::OpenGL::Modern {

namespace {
constexpr const char* kColorVert = R"GLSL(
    #version 330 core
    layout(location = 0) in vec2 aPos;
    layout(location = 1) in float aZ;
    layout(location = 2) in vec4 aColor;

    uniform mat4 uProjection;

    out vec4 vColor;

    void main() {
        gl_Position = uProjection * vec4(aPos, aZ, 1.0);
        vColor = aColor;
    }
)GLSL";

constexpr const char* kColorFrag = R"GLSL(
    #version 330 core
    in vec4 vColor;
    out vec4 FragColor;
    void main() { FragColor = vColor; }
)GLSL";

constexpr const char* kTexVert = R"GLSL(
    #version 330 core
    layout(location = 0) in vec2 aPos;
    layout(location = 1) in float aZ;
    layout(location = 2) in vec2 aUV;

    uniform mat4 uProjection;

    out vec2 vUV;

    void main() {
        gl_Position = uProjection * vec4(aPos, aZ, 1.0);
        vUV = aUV;
    }
)GLSL";

constexpr const char* kTexFrag = R"GLSL(
    #version 330 core
    in vec2 vUV;
    out vec4 FragColor;

    uniform sampler2D uTexture;
    uniform vec4 uModulation;

    void main() {
        FragColor = uModulation * texture(uTexture, vUV);
    }
)GLSL";

GLuint CompileProgram(const char* vertSrc, const char* fragSrc) {
    GLuint vert = ShaderLoader::Compile(vertSrc, ShaderType::VertexShader);
    GLuint frag = ShaderLoader::Compile(fragSrc, ShaderType::FragmentShader);

    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    GLint linked = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (linked == GL_FALSE) {
        GLchar messages[512];
        glGetProgramInfoLog(program, sizeof(messages), nullptr, messages);
        Core::Log::Error() << "Modern GL backend shader link failed: " << messages << Core::Log::Flush;
        glDeleteProgram(program);
        program = 0;
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
    return program;
}

glm::mat4 MakeOrtho(const RectR& view) {
    return glm::ortho(static_cast<float>(view.xMin),
                      static_cast<float>(view.xMax),
                      static_cast<float>(view.yMin),
                      static_cast<float>(view.yMax),
                      -1.0f, 1.0f);
}

} // namespace

FModernDrawBackend2D::FModernDrawBackend2D() {
    ColorProgram = CompileProgram(kColorVert, kColorFrag);
    TextureProgram = CompileProgram(kTexVert, kTexFrag);

    InitColorPipeline();
    InitTexturePipeline();
}

FModernDrawBackend2D::~FModernDrawBackend2D() {
    if (ColorVBO) glDeleteBuffers(1, &ColorVBO);
    if (ColorVAO) glDeleteVertexArrays(1, &ColorVAO);
    if (TexVBO) glDeleteBuffers(1, &TexVBO);
    if (TexVAO) glDeleteVertexArrays(1, &TexVAO);
    if (ColorProgram) glDeleteProgram(ColorProgram);
    if (TextureProgram) glDeleteProgram(TextureProgram);
}

FDrawBackend2DCapabilities FModernDrawBackend2D::GetCapabilities() const noexcept {
    return {
        .SupportsTextures = true,
        .SupportsText = true,
    };
}

void FModernDrawBackend2D::BeginFrame(const RectI& viewport, const RectR& view) const noexcept {
    glViewport(viewport.xMin, viewport.yMin, viewport.GetWidth(), viewport.GetHeight());
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);

    Projection = MakeOrtho(view);
    View = view;
    Viewport = viewport;

    EnsureWriter();
    if (Writer != nullptr) {
        Writer->Reshape(viewport.GetWidth(), viewport.GetHeight());
        Writer->SetPenPositionTransform([this](const FPoint2D& pt) {
            return FromSpaceToViewportCoord(pt, View, Viewport);
        });
    }
}

void FModernDrawBackend2D::EndFrame() const noexcept { }

void FModernDrawBackend2D::InitColorPipeline() {
    glGenVertexArrays(1, &ColorVAO);
    glGenBuffers(1, &ColorVBO);

    glBindVertexArray(ColorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, ColorVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(FColorVertex), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(FColorVertex), reinterpret_cast<void*>(sizeof(glm::vec2)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(FColorVertex),
                          reinterpret_cast<void*>(sizeof(glm::vec2) + sizeof(float)));
    glBindVertexArray(0);
}

void FModernDrawBackend2D::InitTexturePipeline() {
    glGenVertexArrays(1, &TexVAO);
    glGenBuffers(1, &TexVBO);

    glBindVertexArray(TexVAO);
    glBindBuffer(GL_ARRAY_BUFFER, TexVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(FTexVertex), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(FTexVertex), reinterpret_cast<void*>(sizeof(glm::vec2)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(FTexVertex),
                          reinterpret_cast<void*>(sizeof(glm::vec2) + sizeof(float)));
    glBindVertexArray(0);
}

void FModernDrawBackend2D::DrawColor(GLenum mode, const std::vector<FColorVertex>& vertices, float lineWidth) const {
    if (vertices.empty() || ColorProgram == 0) return;

    glUseProgram(ColorProgram);
    const GLint projLoc = glGetUniformLocation(ColorProgram, "uProjection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Projection[0][0]);

    glBindVertexArray(ColorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, ColorVBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(FColorVertex)),
                 vertices.data(), GL_DYNAMIC_DRAW);

    if (mode == GL_LINES) glLineWidth(lineWidth);
    if (mode == GL_POINTS) glPointSize(lineWidth);

    glDrawArrays(mode, 0, static_cast<GLsizei>(vertices.size()));

    glBindVertexArray(0);
}

void FModernDrawBackend2D::DrawTexture(GLenum mode, const std::vector<FTexVertex>& vertices,
                                       const FColor& modulation,
                                       const OpenGL::FTexture& texture) const {
    if (vertices.empty() || TextureProgram == 0) return;

    glUseProgram(TextureProgram);
    const GLint projLoc = glGetUniformLocation(TextureProgram, "uProjection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Projection[0][0]);

    const GLint modLoc = glGetUniformLocation(TextureProgram, "uModulation");
    glUniform4f(modLoc, modulation.r, modulation.g, modulation.b, modulation.a);

    glActiveTexture(GL_TEXTURE0);
    texture.Activate();
    texture.Bind();
    const GLint texLoc = glGetUniformLocation(TextureProgram, "uTexture");
    glUniform1i(texLoc, 0);

    glBindVertexArray(TexVAO);
    glBindBuffer(GL_ARRAY_BUFFER, TexVBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(FTexVertex)),
                 vertices.data(), GL_DYNAMIC_DRAW);

    glDrawArrays(mode, 0, static_cast<GLsizei>(vertices.size()));
    glBindVertexArray(0);
}

void FModernDrawBackend2D::DrawPointSet(const Math::FPointSet_constptr& pSet, const PlotStyle& style) const noexcept {
    if (pSet == nullptr) return;
    DrawPointSet(*pSet, style);
}

void FModernDrawBackend2D::DrawPointSet(const Math::FPointSet& pSet, const PlotStyle& style) const noexcept {
    const auto& pts = pSet.GetPoints();
    if (pts.empty()) return;

    auto toColorVerts = [&](const FColor& color) {
        std::vector<FColorVertex> verts;
        verts.reserve(pts.size());
        for (const auto& p : pts) verts.push_back({{p.x, p.y}, 0.0f, {color.r, color.g, color.b, color.a}});
        return verts;
    };

    auto drawPrim = [&](GLenum mode, float width, const FColor& color) {
        auto verts = toColorVerts(color);
        DrawColor(mode, verts, width);
    };

    switch (style.getPrimitive()) {
    case PlottingSolid:
    case PlottingDotted:
    case PlottingDashed:
    case PlottingDotDashed:
    case PlottingVerticalLinesWithCircles:
    case LinePrimitive::Lines:
    case LinePrimitive::LineStrip:
    case LinePrimitive::LineLoop: {
        GLenum mode = GL_LINE_STRIP;
        if (style.getPrimitive() == LinePrimitive::Lines) mode = GL_LINES;
        else if (style.getPrimitive() == LinePrimitive::LineLoop) mode = GL_LINE_LOOP;
        else if (style.getPrimitive() == PlottingVerticalLinesWithCircles) {
            drawPrim(GL_POINTS, style.thickness * 5.0f, style.lineColor);
            std::vector<FColorVertex> lines;
            lines.reserve(pts.size() * 2);
            for (const auto& p : pts) {
                lines.push_back({{p.x, 0.0f}, 0.0f, {style.lineColor.r, style.lineColor.g, style.lineColor.b, style.lineColor.a}});
                lines.push_back({{p.x, p.y}, 0.0f, {style.lineColor.r, style.lineColor.g, style.lineColor.b, style.lineColor.a}});
            }
            DrawColor(GL_LINES, lines, style.thickness);
            return;
        }
        drawPrim(mode, style.thickness, style.lineColor);
        return;
    }
    case LinePrimitive::Points:
        drawPrim(GL_POINTS, style.thickness, style.lineColor);
        return;
    case LinePrimitive::Triangles:
    case LinePrimitive::TriangleStrip:
    case LinePrimitive::TriangleFan:
    case Quads: {
        std::vector<FColorVertex> verts;
        verts.reserve(pts.size());
        for (const auto& p : pts) verts.push_back({{p.x, p.y}, 0.0f,
                                                   {style.fillColor.r, style.fillColor.g, style.fillColor.b, style.fillColor.a}});

        GLenum mode = GL_TRIANGLES;
        if (style.getPrimitive() == LinePrimitive::TriangleStrip) mode = GL_TRIANGLE_STRIP;
        else if (style.getPrimitive() == LinePrimitive::TriangleFan) mode = GL_TRIANGLE_FAN;
        else if (style.getPrimitive() == LinePrimitive::Quads) mode = GL_TRIANGLE_FAN;

        if (style.filled) DrawColor(mode, verts, style.thickness);
        DrawColor(GL_LINE_LOOP, toColorVerts(style.lineColor), style.thickness);
        return;
    }
    case LinePrimitive::__COUNT__:
    default:
        return;
    }
}

void FModernDrawBackend2D::DrawSolidQuad(const FRectangleShape& rect, const FColor& color, DevFloat zIndex) const noexcept {
    std::vector<FColorVertex> verts{
        {{rect.top_left.x, rect.top_left.y}, static_cast<float>(zIndex), {color.r, color.g, color.b, color.a}},
        {{rect.bottom_right.x, rect.top_left.y}, static_cast<float>(zIndex), {color.r, color.g, color.b, color.a}},
        {{rect.bottom_right.x, rect.bottom_right.y}, static_cast<float>(zIndex), {color.r, color.g, color.b, color.a}},
        {{rect.top_left.x, rect.bottom_right.y}, static_cast<float>(zIndex), {color.r, color.g, color.b, color.a}},
    };
    DrawColor(GL_TRIANGLE_FAN, verts);
}

void FModernDrawBackend2D::DrawLineSegments(std::span<const FLineSegment2D> segments,
                                            const FColor& color,
                                            DevFloat width) const noexcept {
    if (segments.empty()) return;
    std::vector<FColorVertex> verts;
    verts.reserve(segments.size() * 2);
    for (const auto& s : segments) {
        verts.push_back({{s.A.x, s.A.y}, (float)s.Z, {color.r, color.g, color.b, color.a}});
        verts.push_back({{s.B.x, s.B.y}, (float)s.Z, {color.r, color.g, color.b, color.a}});
    }
    DrawColor(GL_LINES, verts, width);
}

void FModernDrawBackend2D::DrawTriangleStrip(std::span<const FColoredVertex2D> vertices) const noexcept {
    if (vertices.size() < 3) return;
    std::vector<FColorVertex> verts;
    verts.reserve(vertices.size());
    for (const auto& v : vertices) {
        verts.push_back({{v.Position.x, v.Position.y},
                         (float)v.Z,
                         {v.Color.r, v.Color.g, v.Color.b, v.Color.a}});
    }
    DrawColor(GL_TRIANGLE_STRIP, verts);
}

void FModernDrawBackend2D::DrawTexturedQuad(const std::array<FTexturedVertex2D, 4>& vertices,
                                            const FColor& modulation,
                                            const OpenGL::FTexture& texture) const noexcept {
    std::vector<FTexVertex> verts;
    verts.reserve(vertices.size());
    for (const auto& v : vertices) {
        verts.push_back({{v.Position.x, v.Position.y}, (float)v.Z, {v.UV.x, v.UV.y}});
    }
    DrawTexture(GL_TRIANGLE_FAN, verts, modulation, texture);
}

void FModernDrawBackend2D::DrawVerticalGradientQuad(const FRectangleShape& rect,
                                                    const FColor& bottom,
                                                    const FColor& top,
                                                    DevFloat zIndex) const noexcept {
    std::array<FColoredVertex2D, 4> strip{{
        {{rect.top_left.x, rect.bottom_right.y}, static_cast<float>(zIndex), bottom},
        {{rect.bottom_right.x, rect.bottom_right.y}, static_cast<float>(zIndex), bottom},
        {{rect.top_left.x, rect.top_left.y}, static_cast<float>(zIndex), top},
        {{rect.bottom_right.x, rect.top_left.y}, static_cast<float>(zIndex), top},
    }};
    DrawTriangleStrip(strip);
}

void FModernDrawBackend2D::EnsureWriter() const {
    if (Writer != nullptr) return;
    Writer = Slab::New<FWriterOpenGL>(Slab::Core::Resources::BuiltinFonts::JuliaMono_Regular(), 28);
}

void FModernDrawBackend2D::DrawText(const Str& text, const FPoint2D& position, const FColor& color) const noexcept {
    EnsureWriter();
    if (Writer == nullptr) return;
    Writer->Write(text, position, color);
}

} // Slab::Graphics::OpenGL::Modern
