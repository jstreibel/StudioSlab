#include <GLES3/gl3.h>

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <string>

namespace {

    struct FVertex {
        GLfloat X;
        GLfloat Y;
        GLfloat R;
        GLfloat G;
        GLfloat B;
    };

    struct FWebGLApp {
        EMSCRIPTEN_WEBGL_CONTEXT_HANDLE Context = 0;
        GLuint Program = 0;
        GLuint Vao = 0;
        GLuint Vbo = 0;
        GLint AngleLocation = -1;
        double StartTimeSeconds = 0.0;
        int CanvasWidth = 0;
        int CanvasHeight = 0;
    };

    constexpr FVertex TriangleVertices[] = {
        { 0.0f,  0.60f, 1.0f, 0.2f, 0.2f},
        {-0.55f, -0.40f, 0.2f, 1.0f, 0.4f},
        { 0.55f, -0.40f, 0.3f, 0.5f, 1.0f}
    };

    constexpr const char *VertexShaderSource = R"glsl(
        #version 300 es
        precision highp float;

        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec3 aColor;

        uniform float uAngle;

        out vec3 vColor;

        void main() {
            mat2 rot = mat2(
                cos(uAngle), -sin(uAngle),
                sin(uAngle),  cos(uAngle));
            vec2 p = rot * aPos;
            gl_Position = vec4(p, 0.0, 1.0);
            vColor = aColor;
        }
    )glsl";

    constexpr const char *FragmentShaderSource = R"glsl(
        #version 300 es
        precision highp float;

        in vec3 vColor;
        out vec4 outColor;

        void main() {
            outColor = vec4(vColor, 1.0);
        }
    )glsl";

    auto PrintShaderLog(const GLuint shader) -> void {
        GLint logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength <= 1) return;

        std::string log(static_cast<std::size_t>(logLength), '\0');
        glGetShaderInfoLog(shader, logLength, nullptr, log.data());
        std::fprintf(stderr, "Shader compile log:\n%s\n", log.c_str());
    }

    auto PrintProgramLog(const GLuint program) -> void {
        GLint logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength <= 1) return;

        std::string log(static_cast<std::size_t>(logLength), '\0');
        glGetProgramInfoLog(program, logLength, nullptr, log.data());
        std::fprintf(stderr, "Program link log:\n%s\n", log.c_str());
    }

    auto CompileShader(const GLenum shaderType, const char *source) -> GLuint {
        const GLuint shader = glCreateShader(shaderType);
        if (shader == 0) return 0;

        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint ok = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
        if (ok == GL_TRUE) return shader;

        PrintShaderLog(shader);
        glDeleteShader(shader);
        return 0;
    }

    auto BuildProgram() -> GLuint {
        const GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, VertexShaderSource);
        if (vertexShader == 0) return 0;

        const GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, FragmentShaderSource);
        if (fragmentShader == 0) {
            glDeleteShader(vertexShader);
            return 0;
        }

        const GLuint program = glCreateProgram();
        if (program == 0) {
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            return 0;
        }

        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        GLint ok = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &ok);
        if (ok == GL_TRUE) return program;

        PrintProgramLog(program);
        glDeleteProgram(program);
        return 0;
    }

    auto EnsureCanvasSize(FWebGLApp &app) -> void {
        double cssWidth = 0.0;
        double cssHeight = 0.0;
        const auto result = emscripten_get_element_css_size("#canvas", &cssWidth, &cssHeight);
        if (result != EMSCRIPTEN_RESULT_SUCCESS || cssWidth <= 1.0 || cssHeight <= 1.0) {
            cssWidth = (app.CanvasWidth > 0) ? static_cast<double>(app.CanvasWidth) : 1280.0;
            cssHeight = (app.CanvasHeight > 0) ? static_cast<double>(app.CanvasHeight) : 720.0;
        }

        const int width = std::max(1, static_cast<int>(std::lround(cssWidth)));
        const int height = std::max(1, static_cast<int>(std::lround(cssHeight)));
        if (width == app.CanvasWidth && height == app.CanvasHeight) return;

        if (emscripten_set_canvas_element_size("#canvas", width, height) != EMSCRIPTEN_RESULT_SUCCESS) {
            std::fprintf(stderr, "Warning: failed to set canvas size to %dx%d.\n", width, height);
        }

        app.CanvasWidth = width;
        app.CanvasHeight = height;
    }

    auto InitializeGraphics(FWebGLApp &app) -> bool {
        app.Program = BuildProgram();
        if (app.Program == 0) {
            std::fprintf(stderr, "Error: failed to build WebGL shader program.\n");
            return false;
        }

        app.AngleLocation = glGetUniformLocation(app.Program, "uAngle");
        if (app.AngleLocation < 0) {
            std::fprintf(stderr, "Error: failed to resolve uniform location.\n");
            return false;
        }

        glGenVertexArrays(1, &app.Vao);
        glBindVertexArray(app.Vao);

        glGenBuffers(1, &app.Vbo);
        glBindBuffer(GL_ARRAY_BUFFER, app.Vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(TriangleVertices), TriangleVertices, GL_STATIC_DRAW);

        constexpr GLsizei stride = static_cast<GLsizei>(sizeof(FVertex));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0,
            2,
            GL_FLOAT,
            GL_FALSE,
            stride,
            reinterpret_cast<void *>(offsetof(FVertex, X)));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            stride,
            reinterpret_cast<void *>(offsetof(FVertex, R)));

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glEnable(GL_DEPTH_TEST);
        return true;
    }

    void RenderFrame(void *userData) {
        auto *app = static_cast<FWebGLApp *>(userData);
        if (app == nullptr) return;

        EnsureCanvasSize(*app);
        glViewport(0, 0, app->CanvasWidth, app->CanvasHeight);

        const double elapsed = emscripten_get_now() * 0.001 - app->StartTimeSeconds;
        const float angle = static_cast<float>(0.85 * elapsed);

        glClearColor(0.08f, 0.10f, 0.14f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(app->Program);
        glUniform1f(app->AngleLocation, angle);
        glBindVertexArray(app->Vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
    }

    auto InitializeWebGLContext(FWebGLApp &app) -> bool {
        EmscriptenWebGLContextAttributes attributes;
        emscripten_webgl_init_context_attributes(&attributes);
        attributes.alpha = EM_TRUE;
        attributes.depth = EM_TRUE;
        attributes.stencil = EM_FALSE;
        attributes.antialias = EM_TRUE;
        attributes.premultipliedAlpha = EM_FALSE;
        attributes.preserveDrawingBuffer = EM_FALSE;
        attributes.enableExtensionsByDefault = EM_TRUE;
        attributes.powerPreference = EM_WEBGL_POWER_PREFERENCE_HIGH_PERFORMANCE;
        attributes.majorVersion = 2;
        attributes.minorVersion = 0;

        app.Context = emscripten_webgl_create_context("#canvas", &attributes);
        if (app.Context <= 0) {
            std::fprintf(stderr, "Error: failed to create WebGL2 context (code=%d).\n", app.Context);
            return false;
        }

        const auto makeCurrentResult = emscripten_webgl_make_context_current(app.Context);
        if (makeCurrentResult != EMSCRIPTEN_RESULT_SUCCESS) {
            std::fprintf(stderr, "Error: failed to activate WebGL context (%d).\n", makeCurrentResult);
            return false;
        }

        return true;
    }

} // namespace

int main() {
    static FWebGLApp app{};

    if (!InitializeWebGLContext(app)) return 1;
    if (!InitializeGraphics(app)) return 1;

    app.StartTimeSeconds = emscripten_get_now() * 0.001;
    RenderFrame(&app);

    emscripten_set_main_loop_arg(RenderFrame, &app, 0, EM_TRUE);
    return 0;
}
