#pragma once

#include <pupene/pupene.h>
#include <imgui/imgui.h>
#include <bits/unique_ptr.h>
#include "puppers.h"
#include "traits.h"


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat"

template<typename T, typename U>
struct input_adapter;

struct bindable {
    virtual ~bindable() = default;
    virtual void update_proxy() = 0;
    virtual void update_value() = 0;
    virtual std::unique_ptr<bindable> copy() const = 0;

    virtual int* int_ptr() = 0;
};

class wrapper {
public:
    wrapper(std::unique_ptr<bindable> bound) : self(std::move(bound)) {}
    wrapper(const wrapper& w) : self(w.self->copy()) {}
    wrapper(wrapper&&) noexcept = default;

//    template <typename Source, typename Proxy>
//    wrapper make(Source& s) : self(std::move<input_adapter<Source, Proxy>>(s)) {

    template <typename Source, typename Proxy>
    static std::unique_ptr<bindable> make(Source& s) {
        return std::make_unique<input_adapter<Source, Proxy>>(s);
    }

    wrapper& operator=(const wrapper& w) {
        *this = wrapper{w};
        return *this;
    }

    wrapper& operator=(wrapper&& w) noexcept = default;

    int* int_ptr() {
        return self->int_ptr();
    }

    void update() {
        self->update_value();
    }

private:

    template<typename T, typename U>
    struct input_adapter : bindable {
        input_adapter(T& t) : actual(t) {
            update_proxy();
        }

        ~input_adapter() {
            update_value();
        }

        void update_proxy() override {
            massaged = static_cast<U>(actual);
        }

        void update_value() override {
            T t = static_cast<T>(massaged);
            actual = t;
        }

        std::unique_ptr<bindable> copy() const override {
            return std::make_unique<input_adapter<T, U>>(*this);
        }

        int* int_ptr() override {
            return &massaged;
        }

        T& actual;
        U massaged;
    };

    std::unique_ptr<bindable> self;
//    void update() {
//        T t = static_cast<T>(massaged);
//        actual = t;
//    }

};
//
//template <typename T>
//using integer_input =  input_adapter<T, int>();
//
//template <typename T>
//using float_input =  input_adapter<T, float>();

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
//        auto foo = wrapper::make<T, int>(value);
        auto& b = bindings;
        pup_to_widget(value, meta, [&b, &value](auto label) {
            b.emplace_back(wrapper::make<T, int>(value));
//            bindings.emplace_back(wrapper<int>(value));
//            bindings.emplace_back(wrapper::make<T, int>(value));
//            auto& bound = bindings.back();
            ImGui::DragInt(label, b.back().int_ptr(), 0, 100);
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
    std::vector<wrapper> bindings;

    template <typename T, typename Fn>
    void to_widget(T& value,
                   const Meta& meta,
                   bool push_id,
                   Fn&& wpup) {

        std::string label = prepare(value, meta);
        if (push_id)
            ImGui::PushID(&value);

        ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
        wpup(label.c_str());
        ImGui::PopItemWidth();

        ImGui::NextColumn();
    }

    template <typename T, typename Fn>
    void pup_to_widget(T& value,
                       const Meta& meta,
                       Fn&& wpup) {

        to_widget(value, meta, false, wpup);
    }

    template <typename T, typename Fn>
    void object_to_widget(T& value,
                          const Meta& meta,
                          Fn&& wpup) {

        to_widget(value, meta, true, wpup);
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
