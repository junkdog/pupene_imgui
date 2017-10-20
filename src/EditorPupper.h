#pragma once

#include <pupene/pupene.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include "puppers.h"
#include "traits.h"


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat"
class EditorPupper : public pupene::Pupper<EditorPupper> {
public:
    using Meta = pupene::Meta;
    using PupPolicy = pupene::PupPolicy;

    explicit EditorPupper(const std::string& title) {
        open_window(title);
    }

    ~EditorPupper() override = default;

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
        pup_to_widget(value, meta, [&value](auto label) {
            ImGui::DragFloat(label, &value, 0, 100);
        });
    }

    template <typename T,
              typename = void,
              typename = enable_if_integer<T>>
    void pup_impl(T& value, const Meta& meta) {
        pup_to_widget(value, meta, [&value](auto label) {
            ImGui::DragInt(label, &value, 0, 100);
        });
    }

    void pup_impl(std::string& value, const Meta& meta) {
        pup_to_widget(value, meta, [&value](auto label) {
            ImGui::InputText(label, &value[0], value.capacity());
        });
    }

    void flush() {
        ImGui::End();
    }

private:
    template <typename T, typename Fn>
    void pup_to_widget(T& value,
                       const Meta& meta,
                       Fn&& wpup) {

        std::string label = prepare(value, meta);

        ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
        wpup(label.c_str());
        ImGui::PopItemWidth();

        ImGui::NextColumn();
    }

    template <typename T, typename Fn>
    void object_to_widget(T& value,
                               const Meta& meta,
                               Fn&& wpup) {

        std::string label = prepare(value, meta);
        ImGui::PushID(&value);

        ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
        wpup(label.c_str());
        ImGui::PopItemWidth();

        ImGui::NextColumn();
    }

    void open_window(const std::string& title);

    template <typename T>
    std::string prepare(T& value, const Meta& meta) {
        ImGui::Text("%s", meta.name.c_str());
        ImGui::NextColumn();

        ImGui::Text("0x%08x", meta.name.c_str());
        ImGui::NextColumn();

        ImGui::Text("%d", sizeof(value));
        ImGui::NextColumn();

        std::string title{"##"};
        title.append(meta.name);

        return title;
    }

    void layout_columns();
};

template <>
pupene::PupPolicy EditorPupper::begin_impl(Color& value,
                                           const pupene::Meta& meta);

template <>
pupene::PupPolicy EditorPupper::begin_impl(vec2f& value,
                                           const pupene::Meta& meta);


#pragma clang diagnostic pop
