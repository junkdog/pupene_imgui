#include "EditorPupper.h"

using pupene::PupPolicy;
using pupene::Meta;

template<>
PupPolicy EditorPupper::begin_impl(Color& value,
                                   const Meta& meta) {

    prepare(value, meta);

    std::string s{"##"};
    s.append(meta.name);

    ImGui::PushID(&value);
    ImGui::ColorEdit4(s.c_str(), &value.r,
                      ImGuiColorEditFlags_RGB
                      | ImGuiColorEditFlags_HEX
                      | ImGuiColorEditFlags_HSV);

    ImGui::NextColumn();

    return PupPolicy::consume_object;
}

template<>
pupene::PupPolicy EditorPupper::begin_impl(vec2f& value,
                                           const pupene::Meta& meta) {
    prepare(value, meta);

    std::string s{"##"};
    s.append(meta.name);

    ImGui::PushID(&value);
    ImGui::InputFloat2(s.c_str(), &value.x);

    ImGui::NextColumn();

    return PupPolicy::consume_object;
}
