#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "global_macros.h"


#include <string>
#include <type_traits>
#include <stdexcept>
#include <memory>
#include <string_view>
#include <chrono>

namespace cru
{
    template<typename T> struct is_shared_ptr : std::false_type {};
    template<typename T> struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};
    template<typename T> constexpr bool is_shared_ptr_v = is_shared_ptr<T>::value;

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
}
