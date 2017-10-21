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

void EditorPupper::open_window(const std::string& title) {
    bool open = true;
    std::string s{"Property editor: "};
    s.append(title);

    if (!ImGui::Begin(s.c_str(), &open)) {
        ImGui::End();
        return;
    }

    ImGui::Columns(4, "title-with-no-end");
    ImGui::Separator();
    layout_columns();

    static ImVec4 color = {.25f, .75f, .9f, 1.f};
    ImGui::TextColored(color, "name"); ImGui::NextColumn();
    ImGui::TextColored(color, "addr"); ImGui::NextColumn();
    ImGui::TextColored(color, "size"); ImGui::NextColumn();

    ImGui::NextColumn();

    ImGui::Separator();
}
