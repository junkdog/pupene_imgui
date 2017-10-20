#pragma once

#include <type_traits>

template<typename T>
using enable_if_decimal = std::enable_if_t<std::is_floating_point<T>{}>;

template <typename T>
using enable_if_integer = std::enable_if_t<std::is_integral<T>{}>;