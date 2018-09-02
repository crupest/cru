#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "global_macros.h"


#include <folly/String.h>
#include <folly/Function.h>

#include <stdexcept>


namespace cru
{
    enum class FlowControl
    {
        Continue,
        Break
    };

	using String = folly::basic_fbstring<wchar_t>;

    template<typename FunctionType>
    using Function = folly::Function<FunctionType>;

    template<typename... Args>
    using Action = Function<void(Args...)>;

    template<typename... Args>
    using FlowControlAction = Function<FlowControl(Args...)>;

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
