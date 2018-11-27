#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "pre.hpp"

// ReSharper disable once CppUnusedIncludeDirective
#include <type_traits>
#include <optional>

namespace cru
{
        template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    float Coerce(const T n, const std::optional<T> min, const std::optional<T> max)
    {
        if (min.has_value() && n < min.value())
            return min.value();
        if (max.has_value() && n > max.value())
            return max.value();
        return n;
    }
    
    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    float Coerce(const T n, const T min, const T max)
    {
        if (n < min)
            return min;
        if (n > max)
            return max;
        return n;
    }

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    float Coerce(const T n, const std::nullopt_t, const std::optional<T> max)
    {
        if (max.has_value() && n > max.value())
            return max.value();
        return n;
    }

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    float Coerce(const T n, const std::optional<T> min, const std::nullopt_t)
    {
        if (min.has_value() && n < min.value())
            return min.value();
        return n;
    }
    
    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    float Coerce(const T n, const std::nullopt_t, const T max)
    {
        if (n > max)
            return max;
        return n;
    }

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    float Coerce(const T n, const T min, const std::nullopt_t)
    {
        if (n < min)
            return min;
        return n;
    }

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    T AtLeast0(const T value)
    {
        return value < static_cast<T>(0) ? static_cast<T>(0) : value;
    }
}
