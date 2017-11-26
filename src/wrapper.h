#pragma once

#include <limits>
#include <memory>
#include "pup-fns.h"

template<typename, typename>
struct input_adapter;

class wrapper {
public:
    struct bindable {
        virtual ~bindable() = default;
        virtual void update_proxy() = 0;
        virtual void update_value() = 0;
        virtual std::unique_ptr<bindable> copy() const = 0;
        virtual void* ptr() = 0; // TODO: variant?
    };

    wrapper(std::unique_ptr<bindable> bound) : self(std::move(bound)) {}
    wrapper(const wrapper& w) : self(w.self->copy()) {}
    wrapper(wrapper&&) noexcept = default;

    template <typename Proxy, typename Source>
    static std::unique_ptr<bindable> make(Source& s) {
        return std::make_unique<input_adapter<Source, Proxy>>(s);
    }

    wrapper& operator=(const wrapper& w) {
        return *this = wrapper{w};
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
        explicit input_adapter(Source& t) : actual(t) {
            update_proxy();
        }

        ~input_adapter() final {
            update_value();
        }

        void update_proxy() override {
            proxy = static_cast<Proxy>(actual);
        }

        void update_value() override {
            constexpr auto min = std::numeric_limits<Source>::min();
            constexpr auto max = std::numeric_limits<Source>::max();

            if (min > proxy) {
                actual = min;
            } else if (max < proxy) {
                actual = max;
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


