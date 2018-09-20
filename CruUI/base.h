#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "global_macros.h"


#ifdef CRU_DEBUG
#include <string>
#include <vector>
#else
#include <folly/String.h>
#include <folly/FBVector.h>
#endif

#include <folly/Function.h>

#include <utility>
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

    enum class FlowControl
    {
        Continue,
        Break
    };

#ifdef CRU_DEBUG
    using String = std::wstring;
    using MultiByteString = std::string;
#else
    using String = folly::basic_fbstring<wchar_t>;
    using MultiByteString = folly::fbstring;
#endif

    using StringView = std::wstring_view;
    using MultiByteStringView = std::string_view;

    template<typename FunctionType>
    using Function = folly::Function<FunctionType>;

    template<typename FunctionType>
    using FunctionPtr = std::shared_ptr<Function<FunctionType>>;

    using Action = Function<void()>;
    using ActionPtr = FunctionPtr<void()>;

    template<typename Type, typename... Args>
    Type CreatePtr(Args&&... args)
    {
        static_assert(is_shared_ptr_v<Type>);
        return std::make_shared<typename Type::element_type>(std::forward<Args>(args)...);
    }

    inline ActionPtr CreateActionPtr(Action&& action)
    {
        return std::make_shared<Action>(std::move(action));
    }

#ifdef CRU_DEBUG
    template<typename T>
    using Vector = std::vector<T>;
#else
    template<typename T>
    using Vector = folly::fbvector<T>;
#endif

    using FloatSecond = std::chrono::duration<double, std::chrono::seconds::period>;

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

    struct ICancelable : virtual Interface
    {
        virtual void Cancel() = 0;
    };

    using CancelablePtr = std::shared_ptr<ICancelable>;

    MultiByteString ToUtf8String(const StringView& string);
}
