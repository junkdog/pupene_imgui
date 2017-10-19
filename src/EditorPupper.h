#pragma once

#include <pupene/pupene.h>
#include <imgui/imgui.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat"
class EditorPupper : public pupene::Pupper<EditorPupper> {
public:
    using Meta = pupene::Meta;
    using PupPolicy = pupene::PupPolicy;

    template <typename T>
    using enable_if_decimal = std::enable_if_t<std::is_floating_point<T>{}>;

    template <typename T>
    using enable_if_integer = std::enable_if_t<std::is_integral<T>{}>;

    EditorPupper() {
        open_window();
    }

    ~EditorPupper() override = default;

    template <typename T>
    PupPolicy begin_impl(T& value, const Meta& meta) {
//        ImGui::PushID(&value);
        ImGui::Text("object: %s", meta.name.c_str());
        return PupPolicy::pup_object;
    }

    void end_impl() {
//        ImGui::PopID();
    }

    template <typename T,
              typename = enable_if_decimal<T>>
    void pup_impl(T& value, const Meta& meta) {
        ImGui::Text("%s (floating:0x%08x)", meta.name.c_str(), &value);
//        ImGui::SliderFloat(meta.name.c_str(), &value, 0, 100);
        ImGui::InputFloat(meta.name.c_str(), &value, 0.f, 100.f);
    }

    template <typename T,
              typename = void,
              typename = enable_if_integer<T>>
    void pup_impl(T& value, const Meta& meta) {
        ImGui::Text("%s (integral:0x%08x)", meta.name.c_str(), &value);
        ImGui::InputInt(meta.name.c_str(), &value, 0, 100);
    }

    void pup_impl(std::string& s, const Meta& meta) {
        ImGui::Text("%s (string:0x%08x)", meta.name.c_str(), &s);
        ImGui::InputText(meta.name.c_str(), &s[0], s.capacity());
    }

private:
    void open_window() {
//        bool open = true;
//        if (!ImGui::Begin("Example: Property editor", &open)) {
//            ImGui::End();
//            return;
//        }
//
//        ImGui::Text("my sailor is rich");
//        ImGui::End();
    }
};
#pragma clang diagnostic pop
