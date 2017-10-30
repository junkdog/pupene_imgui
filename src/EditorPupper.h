#pragma once

#include <algorithm>
#include <pupene/pupene.h>
#include <imgui/imgui.h>
#include <memory>
#include "traits.h"
#include "wrapper.h"
#include "EditorConfig.h"


namespace color {
    extern ImVec4 header;
    extern ImVec4 begin;
    extern ImVec4 object;
}


class EditorPupper : public pupene::Pupper<EditorPupper> {
public:
    using Meta = pupene::Meta;
    using PupPolicy = pupene::PupPolicy;

    explicit EditorPupper(EditorConfig& config) : config(config) {
        open_window();
    }

    ~EditorPupper() override = default;

    template <typename T>
    PupPolicy begin_impl(T& value, const Meta& meta) {
        if (is_filtered(meta)) {
            parents.push_back(meta);
            return PupPolicy::pup_object;
        }

        ImGui::PushID(&value);
        ImGui::PushStyleColor(ImGuiCol_Text, color::begin);
        prepare(value, meta);
        ImGui::NextColumn();
        ImGui::PopStyleColor();

        depth++;

        return PupPolicy::pup_object;
    }

    void end_impl(const Meta& meta) {
        if (is_filtered(meta)) {
            if (!parents.empty() && meta.name == parents.back().name) {
                depth--;
                parents.pop_back();
            }
        } else {
            depth--;
            ImGui::PopID();
        }
    }

    template <typename T,
              typename = enable_if_decimal<T>>
    void pup_impl(T& value, const Meta& meta) {
        pup_to_widget<float>(value, meta, [](auto label, auto ptr) {
            ImGui::DragFloat(
                label,
                ptr,
                .01f,
                std::numeric_limits<T>::min(),
                std::numeric_limits<T>::max(),
                "%.3f");
        });
    }

    template <typename T,
              typename = void,
              typename = enable_if_integer<T>>
    void pup_impl(T& value, const Meta& meta) {
        pup_to_widget<int>(value, meta, [](auto label, auto ptr) {
            constexpr int min = (sizeof(T) >= 4)
                ? std::numeric_limits<int>::min()
                : std::numeric_limits<T>::min();
            constexpr int max = (sizeof(T) >= 4)
                ? std::numeric_limits<int>::max()
                : std::numeric_limits<T>::max();

            ImGui::DragInt(label, ptr, 1.f, min, max);
        });
    }

    void pup_impl(bool& b, const Meta& meta) {
        auto& color = ImGui::GetStyleColorVec4(ImGuiCol_Text);
        to_widget(b, meta, color, [&b](auto label) {
            ImGui::Checkbox(label, &b);
        });
    }

    void pup_impl(std::string& s, const Meta& meta) {
        auto& color = ImGui::GetStyleColorVec4(ImGuiCol_Text);
        to_widget(s, meta, color, [&s](auto label) {
            // currently no clean way to to dynamically grow backing
            // arrays without fiddling around with internals.
            // refer to https://github.com/ocornut/imgui/issues/1008
            static const int MAX_STRING_CHARS = 50;
            if (s.capacity() < MAX_STRING_CHARS) {
                s.reserve(MAX_STRING_CHARS);
            }

            ImGuiTextEditCallback on_edit = [](auto data) -> int {
                auto& s = *static_cast<std::string*>(data->UserData);
                s = data->Buf;
                return 0;
            };

            ImGui::InputText(label,
                             &s[0],
                             s.capacity(),
                             ImGuiInputTextFlags_CallbackAlways,
                             on_edit,
                             &s);
        });
    }

    void flush() {
        ImGui::End();
    }

private:
    std::vector<wrapper> bindings;
    std::vector<Meta> parents;
    int depth = -1;
    EditorConfig& config;

    bool is_filtered(const Meta& meta);

    template <typename T, typename Fn>
    void to_widget(T& value,
                   const Meta& meta,
                   const ImVec4& color,
                   Fn&& wpup) {

        if (is_filtered(meta))
            return;

        int restore_depth = depth; // FIXME: must save with meta/parents
        if (config.filter.show_parents && !parents.empty()) {
            ImGui::PushStyleColor(ImGuiCol_Text, color::begin);
            for (auto& parent : parents) {
                write_label(parent);
                ImGui::NextColumn();
                ImGui::NextColumn();
                ImGui::NextColumn();

                depth++;
            }
            ImGui::PopStyleColor();

            parents.clear();
        }

        ImGui::PushStyleColor(ImGuiCol_Text, color);
        std::string label = prepare(value, meta);
        ImGui::PopStyleColor();

        ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
        wpup(label.c_str());
        ImGui::PopItemWidth();
        ImGui::NextColumn();

        depth = restore_depth;
    }

    template <typename PtrType, typename T, typename Fn>
    void pup_to_widget(T&& value,
                       const Meta& meta,
                       Fn&& wpup) {

        auto& color = ImGui::GetStyleColorVec4(ImGuiCol_Text);
        to_widget(value,
                  meta,
                  color,
                  [&b = bindings, &wpup, &value](auto label) {
                      b.emplace_back(wrapper::make<PtrType>(value));
                      wpup(label, static_cast<PtrType*>(b.back().ptr()));
                  });
    }

    template <typename T, typename Fn>
    void object_to_widget(T& value,
                          const Meta& meta,
                          Fn&& wpup) {

        if (is_filtered(meta))
            return;

        to_widget(value, meta, color::object, [&wpup, &value](auto label) {
            ImGui::PushID(&value);
            wpup(label);
        });

        depth++;
    }

    void open_window();

    template <typename T>
    std::string prepare(T& value, const Meta& meta) {
        // FIXME: indent not working with columns (i think)
        write_label(meta);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat"
        ImGui::Text("0x%08x", &value);
        ImGui::NextColumn();
#pragma clang diagnostic pop

        ImGui::Text("%4d", static_cast<int>(sizeof(value)));
        ImGui::NextColumn();

        std::string title{"###"};
        title.append(meta.name);
        title.append(std::to_string(reinterpret_cast<uint_fast64_t>(&value)));

        return title;
    }

    void write_label(const Meta& meta) const {
        auto indent = std::max(0, depth) * 2;
        std::__cxx11::string s(indent, ' ');
        ImGui::Text("%s%s", s.c_str(), meta.name.c_str());
        ImGui::NextColumn();
    }

    void layout_columns();
};

template <>
pupene::PupPolicy EditorPupper::begin_impl(Color& value,
                                           const pupene::Meta& meta);

template <>
pupene::PupPolicy EditorPupper::begin_impl(vec2f& value,
                                           const pupene::Meta& meta);

