#pragma once

#include <pupene/pupene.h>
#include <imgui/imgui.h>
#include <limits>
#include <memory>
#include "puppers.h"
#include "traits.h"


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat"

template<typename, typename>
struct input_adapter;

struct bindable {
    virtual ~bindable() = default;
    virtual void update_proxy() = 0;
    virtual void update_value() = 0;
    virtual std::unique_ptr<bindable> copy() const = 0;

    virtual void* ptr() = 0; // TODO: variant?
};

class wrapper {
public:
    wrapper(std::unique_ptr<bindable> bound) : self(std::move(bound)) {}
    wrapper(const wrapper& w) : self(w.self->copy()) {}
    wrapper(wrapper&&) noexcept = default;

    template <typename Proxy, typename Source>
    static std::unique_ptr<bindable> make(Source& s) {
        return std::make_unique<input_adapter<Source, Proxy>>(s);
    }

    wrapper& operator=(const wrapper& w) {
        *this = wrapper{w};
        return *this;
    }

    wrapper& operator=(wrapper&& w) noexcept = default;

    void* ptr() {
        return self->ptr();
    }

    void update() {
        self->update_value();
    }

private:

    template<typename Source, typename Proxy>
    struct input_adapter : bindable {
        input_adapter(Source& t) : actual(t) {
            update_proxy();
        }

        ~input_adapter() {
            update_value();
        }

        void update_proxy() override {
            proxy = static_cast<Proxy>(actual);
        }

        void update_value() override {
            if (std::numeric_limits<Source>::min() > proxy) {
                actual = std::numeric_limits<Source>::min();
            } else if (std::numeric_limits<Source>::max() < proxy) {
                actual = std::numeric_limits<Source>::max();
            } else {
                actual = static_cast<Source>(proxy);
            }
        }

        std::unique_ptr<bindable> copy() const override {
            return std::make_unique<input_adapter<Source, Proxy>>(*this);
        }

        void* ptr() override {
            return &proxy;
        }

        Source& actual;
        Proxy proxy;
    };

    std::unique_ptr<bindable> self;
};

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

    template <typename T, typename Fn>
    void to_widget(T& value,
                   const Meta& meta,
                   Fn&& wpup) {

        ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
        std::string label = prepare(value, meta);
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
