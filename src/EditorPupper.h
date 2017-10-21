#pragma once

#include <algorithm>
#include <pupene/pupene.h>
#include <imgui/imgui.h>
#include <memory>
#include "puppers.h"
#include "traits.h"
#include "wrapper.h"


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

        depth++;

        return PupPolicy::pup_object;
    }

    void end_impl() {
        depth--;
        ImGui::PopID();
    }

    template <typename T,
              typename = enable_if_decimal<T>>
    void pup_impl(T& value, const Meta& meta) {
        pup_to_widget<float>(value, meta, [](auto label, auto ptr) {
            ImGui::DragFloat(label, ptr, 0, 100);
        });
    }

    template <typename T,
              typename = void,
              typename = enable_if_integer<T>>
    void pup_impl(T& value, const Meta& meta) {
        pup_to_widget<int>(value, meta, [](auto label, auto ptr) {
            ImGui::DragInt(label, ptr, 0, 100);
        });
    }

    void pup_impl(std::string& value, const Meta& meta) {
        to_widget(value, meta, [&value](auto label) {
            ImGui::InputText(label, &value[0], value.capacity());
        });
    }

    void flush() {
        ImGui::End();
    }

private:
    std::vector<wrapper> bindings;
    int depth = -1;

    template <typename T, typename Fn>
    void to_widget(T& value,
                   const Meta& meta,
                   Fn&& wpup) {

        std::string label = prepare(value, meta);

        ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
        wpup(label.c_str());
        ImGui::PopItemWidth();
        ImGui::NextColumn();
    }

    template <typename PtrType, typename T, typename Fn>
    void pup_to_widget(T&& value,
                       const Meta& meta,
                       Fn&& wpup) {

        to_widget(value,
                  meta,
                  [&b = bindings, &wpup, &value](auto label) {
                      b.emplace_back(wrapper::make<PtrType>(value));
                      wpup(label, static_cast<PtrType*>(b.back().ptr()));
                  });
    }

    template <typename T, typename Fn>
    void object_to_widget(T& value,
                          const Meta& meta,
                          Fn&& wpup) {

        to_widget(value, meta, [&wpup, &value](auto label) {
            ImGui::PushID(&value);
            wpup(label);
        });
        depth++;
    }

    void open_window(const std::string& title);

    template <typename T>
    std::string prepare(T& value, const Meta& meta) {
        // FIXME: indent not working with columns (i think)
        auto indent = std::max(0, depth) * 2;
        std::string s(indent, ' ');
        ImGui::Text("%s%s", s.c_str(), meta.name.c_str());
        ImGui::NextColumn();

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat"
        ImGui::Text("0x%08x", &value);
        ImGui::NextColumn();
#pragma clang diagnostic pop

        ImGui::Text("%4d", static_cast<int>(sizeof(value)));
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


