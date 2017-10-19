#include <iostream>
#include <fstream>
#include "puppers.h"
#include <pupene/pupene.h>
#include <imgui/imgui.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "imgui_impl_sdl_gl3.h"
#include "EditorPupper.h"

using std::literals::string_literals::operator""s;

static SDL_DisplayMode create_display() {
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS|SDL_INIT_TIMER) != 0)
    {
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

static void init_imgui(SDL_Window* window) {
    ImGui_ImplSdlGL3_Init(window);
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

bool poll_events() {
    bool done = false;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSdlGL3_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
            done = true;
    }

    auto io = ImGui::GetIO();
    static bool capture_text = false;
    if (!capture_text && io.WantTextInput) {
        capture_text = true;
        SDL_StartTextInput();
    } else if (capture_text && !io.WantTextInput) {
        capture_text = false;
        SDL_StopTextInput();
    }

    return done;
}

bool render_frame(SDL_Window* window) {
    glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
    glClearColor(0.1, 0.2, 0.4, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui::Render();
    SDL_GL_SwapWindow(window);

    return false;
}

namespace ui::widget {

    template <typename T>
    void object_editor(T& t, const std::string& title, bool debug = false) {
        auto pupper = EditorPupper{title};
        do_pup(pupper, t, debug);
        pupper.flush();
    }
}

int main () {
    auto display = create_display();
    SDL_Window* window = create_window();

    SDL_GLContext gl = SDL_GL_CreateContext(window);
    glewInit();

    init_imgui(window);

    auto vec = ImVec2{10.0, 20.0};
    auto vec2 = vec2f{10.0, 20.0};
    auto vec2b = vec2i{10, 20};


    auto vec22 = vec22fi{{10.0f, 4.0f}, {3, 4}};
    auto ct = complex_thing{};

    auto color = Color{1.f, 0.f, 0.f, 1.f};
    while (!poll_events()) {
        // concluded by render_frame()
        ImGui_ImplSdlGL3_NewFrame(window);

        bool yes = true;
        ImGui::ShowTestWindow(&yes);

        ui::widget::object_editor(ct, "phat");
        render_frame(window);
    }

    cleanup(window, gl);

    return 0;
}
