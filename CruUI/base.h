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

#include <stdexcept>


namespace cru
{
    enum class FlowControl
    {
        Continue,
        Break
    };

#ifdef CRU_DEBUG
    using String = std::wstring;
#else
	using String = folly::basic_fbstring<wchar_t>;
#endif

    template<typename FunctionType>
    using Function = folly::Function<FunctionType>;

    template<typename... Args>
    using Action = Function<void(Args...)>;

    template<typename... Args>
    using FlowControlAction = Function<FlowControl(Args...)>;

#ifdef CRU_DEBUG
    template<typename T>
    using Vector = std::vector<T>;
#else
    template<typename T>
    using Vector = folly::fbvector<T>;
#endif

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
}
