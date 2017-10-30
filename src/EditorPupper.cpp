#include <SDL2/SDL_keycode.h>
#include "EditorPupper.h"

using pupene::PupPolicy;
using pupene::Meta;

template<>
PupPolicy EditorPupper::begin_impl(Color& value,
                                   const Meta& meta) {

    object_to_widget(value, meta, [&value](auto label) {
        int flags = ImGuiColorEditFlags_RGB
                    | ImGuiColorEditFlags_HEX
                    | ImGuiColorEditFlags_HSV;

        ImGui::ColorEdit4(label, &value.r, flags);
    });

    return PupPolicy::consume_object;
}

template<>
pupene::PupPolicy EditorPupper::begin_impl(vec2f& value,
                                           const pupene::Meta& meta) {

    object_to_widget(value.x, meta, [&value](auto label) {
        ImGui::InputFloat2(label, &value.x);
    });

    return PupPolicy::consume_object;
}

void EditorPupper::layout_columns() {
    ImGui::SetColumnWidth(0, 140);
    ImGui::SetColumnWidth(1, 90);
    ImGui::SetColumnWidth(2, 45);
}

void EditorPupper::open_window() {
    bool open = true;
    std::string s{"edit: "};
    s.append(config.title);
    s.append("###");
    s.append(std::to_string(reinterpret_cast<uint64_t>(&config)));

    const auto flags = ImGuiWindowFlags_NoCollapse;
    if (!ImGui::Begin(s.c_str(), &open, ImVec2{450.f, 350.f}, -1.0f, flags)) {
        ImGui::End();
        return;
    }

    ImGui::Text("filter:");
    ImGuiTextEditCallback on_edit = [](auto data) -> int {
        auto& s = *static_cast<std::string*>(data->UserData);
        s = data->Buf;

        return 0;
    };
    ImGui::SameLine();

    if (config.filter.request_focus) {
        ImGui::SetKeyboardFocusHere();
        config.filter.request_focus = false;
    }

    ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
    ImGui::InputText("###filter",
                     &config.filter.pattern[0],
                     config.filter.pattern.capacity(),
                     ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CallbackAlways | ImGuiInputTextFlags_AutoSelectAll,
                     on_edit,
                     &config.filter);

    ImGui::PopItemWidth();
    ImGui::NextColumn();

    ImGui::Columns(4, "title-with-no-end");
    ImGui::Separator();
    layout_columns();

    ImVec4 color = color::header;
    ImGui::TextColored(color, "name"); ImGui::NextColumn();
    ImGui::TextColored(color, "addr"); ImGui::NextColumn();
    ImGui::TextColored(color, "size"); ImGui::NextColumn();

    ImGui::NextColumn();

    ImGui::Separator();
}

bool EditorPupper::is_filtered(const Meta& meta) {
    return !config.filter.pattern.empty()
        && meta.name.find(config.filter.pattern) == std::string::npos;
}

ImVec4 color::header = ImVec4{.25f, .75f, .9f, 1.f};
ImVec4 color::begin = ImVec4{0.454f, 0.551f, 0.505f, 1.f};
ImVec4 color::object = ImVec4{0.204f, 0.651f, 0.400f, 1.f};