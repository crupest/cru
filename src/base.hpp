#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "global_macros.hpp"


#include <string>
#include <stdexcept>
#include <string_view>
#include <chrono>
#include <optional>
// ReSharper disable once CppUnusedIncludeDirective
#include <type_traits>

namespace cru
{
    template<typename T> struct type_tag {};

    //typedefs
    using String = std::wstring;
    using MultiByteString = std::string;

    using StringView = std::wstring_view;
    using MultiByteStringView = std::string_view;

    using FloatSecond = std::chrono::duration<double, std::chrono::seconds::period>;

    enum class FlowControl
    {
        Continue,
        Break
    };


    class Object
    {
    public:
        Object() = default;
        Object(const Object&) = default;
        Object& operator = (const Object&) = default;
        Object(Object&&) = default;
        Object& operator = (Object&&) = default;
        virtual ~Object() = default;
    };

    struct Interface
    {
        virtual ~Interface() = default;
    };

    [[noreturn]] inline void UnreachableCode()
    {
        throw std::logic_error("Unreachable code.");
    }

    MultiByteString ToUtf8String(const StringView& string);

    inline void Require(const bool condition, const MultiByteStringView& error_message)
    {
        if (!condition)
            throw std::invalid_argument(error_message.data());
    }

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

}
