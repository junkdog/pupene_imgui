#include <iostream>
#include <array>
#include <algorithm>
#include <fstream>
#include "pup-fns.h"
#include <pupene/util.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "imgui_impl_sdl_gl3.h"
#include "EditorPupper.h"


void process_shortcuts(EditorConfig& config, bool is_key_down_event, int key);

using std::literals::string_literals::operator""s;


static SDL_DisplayMode create_display() {
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS|SDL_INIT_TIMER) != 0) {
        printf("Error: %s\n", SDL_GetError());
        throw std::runtime_error("sdl error:"s + SDL_GetError());
    }

    // Setup window
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);

    return current;
}

template <typename T>
T clamp(T&& v, T&& min, T&& max) {
    return (v > max) ? max
         : (v < min) ? min
                     : v;
}

static void update_styling(float hue_shift, float saturation_shift, float value_shift) {
    // https://gist.github.com/dougbinks/8089b4bbaccaaf6fa204236978d165a9#file-imguiutils-h-L9-L93
    ImGuiStyle& style = ImGui::GetStyle();
    style.Alpha = 1.0f;
    style.FrameRounding = 3.0f;
    style.Colors[ImGuiCol_Text]                  = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
    style.Colors[ImGuiCol_ChildWindowBg]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg]               = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
    style.Colors[ImGuiCol_Border]                = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
    style.Colors[ImGuiCol_BorderShadow]          = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
    style.Colors[ImGuiCol_FrameBg]               = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
    style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
    style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
    style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    style.Colors[ImGuiCol_ComboBg]               = ImVec4(0.86f, 0.86f, 0.86f, 0.99f);
    style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Button]                = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Header]                = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Column]                = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    style.Colors[ImGuiCol_ColumnHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
    style.Colors[ImGuiCol_ColumnActive]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
    style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    style.Colors[ImGuiCol_CloseButton]           = ImVec4(0.59f, 0.59f, 0.59f, 0.50f);
    style.Colors[ImGuiCol_CloseButtonHovered]    = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
    style.Colors[ImGuiCol_CloseButtonActive]     = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
    style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    style.Colors[ImGuiCol_ModalWindowDarkening]  = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

    float alpha = 1.f;
    // darken
    for (int i = 0; i <= ImGuiCol_COUNT; i++)
    {
        ImVec4& col = style.Colors[i];
        float H, S, V;
        ImGui::ColorConvertRGBtoHSV( col.x, col.y, col.z, H, S, V );

        H += hue_shift;
        if (H > 1.f) H -= 1.f;

        V = clamp(V + value_shift, 0.f, 1.f);
        S = clamp(S + saturation_shift, 0.f, 1.f);

        if (S < 0.1f) {
            V = 1.0f - V;
        }

        ImGui::ColorConvertHSVtoRGB(H, S, V, col.x, col.y, col.z);
        if (col.w < 1.00f) {
            col.w *= alpha;
        }
    }
}

bool file_exists(const std::string& file) {
    return static_cast<bool>(std::ifstream(file));
}


static void init_imgui(SDL_Window* window) {
    ImGui_ImplSdlGL3_Init(window);

    auto io = ImGui::GetIO();

    ImFontConfig config;
    config.OversampleH = 3;
    config.OversampleV = 2;
    config.GlyphExtraSpacing.x = 1.0f;

    auto file = "./SourceCodePro-Semibold.ttf";
    if (file_exists(file)) {
        io.Fonts->AddFontFromFileTTF(file, 14.0f, &config);
    } else {
        std::cerr << "Font not found in current directory: " << file << std::endl;
    }

    update_styling(0.12f, 0.f, 0.f);
}

static SDL_Window* create_window() {
    SDL_Window *window = SDL_CreateWindow(
            "pupene/imgui editor example",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            1280,
            720,
            SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);

    return window;
}

void cleanup(SDL_Window* window, SDL_GLContext gl) {
    ImGui_ImplSdlGL3_Shutdown();
    SDL_GL_DeleteContext(gl);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool poll_events(std::function<void(bool, int)>& callback) {
    bool done = false;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSdlGL3_ProcessEvent(&event, callback);
        if (event.type == SDL_QUIT)
            done = true;
    }

    return done;
}

template <typename Fn>
void render_frame(SDL_Window* window, Fn&& fn) {
    ImGui_ImplSdlGL3_NewFrame(window);

    fn();

    glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
    glClearColor(0.4, 0.5, 0.4, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui::Render();
    SDL_GL_SwapWindow(window);
}

namespace ui::widget {

    template <typename T>
    void object_editor(T& t, EditorConfig& config) {
        auto pupper = EditorPupper{config};
        pupene::pup(pupper, t, {});
        pupper.flush();
    }
}

int main() {
    create_display();
    SDL_Window* window = create_window();

    SDL_GLContext gl = SDL_GL_CreateContext(window);
    glewInit();

    init_imgui(window);

    auto ct = complex_thing{};

    auto config = EditorConfig{"object"};
    config.filter.pattern.reserve(50);

    auto cconfig = EditorConfig{"config instance"};
    cconfig.filter.pattern.reserve(50);

    std::function<void(bool, int)> key_callback = [&config](bool type_key_down, int key) {
        process_shortcuts(config, type_key_down, key);
    };

    while (!poll_events(key_callback)) {
        render_frame(window, [&ct, &config, &cconfig]() {
//            ImGui::ShowTestWindow(nullptr);

            ui::widget::object_editor(ct, config);
            ui::widget::object_editor(config, cconfig);
        });
    }

    cleanup(window, gl);

    return 0;
}

void process_shortcuts(EditorConfig& config,
                       bool is_key_down_event,
                       int key) {

    if (!is_key_down_event)
        return;

    auto& io = ImGui::GetIO();
    if (!io.KeyCtrl)
        return;

    auto toggle = [](bool& flag) { flag = !flag; };
    if (SDLK_f == key)
        toggle(config.filter.request_focus);
    if (SDLK_l == key)
        toggle(config.filter.show_parents);
    if (SDLK_d == key)
        config.filter.pattern = "";
}
