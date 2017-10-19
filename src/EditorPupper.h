#pragma once

#include <pupene/pupene.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include "puppers.h"


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

    EditorPupper(const std::string& title) {
        open_window(title);
    }

    ~EditorPupper() override = default;


    void layout_columns() {
        ImGui::SetColumnWidth(0, 140);
        ImGui::SetColumnWidth(1, 90);
        ImGui::SetColumnWidth(2, 40);
//        ImGui::SetColumnWidth(3, 240);
    }

    template <typename T>
    void prepare(T& value, const Meta& meta) {
        ImGui::Text("%s", meta.name.c_str());
        ImGui::NextColumn();

        ImGui::Text("0x%08x", meta.name.c_str());
        ImGui::NextColumn();

        ImGui::Text("%d", sizeof(value));
        ImGui::NextColumn();

        ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
    }

    template <typename T>
    PupPolicy begin_impl(T& value, const Meta& meta) {
        ImGui::PushID(&value);

        prepare(value, meta);
        ImGui::NextColumn();

        return PupPolicy::pup_object;
    }

    void end_impl() {
        ImGui::PopID();
    }

    template <typename T,
              typename = enable_if_decimal<T>>
    void pup_impl(T& value, const Meta& meta) {
        prepare(value, meta);
        std::string s{"##"};
        s.append(meta.name);

        ImGui::DragFloat(s.c_str(), &value, 0, 100);
        ImGui::NextColumn();
    }

    template <typename T,
              typename = void,
              typename = enable_if_integer<T>>
    void pup_impl(T& value, const Meta& meta) {
        prepare(value, meta);
        std::string s{"##"};
        s.append(meta.name);

        ImGui::DragInt(s.c_str(), &value, 0, 100);
        ImGui::NextColumn();
    }

    void pup_impl(std::string& value, const Meta& meta) {
        prepare(value, meta);
        std::string s{"###"};
        s.append(meta.name);

        ImGui::InputText(s.c_str(), &value[0], s.capacity());
        ImGui::NextColumn();
    }

    void flush() {
        ImGui::End();
    }

private:
    void open_window(const std::string& title) {
        bool open = true;
        std::string s{"Property editor: "};
        s.append(title);

        if (!ImGui::Begin(s.c_str(), &open)) {
            ImGui::End();
            return;
        }

//        layout_columns();
        ImGui::Columns(4, "title-with-no-end");
        ImGui::Separator();
        layout_columns();

        static ImVec4 color = {.25f, .75f, .9f, 1.f};
        ImGui::TextColored(color, "name"); ImGui::NextColumn();
        ImGui::TextColored(color, "addr"); ImGui::NextColumn();
        ImGui::TextColored(color, "size"); ImGui::NextColumn();

        ImGui::NextColumn();

        ImGui::Separator();

//        ImGui::Text(""); ImGui::NextColumn();
    }
};

template <>
pupene::PupPolicy EditorPupper::begin_impl(Color& value,
                                           const pupene::Meta& meta);

template <>
pupene::PupPolicy EditorPupper::begin_impl(vec2f& value,
                                           const pupene::Meta& meta);


#pragma clang diagnostic pop
