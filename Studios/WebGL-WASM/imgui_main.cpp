#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include <cstdio>

namespace {

    struct FImGuiSandboxApp {
        GLFWwindow *Window = nullptr;
        bool ShowImGuiDemo = true;
        ImVec4 ClearColor = ImVec4(0.08f, 0.10f, 0.14f, 1.0f);
    };

    auto ErrorCallback(const int errorCode, const char *description) -> void {
        std::fprintf(stderr, "GLFW error %d: %s\n", errorCode, description);
    }

    auto Shutdown(FImGuiSandboxApp &app) -> void {
        if (app.Window == nullptr) return;

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(app.Window);
        app.Window = nullptr;
        glfwTerminate();
    }

    auto RenderFrame(void *userData) -> void {
        auto *app = static_cast<FImGuiSandboxApp *>(userData);
        if (app == nullptr || app->Window == nullptr) return;

        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (app->ShowImGuiDemo) {
            ImGui::ShowDemoWindow(&app->ShowImGuiDemo);
        }

        ImGui::SetNextWindowPos(ImVec2(16.0f, 16.0f), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(420.0f, 0.0f), ImGuiCond_FirstUseEver);
        ImGui::Begin("StudioSlab WASM Sandbox");
        ImGui::Text("Minimal reusable browser target");
        ImGui::Separator();
        ImGui::BulletText("GUI: Dear ImGui");
        ImGui::BulletText("Window/input: Emscripten GLFW port");
        ImGui::BulletText("Rendering: OpenGL ES 3 / WebGL2");
        ImGui::Checkbox("Show Dear ImGui demo", &app->ShowImGuiDemo);
        ImGui::ColorEdit3("Clear color", reinterpret_cast<float *>(&app->ClearColor));
        const auto &io = ImGui::GetIO();
        ImGui::Text("Framebuffer scale: %.2f x %.2f", io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();

        ImGui::Render();

        int framebufferWidth = 1;
        int framebufferHeight = 1;
        glfwGetFramebufferSize(app->Window, &framebufferWidth, &framebufferHeight);

        glViewport(0, 0, framebufferWidth, framebufferHeight);
        glClearColor(app->ClearColor.x, app->ClearColor.y, app->ClearColor.z, app->ClearColor.w);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(app->Window);
    }

} // namespace

auto main() -> int {
    glfwSetErrorCallback(ErrorCallback);
    if (!glfwInit()) {
        std::fprintf(stderr, "Failed to initialize GLFW.\n");
        return 1;
    }

    constexpr const char *glslVersion = "#version 300 es";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

    FImGuiSandboxApp app{};
    app.Window = glfwCreateWindow(1280, 720, "StudioSlab WASM ImGui Sandbox", nullptr, nullptr);
    if (app.Window == nullptr) {
        std::fprintf(stderr, "Failed to create GLFW window.\n");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(app.Window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    auto &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.IniFilename = nullptr;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(app.Window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(app.Window, "#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glslVersion);

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(RenderFrame, &app, 0, true);
    return 0;
#else
    while (!glfwWindowShouldClose(app.Window)) {
        RenderFrame(&app);
    }

    Shutdown(app);
    return 0;
#endif
}
