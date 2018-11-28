#pragma once
//--------------------------------------------------------
//-------begin of file: src\any_map.hpp
//--------------------------------------------------------

// ReSharper disable once CppUnusedIncludeDirective
//--------------------------------------------------------
//-------begin of file: src\pre.hpp
//--------------------------------------------------------

#ifdef _DEBUG
#define CRU_DEBUG
#endif

#ifdef CRU_DEBUG
#define CRU_DEBUG_LAYOUT
#endif


#ifdef CRU_DEBUG
// ReSharper disable once IdentifierTypo
// ReSharper disable once CppInconsistentNaming
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#endif
//--------------------------------------------------------
//-------end of file: src\pre.hpp
//--------------------------------------------------------

#include <any>
#include <unordered_map>
#include <functional>
#include <optional>
#include <typeinfo>

//--------------------------------------------------------
//-------begin of file: src\base.hpp
//--------------------------------------------------------

// ReSharper disable once CppUnusedIncludeDirective

#include <string>
#include <stdexcept>
#include <string_view>
#include <chrono>

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
}
//--------------------------------------------------------
//-------end of file: src\base.hpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\format.hpp
//--------------------------------------------------------

// ReSharper disable once CppUnusedIncludeDirective


namespace cru
{
    namespace details
    {
        constexpr StringView PlaceHolder(type_tag<String>)
        {
            return StringView(L"{}");
        }

        constexpr MultiByteStringView PlaceHolder(type_tag<MultiByteString>)
        {
            return MultiByteStringView("{}");
        }

        template<typename TString>
        void FormatInternal(TString& string)
        {
            const auto find_result = string.find(PlaceHolder(type_tag<TString>{}));
            if (find_result != TString::npos)
                throw std::invalid_argument("There is more placeholders than args.");
        }

        template<typename TString, typename T, typename... TRest>
        void FormatInternal(TString& string, const T& arg, const TRest&... args)
        {
            const auto find_result = string.find(PlaceHolder(type_tag<TString>{}));
            if (find_result == TString::npos)
                throw std::invalid_argument("There is less placeholders than args.");

            string.replace(find_result, 2, FormatToString(arg, type_tag<TString>{}));
            FormatInternal<TString>(string, args...);
        }
    }

    template<typename... T>
    String Format(const StringView& format, const T&... args)
    {
        String result(format);
        details::FormatInternal<String>(result, args...);
        return result;
    }

    template<typename... T>
    MultiByteString Format(const MultiByteStringView& format, const T&... args)
    {
        MultiByteString result(format);
        details::FormatInternal<MultiByteString>(result, args...);
        return result;
    }

#define CRU_FORMAT_NUMBER(type) \
    inline String FormatToString(const type number, type_tag<String>) \
    { \
        return std::to_wstring(number); \
    } \
    inline MultiByteString FormatToString(const type number, type_tag<MultiByteString>) \
    { \
        return std::to_string(number); \
    }

    CRU_FORMAT_NUMBER(int)
    CRU_FORMAT_NUMBER(short)
    CRU_FORMAT_NUMBER(long)
    CRU_FORMAT_NUMBER(long long)
    CRU_FORMAT_NUMBER(unsigned int)
    CRU_FORMAT_NUMBER(unsigned short)
    CRU_FORMAT_NUMBER(unsigned long)
    CRU_FORMAT_NUMBER(unsigned long long)
    CRU_FORMAT_NUMBER(float)
    CRU_FORMAT_NUMBER(double)

#undef CRU_FORMAT_NUMBER

    inline StringView FormatToString(const String& string, type_tag<String>)
    {
        return string;
    }

    inline MultiByteString FormatToString(const MultiByteString& string, type_tag<MultiByteString>)
    {
        return string;
    }

    inline StringView FormatToString(const StringView& string, type_tag<String>)
    {
        return string;
    }

    inline MultiByteStringView FormatToString(const MultiByteStringView& string, type_tag<MultiByteString>)
    {
        return string;
    }

    inline StringView FormatToString(const wchar_t* string, type_tag<String>)
    {
        return StringView(string);
    }

    inline MultiByteStringView FormatToString(const char* string, type_tag<MultiByteString>)
    {
        return MultiByteString(string);
    }
}
//--------------------------------------------------------
//-------end of file: src\format.hpp
//--------------------------------------------------------


namespace cru
{
    // A map with String as key and any type as value.
    // It also has notification when value with specified key changed.
    class AnyMap : public Object
    {
    public:
        using ListenerToken = long;
        using Listener = std::function<void(const std::any&)>;

        AnyMap() = default;
        AnyMap(const AnyMap& other) = delete;
        AnyMap(AnyMap&& other) = delete;
        AnyMap& operator=(const AnyMap& other) = delete;
        AnyMap& operator=(AnyMap&& other) = delete;
        ~AnyMap() override = default;


        // return the value if the value exists and the type of value is T.
        // return a null optional if value doesn't exists.
        // throw std::runtime_error if type is mismatch.
        template <typename T>
        std::optional<T> GetOptionalValue(const String& key) const
        {
            try
            {
                const auto find_result = map_.find(key);
                if (find_result != map_.cend())
                {
                    const auto& value = find_result->second.first;
                    if (value.has_value())
                        return std::any_cast<T>(value);
                    return std::nullopt;
                }
                return std::nullopt;
            }
            catch (const std::bad_any_cast&)
            {
                throw std::runtime_error(Format("Value of key \"{}\" in AnyMap is not of the type {}.", ToUtf8String(key), typeid(T).name()));
            }
        }

        // return the value if the value exists and the type of value is T.
        // throw if value doesn't exists. (different from "GetOptionalValue").
        // throw std::runtime_error if type is mismatch.
        template <typename T>
        T GetValue(const String& key) const
        {
            const auto optional_value = GetOptionalValue<T>(key);
            if (optional_value.has_value())
                return optional_value.value();
            else
                throw std::runtime_error(Format("Key \"{}\" does not exists in AnyMap.", ToUtf8String(key)));
        }

        // Set the value of key, and trigger all related listeners.
        template <typename T>
        void SetValue(const String& key, T&& value)
        {
            auto& p = map_[key];
            p.first = std::make_any<T>(std::forward<T>(value));
            InvokeListeners(p.second, p.first);
        }

        // Remove the value of the key.
        void ClearValue(const String& key)
        {
            auto& p = map_[key];
            p.first = std::any{};
            InvokeListeners(p.second, std::any{});
        }

        // Add a listener which is called when value of key is changed.
        // Return a token used to remove the listener.
        ListenerToken RegisterValueChangeListener(const String& key, const Listener& listener);

        // Remove a listener by token.
        void UnregisterValueChangeListener(ListenerToken token);

    private:
        void InvokeListeners(std::list<ListenerToken>& listener_list, const std::any& value);

    private:
        std::unordered_map<String, std::pair<std::any, std::list<ListenerToken>>> map_{};
        std::unordered_map<ListenerToken, Listener> listeners_{};
        ListenerToken current_listener_token_ = 0;
    };
}
//--------------------------------------------------------
//-------end of file: src\any_map.hpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\application.hpp
//--------------------------------------------------------

// ReSharper disable once CppUnusedIncludeDirective

//--------------------------------------------------------
//-------begin of file: src\system_headers.hpp
//--------------------------------------------------------

// ReSharper disable once CppUnusedIncludeDirective

//include system headers

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>

#pragma comment(lib, "D3D11.lib")
#include <d3d11.h>

#pragma comment(lib, "D2d1.lib")
#include <d2d1_1.h>

#pragma comment(lib, "DWrite.lib")
#include <dwrite.h>

#include <dxgi1_2.h>
#include <wrl/client.h>

#include <VersionHelpers.h>
//--------------------------------------------------------
//-------end of file: src\system_headers.hpp
//--------------------------------------------------------
#include <memory>
#include <optional>
#include <functional>
#include <typeindex>
#include <type_traits>


#ifdef CRU_DEBUG
#include <unordered_set>
#endif

namespace cru 
{
    class Application;

    namespace ui
    {
        class WindowClass;
    }


    class GodWindow : public Object
    {
    public:
        explicit GodWindow(Application* application);
        GodWindow(const GodWindow& other) = delete;
        GodWindow(GodWindow&& other) = delete;
        GodWindow& operator=(const GodWindow& other) = delete;
        GodWindow& operator=(GodWindow&& other) = delete;
        ~GodWindow() override;

        HWND GetHandle() const
        {
            return hwnd_;
        }

        std::optional<LRESULT> HandleGodWindowMessage(HWND hwnd, int msg, WPARAM w_param, LPARAM l_param);

    private:
        std::unique_ptr<ui::WindowClass> god_window_class_;
        HWND hwnd_;
    };

    class Application : public Object
    {
    public:
        static Application* GetInstance();
    private:
        static Application* instance_;

    public:
        explicit Application(HINSTANCE h_instance);
        Application(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator = (const Application&) = delete;
        Application& operator = (Application&&) = delete;
        ~Application() override;

    public:
        int Run();
        void Quit(int quit_code);


        HINSTANCE GetInstanceHandle() const
        {
            return h_instance_;
        }

        GodWindow* GetGodWindow() const
        {
            return god_window_.get();
        }

        // Resolve a singleton.
        // All singletons will be delete in reverse order of resolve.
        template<typename T, typename = std::enable_if_t<std::is_base_of_v<Object, T>>>
        T* ResolveSingleton(const std::function<T*(Application*)>& creator)
        {
            const auto& index = std::type_index{typeid(T)};
            const auto find_result = singleton_map_.find(index);
            if (find_result != singleton_map_.cend())
                return static_cast<T*>(find_result->second);

#ifdef CRU_DEBUG
            const auto type_find_result = singleton_type_set_.find(index);
            if (type_find_result != singleton_type_set_.cend())
                throw std::logic_error("The singleton of that type is being constructed. This may cause a dead recursion.");
            singleton_type_set_.insert(index);
#endif

            auto singleton = creator(this);
            singleton_map_.emplace(index, static_cast<Object*>(singleton));
            singleton_list_.push_back(singleton);
            return singleton;
        }

    private:
        HINSTANCE h_instance_;

        std::unique_ptr<GodWindow> god_window_;

        std::unordered_map<std::type_index, Object*> singleton_map_;
        std::list<Object*> singleton_list_; // used for reverse destroy.
#ifdef CRU_DEBUG
        std::unordered_set<std::type_index> singleton_type_set_; // used for dead recursion.
#endif
    };


    void InvokeLater(const std::function<void()>& action);
}
//--------------------------------------------------------
//-------end of file: src\application.hpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\exception.hpp
//--------------------------------------------------------

// ReSharper disable once CppUnusedIncludeDirective

#include <optional>



namespace cru {
    class HResultError : public std::runtime_error
    {
    public:
        explicit HResultError(HRESULT h_result, std::optional<MultiByteStringView> additional_message = std::nullopt);
        HResultError(const HResultError& other) = default;
        HResultError(HResultError&& other) = default;
        HResultError& operator=(const HResultError& other) = default;
        HResultError& operator=(HResultError&& other) = default;
        ~HResultError() override = default;

        HRESULT GetHResult() const
        {
            return h_result_;
        }

    private:
        HRESULT h_result_;
    };

    inline void ThrowIfFailed(const HRESULT h_result) {
        if (FAILED(h_result))
            throw HResultError(h_result);
    }

    inline void ThrowIfFailed(const HRESULT h_result, const MultiByteStringView& message) {
        if (FAILED(h_result))
            throw HResultError(h_result, message);
    }

    class Win32Error : public std::runtime_error
    {
    public:
        explicit Win32Error(DWORD error_code, std::optional<MultiByteStringView> additional_message = std::nullopt);
        Win32Error(const Win32Error& other) = default;
        Win32Error(Win32Error&& other) = default;
        Win32Error& operator=(const Win32Error& other) = default;
        Win32Error& operator=(Win32Error&& other) = default;
        ~Win32Error() override = default;

        HRESULT GetErrorCode() const
        {
            return error_code_;
        }

    private:
        DWORD error_code_;
    };
}
//--------------------------------------------------------
//-------end of file: src\exception.hpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\timer.hpp
//--------------------------------------------------------

// ReSharper disable once CppUnusedIncludeDirective

#include <map>
#include <chrono>
#include <functional>
#include <optional>


namespace cru
{
    using TimerAction = std::function<void()>;

    class TimerManager : public Object
    {
    public:
        static TimerManager* GetInstance();

    private:
        TimerManager() = default;
    public:
        TimerManager(const TimerManager& other) = delete;
        TimerManager(TimerManager&& other) = delete;
        TimerManager& operator=(const TimerManager& other) = delete;
        TimerManager& operator=(TimerManager&& other) = delete;
        ~TimerManager() override = default;

        UINT_PTR CreateTimer(UINT milliseconds, bool loop, const TimerAction& action);
        void KillTimer(UINT_PTR id);
        std::optional<std::pair<bool, TimerAction>> GetAction(UINT_PTR id);

    private:
        std::map<UINT_PTR, std::pair<bool, TimerAction>> map_{};
        UINT_PTR current_count_ = 0;
    };

    class TimerTask
    {
        friend TimerTask SetTimeout(std::chrono::milliseconds milliseconds, const TimerAction& action);
        friend TimerTask SetInterval(std::chrono::milliseconds milliseconds, const TimerAction& action);

    private:
        explicit TimerTask(UINT_PTR id);

    public:
        TimerTask(const TimerTask& other) = default;
        TimerTask(TimerTask&& other) = default;
        TimerTask& operator=(const TimerTask& other) = default;
        TimerTask& operator=(TimerTask&& other) = default;
        ~TimerTask() = default;

        void Cancel() const;

    private:
        UINT_PTR id_;
    };

    TimerTask SetTimeout(std::chrono::milliseconds milliseconds, const TimerAction& action);
    TimerTask SetInterval(std::chrono::milliseconds milliseconds, const TimerAction& action);
}
//--------------------------------------------------------
//-------end of file: src\timer.hpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\window.hpp
//--------------------------------------------------------

// ReSharper disable once CppUnusedIncludeDirective

#include <list>
#include <map>
#include <memory>

//--------------------------------------------------------
//-------begin of file: src\ui\control.hpp
//--------------------------------------------------------

// ReSharper disable once CppUnusedIncludeDirective

#include <unordered_map>
#include <any>
#include <utility>

//--------------------------------------------------------
//-------begin of file: src\ui\ui_base.hpp
//--------------------------------------------------------

// ReSharper disable once CppUnusedIncludeDirective

#include <optional>


namespace cru::ui
{
    struct Point
    {
        constexpr static Point Zero()
        {
            return Point(0, 0);
        }

        constexpr Point() = default;
        constexpr Point(const float x, const float y) : x(x), y(y) { }

        float x = 0;
        float y = 0;
    };

    constexpr bool operator==(const Point& left, const Point& right)
    {
        return left.x == right.x && left.y == right.y;
    }

    constexpr bool operator!=(const Point& left, const Point& right)
    {
        return !(left == right);
    }

    struct Size
    {
        constexpr static Size Zero()
        {
            return Size(0, 0);
        }

        constexpr Size() = default;
        constexpr Size(const float width, const float height) : width(width), height(height) { }

        float width = 0;
        float height = 0;
    };

    constexpr Size operator + (const Size& left, const Size& right)
    {
        return Size(left.width + right.width, left.height + right.height);
    }

    constexpr Size operator - (const Size& left, const Size& right)
    {
        return Size(left.width - right.width, left.height - right.height);
    }

    constexpr bool operator==(const Size& left, const Size& right)
    {
        return left.width == right.width && left.height == right.height;
    }

    constexpr bool operator!=(const Size& left, const Size& right)
    {
        return !(left == right);
    }

    struct Thickness
    {
        constexpr static Thickness Zero()
        {
            return Thickness(0);
        }

        constexpr Thickness() : Thickness(0) { }

        constexpr explicit Thickness(const float width)
            : left(width), top(width), right(width), bottom(width) { }

        constexpr explicit Thickness(const float horizontal, const float vertical)
            : left(horizontal), top(vertical), right(horizontal), bottom(vertical) { }

        constexpr Thickness(const float left, const float top, const float right, const float bottom)
            : left(left), top(top), right(right), bottom(bottom) { }

        float GetHorizontalTotal() const
        {
            return left + right;
        }

        float GetVerticalTotal() const
        {
            return top + bottom;
        }

        void SetLeftRight(const float value)
        {
            left = right = value;
        }

        void SetTopBottom(const float value)
        {
            top = bottom = value;
        }

        void SetAll(const float value)
        {
            left = top = right = bottom = value;
        }

        float Validate() const
        {
            return left >= 0.0 && top >= 0.0 && right >= 0.0 && bottom >= 0.0;
        }

        float left;
        float top;
        float right;
        float bottom;
    };

    struct Rect
    {
        constexpr Rect() = default;
        constexpr Rect(const float left, const float top, const float width, const float height)
            : left(left), top(top), width(width), height(height) { }
        constexpr Rect(const Point& lefttop, const Size& size)
            : left(lefttop.x), top(lefttop.y), width(size.width), height(size.height) { }

        constexpr static Rect FromVertices(const float left, const float top, const float right, const float bottom)
        {
            return Rect(left, top, right - left, bottom - top);
        }

        constexpr float GetRight() const
        {
            return left + width;
        }

        constexpr float GetBottom() const
        {
            return top + height;
        }

        constexpr Point GetLeftTop() const
        {
            return Point(left, top);
        }

        constexpr Point GetRightBottom() const
        {
            return Point(left + width, top + height);
        }

        constexpr Point GetLeftBottom() const
        {
            return Point(left, top + height);
        }

        constexpr Point GetRightTop() const
        {
            return Point(left + width, top);
        }

        constexpr Size GetSize() const
        {
            return Size(width, height);
        }

        constexpr Rect Shrink(const Thickness& thickness) const
        {
            return Rect(left + thickness.left, top + thickness.top, width - thickness.GetHorizontalTotal(), height - thickness.GetVerticalTotal());
        }

        constexpr bool IsPointInside(const Point& point) const
        {
            return
                point.x >= left &&
                point.x < GetRight() &&
                point.y >= top &&
                point.y < GetBottom();
        }

        float left = 0.0f;
        float top = 0.0f;
        float width = 0.0f;
        float height = 0.0f;
    };

    enum class MouseButton
    {
        Left,
        Right,
        Middle
    };

    struct TextRange
    {
        constexpr static std::optional<TextRange> FromTwoSides(unsigned first, unsigned second)
        {
            if (first > second)
                return std::make_optional<TextRange>(second, first - second);
            if (first < second)
                return std::make_optional<TextRange>(first, second - first);
            return std::nullopt;
        }

        constexpr static std::pair<unsigned, unsigned> ToTwoSides(std::optional<TextRange> text_range, unsigned default_position = 0)
        {
            if (text_range.has_value())
                return std::make_pair(text_range.value().position, text_range.value().position + text_range.value().count);
            return std::make_pair(default_position, default_position);
        }

        constexpr TextRange() = default;
        constexpr TextRange(const unsigned position, const unsigned count)
            : position(position), count(count)
        {

        }

        unsigned position = 0;
        unsigned count = 0;
    };

    bool IsKeyDown(int virtual_code);
    bool IsKeyToggled(int virtual_code);
    bool IsAnyMouseButtonDown();
}
//--------------------------------------------------------
//-------end of file: src\ui\ui_base.hpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\layout_base.hpp
//--------------------------------------------------------

// ReSharper disable once CppUnusedIncludeDirective

#include <unordered_set>


namespace cru::ui
{
    class Control;
    class Window;

    enum class Alignment
    {
        Center,
        Start,
        End
    };

    enum class MeasureMode
    {
        Exactly,
        Content,
        Stretch
    };

    enum class RectRange
    {
        Content, // content excluding padding, border and margin
        Padding, // only including content and padding
        HalfBorder, // including content, padding and half border
        FullBorder, // including content, padding and full border
        Margin // including content, padding, border and margin
    };

    struct LayoutSideParams final
    {
        constexpr static LayoutSideParams Exactly(const float length, const Alignment alignment = Alignment::Center)
        {
            return LayoutSideParams(MeasureMode::Exactly, length, alignment);
        }

        constexpr static LayoutSideParams Content(const Alignment alignment = Alignment::Center)
        {
            return LayoutSideParams(MeasureMode::Content, 0, alignment);
        }

        constexpr static LayoutSideParams Stretch(const Alignment alignment = Alignment::Center)
        {
            return LayoutSideParams(MeasureMode::Stretch, 0, alignment);
        }

        constexpr LayoutSideParams() = default;

        constexpr explicit LayoutSideParams(const MeasureMode mode, const float length, const Alignment alignment)
            : length(length), mode(mode), alignment(alignment)
        {

        }

        constexpr bool Validate() const
        {
            if (length < 0.0)
                return false;
            if (min.has_value() && min.value() < 0.0)
                return false;
            if (max.has_value() && max.value() < 0.0)
                return false;
            if (min.has_value() && max.has_value() && min.value() > max.value())
                return false;
            return true;
        }

        // only used in exactly mode, specify the exactly side length of content.
        float length = 0.0;
        MeasureMode mode = MeasureMode::Content;
        Alignment alignment = Alignment::Center;

        // min and max specify the min/max side length of content.
        // they are used as hint and respect the actual size that content needs.
        // when mode is exactly, length is coerced into the min-max range.
        std::optional<float> min = std::nullopt;
        std::optional<float> max = std::nullopt;
    };

    struct BasicLayoutParams final
    {
        BasicLayoutParams() = default;
        BasicLayoutParams(const BasicLayoutParams&) = default;
        BasicLayoutParams(BasicLayoutParams&&) = default;
        BasicLayoutParams& operator = (const BasicLayoutParams&) = default;
        BasicLayoutParams& operator = (BasicLayoutParams&&) = default;
        ~BasicLayoutParams() = default;

        bool Validate() const
        {
            return width.Validate() && height.Validate() && margin.Validate() && padding.Validate();
        }

        LayoutSideParams width;
        LayoutSideParams height;
        Thickness padding;
        Thickness margin;
    };


    class LayoutManager : public Object
    {
    public:
        static LayoutManager* GetInstance();
    private:
        LayoutManager() = default;
    public:
        LayoutManager(const LayoutManager& other) = delete;
        LayoutManager(LayoutManager&& other) = delete;
        LayoutManager& operator=(const LayoutManager& other) = delete;
        LayoutManager& operator=(LayoutManager&& other) = delete;
        ~LayoutManager() override = default;


        //*************** region: position cache ***************

        //Mark position cache of the control and its descendants invalid,
        //(which is saved as an auto-managed list internal)
        //and send a message to refresh them.
        void InvalidateControlPositionCache(Control* control);

        //Refresh position cache of the control and its descendants whose cache
        //has been marked as invalid.
        void RefreshInvalidControlPositionCache();

        //Refresh position cache of the control and its descendants immediately.
        static void RefreshControlPositionCache(Control* control);

    private:
        static void RefreshControlPositionCacheInternal(Control* control, const Point& parent_lefttop_absolute);

    private:
        std::unordered_set<Control*> cache_invalid_controls_;
        std::unordered_set<Window*> layout_invalid_windows_;
    };
}
//--------------------------------------------------------
//-------end of file: src\ui\layout_base.hpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\events\ui_event.hpp
//--------------------------------------------------------

// ReSharper disable once CppUnusedIncludeDirective

#include <optional>

//--------------------------------------------------------
//-------begin of file: src\cru_event.hpp
//--------------------------------------------------------

// ReSharper disable once CppUnusedIncludeDirective

#include <type_traits>
#include <functional>
#include <map>


namespace cru {
	//Base class of all event args.
	class BasicEventArgs : public Object
	{
	public:
		explicit BasicEventArgs(Object* sender)
            : sender_(sender)
		{
		    
		}
	    BasicEventArgs(const BasicEventArgs& other) = default;
	    BasicEventArgs(BasicEventArgs&& other) = default;
	    BasicEventArgs& operator=(const BasicEventArgs& other) = default;
	    BasicEventArgs& operator=(BasicEventArgs&& other) = default;
		~BasicEventArgs() override = default;

		//Get the sender of the event.
		Object* GetSender() const
		{
            return sender_;
		}

	private:
		Object* sender_;
	};


	//A non-copyable non-movable Event class.
	//It stores a list of event handlers.
	//TArgsType must be subclass of BasicEventArgs.
    template<typename TArgsType>
    class Event
	{
    public:
		static_assert(std::is_base_of_v<BasicEventArgs, TArgsType>,
			"TArgsType must be subclass of BasicEventArgs.");


        using ArgsType = TArgsType;
        using EventHandler = std::function<void(ArgsType&)>;
        using EventHandlerToken = long;

        Event() = default;
        Event(const Event&) = delete;
        Event& operator = (const Event&) = delete;
        Event(Event&&) = delete;
        Event& operator = (Event&&) = delete;
        ~Event() = default;

        EventHandlerToken AddHandler(const EventHandler& handler)
		{
            const auto token = current_token_++;
		    handlers_.emplace(token, handler);
            return token;
		}

		void RemoveHandler(const EventHandlerToken token) {
			auto find_result = handlers_.find(token);
			if (find_result != handlers_.cend())
				handlers_.erase(find_result);
		}

		void Raise(ArgsType& args) {
			for (const auto& handler : handlers_)
				(handler.second)(args);
		}


        //TODO: Remove this!
        bool IsNoHandler() const
		{
		    return handlers_.empty();
		}

    private:
        std::map<EventHandlerToken, EventHandler> handlers_;

        EventHandlerToken current_token_ = 0;
    };
}
//--------------------------------------------------------
//-------end of file: src\cru_event.hpp
//--------------------------------------------------------

namespace cru::ui
{
    class Control;
}

namespace cru::ui::events
{
    class UiEventArgs : public BasicEventArgs
    {
    public:
        UiEventArgs(Object* sender, Object* original_sender)
            : BasicEventArgs(sender), original_sender_(original_sender), handled_(false)
        {

        }

        UiEventArgs(const UiEventArgs& other) = default;
        UiEventArgs(UiEventArgs&& other) = default;
        UiEventArgs& operator=(const UiEventArgs& other) = default;
        UiEventArgs& operator=(UiEventArgs&& other) = default;
        ~UiEventArgs() override = default;

        Object* GetOriginalSender() const
        {
            return original_sender_;
        }

        bool IsHandled() const
        {
            return handled_;
        }

        void SetHandled(const bool handled = true)
        {
            handled_ = handled;
        }

    private:
        Object* original_sender_;
        bool handled_;
    };

    template <typename TEventArgs>
    class RoutedEvent
    {
    public:
        static_assert(std::is_base_of_v<UiEventArgs, TEventArgs>, "TEventArgs must be subclass of UiEventArgs.");

        using EventArgs = TEventArgs;

        RoutedEvent() = default;
        RoutedEvent(const RoutedEvent& other) = delete;
        RoutedEvent(RoutedEvent&& other) = delete;
        RoutedEvent& operator=(const RoutedEvent& other) = delete;
        RoutedEvent& operator=(RoutedEvent&& other) = delete;
        ~RoutedEvent() = default;

        Event<TEventArgs> direct;
        Event<TEventArgs> bubble;
        Event<TEventArgs> tunnel;
    };

    class MouseEventArgs : public UiEventArgs
    {
    public:
        MouseEventArgs(Object* sender, Object* original_sender, const std::optional<Point>& point = std::nullopt)
            : UiEventArgs(sender, original_sender), point_(point)
        {

        }
        MouseEventArgs(const MouseEventArgs& other) = default;
        MouseEventArgs(MouseEventArgs&& other) = default;
        MouseEventArgs& operator=(const MouseEventArgs& other) = default;
        MouseEventArgs& operator=(MouseEventArgs&& other) = default;
        ~MouseEventArgs() override = default;

        Point GetPoint(Control* control, RectRange range = RectRange::Content) const;

    private:
        std::optional<Point> point_;
    };


    class MouseButtonEventArgs : public MouseEventArgs
    {
    public:
        MouseButtonEventArgs(Object* sender, Object* original_sender, const Point& point, const MouseButton button)
            : MouseEventArgs(sender, original_sender, point), button_(button)
        {

        }
        MouseButtonEventArgs(const MouseButtonEventArgs& other) = default;
        MouseButtonEventArgs(MouseButtonEventArgs&& other) = default;
        MouseButtonEventArgs& operator=(const MouseButtonEventArgs& other) = default;
        MouseButtonEventArgs& operator=(MouseButtonEventArgs&& other) = default;
        ~MouseButtonEventArgs() override = default;

        MouseButton GetMouseButton() const
        {
            return button_;
        }

    private:
        MouseButton button_;
    };


    class MouseWheelEventArgs : public MouseEventArgs
    {
    public:
        MouseWheelEventArgs(Object* sender, Object* original_sender, const Point& point, const float delta)
            : MouseEventArgs(sender, original_sender, point), delta_(delta)
        {

        }
        MouseWheelEventArgs(const MouseWheelEventArgs& other) = default;
        MouseWheelEventArgs(MouseWheelEventArgs&& other) = default;
        MouseWheelEventArgs& operator=(const MouseWheelEventArgs& other) = default;
        MouseWheelEventArgs& operator=(MouseWheelEventArgs&& other) = default;
        ~MouseWheelEventArgs() override = default;

        float GetDelta() const
        {
            return delta_;
        }

    private:
        float delta_;
    };


    class DrawEventArgs : public UiEventArgs
    {
    public:
        DrawEventArgs(Object* sender, Object* original_sender, ID2D1DeviceContext* device_context)
            : UiEventArgs(sender, original_sender), device_context_(device_context)
        {

        }
        DrawEventArgs(const DrawEventArgs& other) = default;
        DrawEventArgs(DrawEventArgs&& other) = default;
        DrawEventArgs& operator=(const DrawEventArgs& other) = default;
        DrawEventArgs& operator=(DrawEventArgs&& other) = default;
        ~DrawEventArgs() = default;

        ID2D1DeviceContext* GetDeviceContext() const
        {
            return device_context_;
        }

    private:
        ID2D1DeviceContext * device_context_;
    };


    class PositionChangedEventArgs : public UiEventArgs
    {
    public:
        PositionChangedEventArgs(Object* sender, Object* original_sender, const Point& old_position, const Point& new_position)
            : UiEventArgs(sender, original_sender), old_position_(old_position), new_position_(new_position)
        {

        }
        PositionChangedEventArgs(const PositionChangedEventArgs& other) = default;
        PositionChangedEventArgs(PositionChangedEventArgs&& other) = default;
        PositionChangedEventArgs& operator=(const PositionChangedEventArgs& other) = default;
        PositionChangedEventArgs& operator=(PositionChangedEventArgs&& other) = default;
        ~PositionChangedEventArgs() override = default;

        Point GetOldPosition() const
        {
            return old_position_;
        }

        Point GetNewPosition() const
        {
            return new_position_;
        }

    private:
        Point old_position_;
        Point new_position_;
    };


    class SizeChangedEventArgs : public UiEventArgs
    {
    public:
        SizeChangedEventArgs(Object* sender, Object* original_sender, const Size& old_size, const Size& new_size)
            : UiEventArgs(sender, original_sender), old_size_(old_size), new_size_(new_size)
        {

        }
        SizeChangedEventArgs(const SizeChangedEventArgs& other) = default;
        SizeChangedEventArgs(SizeChangedEventArgs&& other) = default;
        SizeChangedEventArgs& operator=(const SizeChangedEventArgs& other) = default;
        SizeChangedEventArgs& operator=(SizeChangedEventArgs&& other) = default;
        ~SizeChangedEventArgs() override = default;

        Size GetOldSize() const
        {
            return old_size_;
        }

        Size GetNewSize() const
        {
            return new_size_;
        }

    private:
        Size old_size_;
        Size new_size_;
    };

    class FocusChangeEventArgs : public UiEventArgs
    {
    public:
        FocusChangeEventArgs(Object* sender, Object* original_sender, const bool is_window = false)
            : UiEventArgs(sender, original_sender), is_window_(is_window)
        {

        }
        FocusChangeEventArgs(const FocusChangeEventArgs& other) = default;
        FocusChangeEventArgs(FocusChangeEventArgs&& other) = default;
        FocusChangeEventArgs& operator=(const FocusChangeEventArgs& other) = default;
        FocusChangeEventArgs& operator=(FocusChangeEventArgs&& other) = default;
        ~FocusChangeEventArgs() override = default;

        // Return whether the focus change is caused by the window-wide focus change.
        bool IsWindow() const
        {
            return is_window_;
        }

    private:
        bool is_window_;
    };

    class ToggleEventArgs : public UiEventArgs
    {
    public:
        ToggleEventArgs(Object* sender, Object* original_sender, bool new_state)
            : UiEventArgs(sender, original_sender), new_state_(new_state)
        {

        }
        ToggleEventArgs(const ToggleEventArgs& other) = default;
        ToggleEventArgs(ToggleEventArgs&& other) = default;
        ToggleEventArgs& operator=(const ToggleEventArgs& other) = default;
        ToggleEventArgs& operator=(ToggleEventArgs&& other) = default;
        ~ToggleEventArgs() override = default;

        bool GetNewState() const
        {
            return new_state_;
        }

    private:
        bool new_state_;
    };

    struct WindowNativeMessage
    {
        HWND hwnd;
        int msg;
        WPARAM w_param;
        LPARAM l_param;
    };

    class WindowNativeMessageEventArgs : public UiEventArgs
    {
    public:
        WindowNativeMessageEventArgs(Object* sender, Object* original_sender, const WindowNativeMessage& message)
            : UiEventArgs(sender, original_sender), message_(message), result_(std::nullopt)
        {

        }
        WindowNativeMessageEventArgs(const WindowNativeMessageEventArgs& other) = default;
        WindowNativeMessageEventArgs(WindowNativeMessageEventArgs&& other) = default;
        WindowNativeMessageEventArgs& operator=(const WindowNativeMessageEventArgs& other) = default;
        WindowNativeMessageEventArgs& operator=(WindowNativeMessageEventArgs&& other) = default;
        ~WindowNativeMessageEventArgs() override = default;

        WindowNativeMessage GetWindowMessage() const
        {
            return message_;
        }

        std::optional<LRESULT> GetResult() const
        {
            return result_;
        }

        void SetResult(const std::optional<LRESULT> result)
        {
            result_ = result;
        }

    private:
        WindowNativeMessage message_;
        std::optional<LRESULT> result_;
    };

    class KeyEventArgs : public UiEventArgs
    {
    public:
        KeyEventArgs(Object* sender, Object* original_sender, int virtual_code)
            : UiEventArgs(sender, original_sender), virtual_code_(virtual_code)
        {
        }
        KeyEventArgs(const KeyEventArgs& other) = default;
        KeyEventArgs(KeyEventArgs&& other) = default;
        KeyEventArgs& operator=(const KeyEventArgs& other) = default;
        KeyEventArgs& operator=(KeyEventArgs&& other) = default;
        ~KeyEventArgs() override = default;

        int GetVirtualCode() const
        {
            return virtual_code_;
        }

    private:
        int virtual_code_;
    };

    class CharEventArgs : public UiEventArgs
    {
    public:
        CharEventArgs(Object* sender, Object* original_sender, wchar_t c)
            : UiEventArgs(sender, original_sender), c_(c)
        {
        }
        CharEventArgs(const CharEventArgs& other) = default;
        CharEventArgs(CharEventArgs&& other) = default;
        CharEventArgs& operator=(const CharEventArgs& other) = default;
        CharEventArgs& operator=(CharEventArgs&& other) = default;
        ~CharEventArgs() override = default;

        wchar_t GetChar() const
        {
            return c_;
        }

    private:
        wchar_t c_;
    };
}
//--------------------------------------------------------
//-------end of file: src\ui\events\ui_event.hpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\border_property.hpp
//--------------------------------------------------------

// ReSharper disable once CppUnusedIncludeDirective




namespace cru::ui
{
    class BorderProperty final
    {
    public:
        BorderProperty();
        explicit BorderProperty(Microsoft::WRL::ComPtr<ID2D1Brush> brush);
        BorderProperty(Microsoft::WRL::ComPtr<ID2D1Brush> brush, float width, float radius_x, float radius_y, Microsoft::WRL::ComPtr<ID2D1StrokeStyle> stroke_style = nullptr);
        BorderProperty(const BorderProperty& other) = default;
        BorderProperty(BorderProperty&& other) = default;
        BorderProperty& operator=(const BorderProperty& other) = default;
        BorderProperty& operator=(BorderProperty&& other) = default;
        ~BorderProperty() = default;


        Microsoft::WRL::ComPtr<ID2D1Brush> GetBrush() const
        {
            return brush_;
        }

        float GetStrokeWidth() const
        {
            return stroke_width_;
        }

        Microsoft::WRL::ComPtr<ID2D1StrokeStyle> GetStrokeStyle() const
        {
            return stroke_style_;
        }

        float GetRadiusX() const
        {
            return radius_x_;
        }

        float GetRadiusY() const
        {
            return radius_y_;
        }

        void SetBrush(Microsoft::WRL::ComPtr<ID2D1Brush> brush)
        {
            Require(brush == nullptr, "Brush of BorderProperty mustn't be null.");
            brush_ = std::move(brush);
        }

        void SetStrokeWidth(const float stroke_width)
        {
            Require(stroke_width >= 0.0f, "Stroke width must be no less than 0.");
            stroke_width_ = stroke_width;
        }

        void SetStrokeStyle(Microsoft::WRL::ComPtr<ID2D1StrokeStyle> stroke_style)
        {
            stroke_style_ = std::move(stroke_style);
        }

        void SetRadiusX(const float radius_x)
        {
            Require(radius_x >= 0.0f, "Radius-x must be no less than 0.");
            radius_x_ = radius_x;
        }

        void SetRadiusY(const float radius_y)
        {
            Require(radius_y >= 0.0f, "Radius-y must be no less than 0.");
            radius_y_ = radius_y;
        }

    private:
        Microsoft::WRL::ComPtr<ID2D1Brush> brush_;
        float stroke_width_ = 1.0f;
        float radius_x_ = 0.0f;
        float radius_y_ = 0.0f;
        Microsoft::WRL::ComPtr<ID2D1StrokeStyle> stroke_style_ = nullptr;
    };
}
//--------------------------------------------------------
//-------end of file: src\ui\border_property.hpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\cursor.hpp
//--------------------------------------------------------

// ReSharper disable once CppUnusedIncludeDirective

#include <memory>


namespace cru::ui
{
    class Cursor : public Object
    {
    public:
        using Ptr = std::shared_ptr<Cursor>;

        Cursor(HCURSOR handle, bool auto_release);
        Cursor(const Cursor& other) = delete;
        Cursor(Cursor&& other) = delete;
        Cursor& operator=(const Cursor& other) = delete;
        Cursor& operator=(Cursor&& other) = delete;
        ~Cursor() override;

        HCURSOR GetHandle() const
        {
            return handle_;
        }

    private:
        HCURSOR handle_;
        bool auto_release_;
    };

    namespace cursors
    {
        extern Cursor::Ptr arrow;
        extern Cursor::Ptr hand;
        extern Cursor::Ptr i_beam;

        void LoadSystemCursors();
    }
}
//--------------------------------------------------------
//-------end of file: src\ui\cursor.hpp
//--------------------------------------------------------

namespace cru::ui
{
    class Control;
    class Window;


    //the position cache
    struct ControlPositionCache
    {
        //The lefttop relative to the ancestor.
        Point lefttop_position_absolute;
    };


    class Control : public Object
    {
        friend class Window;
        friend class LayoutManager;

    protected:
        struct GeometryInfo
        {
            Microsoft::WRL::ComPtr<ID2D1Geometry> border_geometry = nullptr;
            Microsoft::WRL::ComPtr<ID2D1Geometry> padding_content_geometry = nullptr;
            Microsoft::WRL::ComPtr<ID2D1Geometry> content_geometry = nullptr;
        };


    protected:
        struct WindowConstructorTag {}; //Used for constructor for class Window. 

        explicit Control(bool container = false);

        // Used only for creating Window. It will set window_ as window.
        Control(WindowConstructorTag, Window* window);

    public:
        Control(const Control& other) = delete;
        Control(Control&& other) = delete;
        Control& operator=(const Control& other) = delete;
        Control& operator=(Control&& other) = delete;
        ~Control() override;

    public:

        //*************** region: tree ***************
        virtual StringView GetControlType() const = 0;

        bool IsContainer() const
        {
            return is_container_;
        }

        //Get parent of control, return nullptr if it has no parent.
        Control* GetParent() const
        {
            return parent_;
        }

        //Return a immutable vector of all children.
        const std::vector<Control*>& GetChildren() const;

        //Add a child at tail.
        void AddChild(Control* control);

        //Add a child before the position.
        void AddChild(Control* control, int position);

        //Remove a child.
        void RemoveChild(Control* child);

        //Remove a child at specified position.
        void RemoveChild(int position);

        //Get the ancestor of the control.
        Control* GetAncestor();

        //Get the window if attached, otherwise, return nullptr.
        Window* GetWindow() const
        {
            return window_;
        }

        //Traverse the tree rooted the control including itself.
        void TraverseDescendants(const std::function<void(Control*)>& predicate);

        //*************** region: position and size ***************
        // Position and size part must be isolated from layout part.
        // All the operations in this part must be done independently.
        // And layout part must use api of this part.

        //Get the lefttop relative to its parent.
        virtual Point GetPositionRelative();

        //Set the lefttop relative to its parent.
        virtual void SetPositionRelative(const Point& position);

        //Get the actual size.
        virtual Size GetSize();

        //Set the actual size directly without re-layout.
        virtual void SetSize(const Size& size);

        //Get lefttop relative to ancestor. This is only valid when
        //attached to window. Notice that the value is cached.
        //You can invalidate and recalculate it by calling "InvalidatePositionCache". 
        Point GetPositionAbsolute() const;

        //Local point to absolute point.
        Point ControlToWindow(const Point& point) const;

        //Absolute point to local point.
        Point WindowToControl(const Point& point) const;

        // Default implement in Control is test point in border geometry's
        // fill and stroke with width of border.
        virtual bool IsPointInside(const Point& point);

        // Get the top control among all descendants (including self) in local coordinate.
        virtual Control* HitTest(const Point& point);

        //*************** region: graphic ***************

        bool IsClipContent() const
        {
            return clip_content_;
        }

        void SetClipContent(bool clip);

        //Draw this control and its child controls.
        void Draw(ID2D1DeviceContext* device_context);

        virtual void InvalidateDraw();

        Microsoft::WRL::ComPtr<ID2D1Brush> GetForegroundBrush() const
        {
            return foreground_brush_;
        }

        void SetForegroundBrush(Microsoft::WRL::ComPtr<ID2D1Brush> foreground_brush)
        {
            foreground_brush_ = std::move(foreground_brush);
            InvalidateDraw();
        }

        Microsoft::WRL::ComPtr<ID2D1Brush> GetBackgroundBrush() const
        {
            return background_brush_;
        }

        void SetBackgroundBrush(Microsoft::WRL::ComPtr<ID2D1Brush> background_brush)
        {
            background_brush_ = std::move(background_brush);
            InvalidateDraw();
        }


        //*************** region: focus ***************

        bool RequestFocus();

        bool HasFocus();

        bool IsFocusOnPressed() const
        {
            return is_focus_on_pressed_;
        }

        void SetFocusOnPressed(const bool value)
        {
            is_focus_on_pressed_ = value;
        }

        //*************** region: layout ***************

        void InvalidateLayout();

        void Measure(const Size& available_size);

        void Layout(const Rect& rect);

        Size GetDesiredSize() const;

        void SetDesiredSize(const Size& desired_size);

        BasicLayoutParams* GetLayoutParams()
        {
            return &layout_params_;
        }

        Rect GetRect(RectRange range);

        Point TransformPoint(const Point& point, RectRange from = RectRange::Margin, RectRange to = RectRange::Content);

        //*************** region: border ***************

        BorderProperty& GetBorderProperty()
        {
            return border_property_;
        }

        void UpdateBorder();

        bool IsBordered() const
        {
            return is_bordered_;
        }

        void SetBordered(bool bordered);


        //*************** region: additional properties ***************
        AnyMap* GetAdditionalPropertyMap()
        {
            return &additional_property_map_;
        }

        
        //*************** region: cursor ***************
        // If cursor is set to null, then it uses parent's cursor.
        // Window's cursor can't be null.

        Cursor::Ptr GetCursor() const
        {
            return cursor_;
        }

        void SetCursor(const Cursor::Ptr& cursor);


        //*************** region: events ***************
        //Raised when mouse enter the control.
        events::RoutedEvent<events::MouseEventArgs> mouse_enter_event;
        //Raised when mouse is leave the control.
        events::RoutedEvent<events::MouseEventArgs> mouse_leave_event;
        //Raised when mouse is move in the control.
        events::RoutedEvent<events::MouseEventArgs> mouse_move_event;
        //Raised when a mouse button is pressed in the control.
        events::RoutedEvent<events::MouseButtonEventArgs> mouse_down_event;
        //Raised when a mouse button is released in the control.
        events::RoutedEvent<events::MouseButtonEventArgs> mouse_up_event;
        //Raised when a mouse button is pressed in the control and released in the control with mouse not leaving it between two operations.
        events::RoutedEvent<events::MouseButtonEventArgs> mouse_click_event;

        events::RoutedEvent<events::MouseWheelEventArgs> mouse_wheel_event;

        events::RoutedEvent<events::KeyEventArgs> key_down_event;
        events::RoutedEvent<events::KeyEventArgs> key_up_event;
        events::RoutedEvent<events::CharEventArgs> char_event;

        events::RoutedEvent<events::FocusChangeEventArgs> get_focus_event;
        events::RoutedEvent<events::FocusChangeEventArgs> lose_focus_event;

        Event<events::DrawEventArgs> draw_content_event;
        Event<events::DrawEventArgs> draw_background_event;
        Event<events::DrawEventArgs> draw_foreground_event;

        Event<events::PositionChangedEventArgs> position_changed_event;
        Event<events::SizeChangedEventArgs> size_changed_event;

        //*************** region: tree event ***************
    protected:
        //Invoked when a child is added. Overrides should invoke base.
        virtual void OnAddChild(Control* child);
        //Invoked when a child is removed. Overrides should invoke base.
        virtual void OnRemoveChild(Control* child);

        //Invoked when the control is attached to a window. Overrides should invoke base.
        virtual void OnAttachToWindow(Window* window);
        //Invoked when the control is detached to a window. Overrides should invoke base.
        virtual void OnDetachToWindow(Window* window);


        //*************** region: graphic event ***************
    private:
        void OnDrawDecoration(ID2D1DeviceContext* device_context);
        void OnDrawCore(ID2D1DeviceContext* device_context);


        //*************** region: position and size event ***************
    protected:
        void RegenerateGeometryInfo();

        const GeometryInfo& GetGeometryInfo() const
        {
            return geometry_info_;
        }


        //*************** region: mouse event ***************
    protected:
        virtual void OnMouseClickBegin(MouseButton button);
        virtual void OnMouseClickEnd(MouseButton button);


        //*************** region: layout ***************
    private:
        Size OnMeasureCore(const Size& available_size);
        void OnLayoutCore(const Rect& rect);

    protected:
        virtual Size OnMeasureContent(const Size& available_size);
        virtual void OnLayoutContent(const Rect& rect);

        // Called by Layout after set position and size.
        virtual void AfterLayoutSelf();

    private:
        // Only for layout manager to use.
        // Check if the old position is updated to current position.
        // If not, then a notify of position change and update will
        // be done.
        void CheckAndNotifyPositionChanged();

        void ThrowIfNotContainer() const
        {
            if (!is_container_)
                throw std::runtime_error("You can't perform such operation on a non-container control.");
        }

    private:
        bool is_container_;

        Window * window_ = nullptr;

        Control * parent_ = nullptr;
        std::vector<Control*> children_{};

        // When position is changed and notification hasn't been
        // sent, it will be the old position. When position is changed
        // more than once, it will be the oldest position since last
        // notification. If notification has been sent, it will be updated
        // to position_.
        Point old_position_ = Point::Zero();
        Point position_ = Point::Zero();
        Size size_ = Size::Zero();

        ControlPositionCache position_cache_{};

        std::unordered_map<MouseButton, bool> is_mouse_click_valid_map_
        {
            { MouseButton::Left, true },
            { MouseButton::Middle, true },
            { MouseButton::Right, true }
        }; // used for clicking determination

        BasicLayoutParams layout_params_{};
        Size desired_size_ = Size::Zero();

        bool is_bordered_ = false;
        BorderProperty border_property_;

        GeometryInfo geometry_info_{};

        bool clip_content_ = false;

        Microsoft::WRL::ComPtr<ID2D1Brush> foreground_brush_ = nullptr;
        Microsoft::WRL::ComPtr<ID2D1Brush> background_brush_ = nullptr;

        AnyMap additional_property_map_{};

        bool is_focus_on_pressed_ = true;

#ifdef CRU_DEBUG_LAYOUT
        Microsoft::WRL::ComPtr<ID2D1Geometry> margin_geometry_;
        Microsoft::WRL::ComPtr<ID2D1Geometry> padding_geometry_;
#endif

        Cursor::Ptr cursor_{};
    };


    //*************** region: event dispatcher helper ***************

    // Dispatch the event.
    // 
    // This will raise routed event of the control and its parent and parent's
    // parent ... (until "last_receiver" if it's not nullptr) with appropriate args.
    // 
    // First tunnel from top to bottom possibly stopped by "handled" flag in EventArgs.
    // Second bubble from bottom to top possibly stopped by "handled" flag in EventArgs.
    // Last direct to each control.
    // 
    // Args is of type "EventArgs". The first init argument is "sender", which is
    // automatically bound to each receiving control. The second init argument is
    // "original_sender", which is unchanged. And "args" will be perfectly forwarded
    // as the rest arguments.
    template<typename EventArgs, typename... Args>
    void DispatchEvent(Control* const original_sender, events::RoutedEvent<EventArgs> Control::* event_ptr, Control* const last_receiver, Args&&... args)
    {
        std::list<Control*> receive_list;

        auto parent = original_sender;
        while (parent != last_receiver)
        {
            receive_list.push_back(parent);
            parent = parent->GetParent();
        }

        auto handled = false;

        //tunnel
        for (auto i = receive_list.crbegin(); i != receive_list.crend(); ++i)
        {
            EventArgs event_args(*i, original_sender, std::forward<Args>(args)...);
            (*i->*event_ptr).tunnel.Raise(event_args);
            if (event_args.IsHandled())
            {
                handled = true;
                break;
            }
        }

        //bubble
        if (!handled)
        {
            for (auto i : receive_list)
            {
                EventArgs event_args(i, original_sender, std::forward<Args>(args)...);
                (i->*event_ptr).bubble.Raise(event_args);
                if (event_args.IsHandled())
                    break;
            }
        }

        //direct
        for (auto i : receive_list)
        {
            EventArgs event_args(i, original_sender, std::forward<Args>(args)...);
            (i->*event_ptr).direct.Raise(event_args);
        }
    }


    //*************** region: tree helper ***************

    // Find the lowest common ancestor.
    // Return nullptr if "left" and "right" are not in the same tree.
    Control* FindLowestCommonAncestor(Control* left, Control* right);

    // Return the ancestor if one control is the ancestor of the other one, otherwise nullptr.
    Control* IsAncestorOrDescendant(Control* left, Control* right);

    template <typename TControl, typename... Args>
    TControl* CreateWithLayout(const LayoutSideParams& width, const LayoutSideParams& height, Args&&... args)
    {
        static_assert(std::is_base_of_v<Control, TControl>, "TControl is not a control class.");
        TControl* control = TControl::Create(std::forward<Args>(args)...);
        control->GetLayoutParams()->width = width;
        control->GetLayoutParams()->height = height;
        return control;
    }


    //*************** region: create helper ***************

    template <typename TControl, typename... Args>
    TControl* CreateWithLayout(const Thickness& padding, const Thickness& margin, Args&&... args)
    {
        static_assert(std::is_base_of_v<Control, TControl>, "TControl is not a control class.");
        TControl* control = TControl::Create(std::forward<Args>(args)...);
        control->GetLayoutParams()->padding = padding;
        control->GetLayoutParams()->margin = margin;
        return control;
    }

    template <typename TControl, typename... Args>
    TControl* CreateWithLayout(const LayoutSideParams& width, const LayoutSideParams& height, const Thickness& padding, const Thickness& margin, Args&&... args)
    {
        static_assert(std::is_base_of_v<Control, TControl>, "TControl is not a control class.");
        TControl* control = TControl::Create(std::forward<Args>(args)...);
        control->GetLayoutParams()->width = width;
        control->GetLayoutParams()->height = height;
        control->GetLayoutParams()->padding = padding;
        control->GetLayoutParams()->margin = margin;
        return control;
    }

    using ControlList = std::initializer_list<Control*>;
}
//--------------------------------------------------------
//-------end of file: src\ui\control.hpp
//--------------------------------------------------------

namespace cru::graph
{
    class WindowRenderTarget;
}

namespace cru::ui
{
    class WindowClass : public Object
    {
    public:
        WindowClass(const String& name, WNDPROC window_proc, HINSTANCE h_instance);
        WindowClass(const WindowClass& other) = delete;
        WindowClass(WindowClass&& other) = delete;
        WindowClass& operator=(const WindowClass& other) = delete;
        WindowClass& operator=(WindowClass&& other) = delete;
        ~WindowClass() override = default;


        const wchar_t* GetName() const
        {
            return name_.c_str();
        }

        ATOM GetAtom() const
        {
            return atom_;
        }

    private:
        String name_;
        ATOM atom_;
    };

    class WindowManager : public Object
    {
    public:
        static WindowManager* GetInstance();
    private:
        WindowManager();
    public:
        WindowManager(const WindowManager& other) = delete;
        WindowManager(WindowManager&& other) = delete;
        WindowManager& operator=(const WindowManager& other) = delete;
        WindowManager& operator=(WindowManager&& other) = delete;
        ~WindowManager() override = default;


        //Get the general window class for creating ordinary window.
        WindowClass* GetGeneralWindowClass() const
        {
            return general_window_class_.get();
        }

        //Register a window newly created.
        //This function adds the hwnd to hwnd-window map.
        //It should be called immediately after a window was created.
        void RegisterWindow(HWND hwnd, Window* window);

        //Unregister a window that is going to be destroyed.
        //This function removes the hwnd from the hwnd-window map.
        //It should be called immediately before a window is going to be destroyed,
        void UnregisterWindow(HWND hwnd);

        //Return a pointer to the Window object related to the HWND or nullptr if the hwnd is not in the map.
        Window* FromHandle(HWND hwnd);

        std::vector<Window*> GetAllWindows() const;

    private:
        std::unique_ptr<WindowClass> general_window_class_;
        std::map<HWND, Window*> window_map_;
    };



    class Window final : public Control
    {
        friend class WindowManager;
    public:
        static constexpr auto control_type = L"Window";

    public:
        static Window* CreateOverlapped();
        static Window* CreatePopup(Window* parent, bool caption = false);

    private:
        struct tag_overlapped_constructor {};
        struct tag_popup_constructor {};

        explicit Window(tag_overlapped_constructor);
        Window(tag_popup_constructor, Window* parent, bool caption);

        void AfterCreateHwnd(WindowManager* window_manager);

    public:
        Window(const Window& other) = delete;
        Window(Window&& other) = delete;
        Window& operator=(const Window& other) = delete;
        Window& operator=(Window&& other) = delete;
        ~Window() override;

    public:
        StringView GetControlType() const override final;

        void SetDeleteThisOnDestroy(bool value);

        //*************** region: handle ***************

        //Get the handle of the window. Return null if window is invalid.
        HWND GetWindowHandle() const
        {
            return hwnd_;
        }

        //Return if the window is still valid, that is, hasn't been closed or destroyed.
        bool IsWindowValid() const
        {
            return hwnd_ != nullptr;
        }


        //*************** region: window operations ***************

        Window* GetParentWindow() const
        {
            return parent_window_;
        }

        //Close and destroy the window if the window is valid.
        void Close();

        //Send a repaint message to the window's message queue which may make the window repaint.
        void InvalidateDraw() override final;

        //Show the window.
        void Show();

        //Hide thw window.
        void Hide();

        //Get the client size.
        Size GetClientSize();

        //Set the client size and repaint.
        void SetClientSize(const Size& size);

        //Get the rect of the window containing frame.
        //The lefttop of the rect is relative to screen lefttop.
        Rect GetWindowRect();

        //Set the rect of the window containing frame.
        //The lefttop of the rect is relative to screen lefttop.
        void SetWindowRect(const Rect& rect);

        //Set the lefttop of the window relative to screen.
        void SetWindowPosition(const Point& position);

        Point PointToScreen(const Point& point);

        Point PointFromScreen(const Point& point);

        //Handle the raw window message.
        //Return true if the message is handled and get the result through "result" argument.
        //Return false if the message is not handled.
        bool HandleWindowMessage(HWND hwnd, int msg, WPARAM w_param, LPARAM l_param, LRESULT& result);

        //*************** region: mouse ***************

        Point GetMousePosition();

        Control* GetMouseHoverControl() const
        {
            return mouse_hover_control_;
        }

        //*************** region: position and size ***************

        //Always return (0, 0) for a window.
        Point GetPositionRelative() override final;

        //This method has no effect for a window.
        void SetPositionRelative(const Point& position) override final;

        //Get the size of client area for a window.
        Size GetSize() override final;

        //This method has no effect for a window. Use SetClientSize instead.
        void SetSize(const Size& size) override final;

        //Override. If point is in client area, it is in window.
        bool IsPointInside(const Point& point) override final;

        //*************** region: layout ***************

        void WindowInvalidateLayout();

        void Relayout();

        void SetSizeFitContent(const Size& max_size = Size(1000, 1000));


        //*************** region: focus ***************

        //Request focus for specified control.
        bool RequestFocusFor(Control* control);

        //Get the control that has focus.
        Control* GetFocusControl();


        //*************** region: mouse capture ***************

        Control* CaptureMouseFor(Control* control);
        Control* ReleaseCurrentMouseCapture();

        
        //*************** region: cursor ***************
        void UpdateCursor();

        //*************** region: debug ***************
#ifdef CRU_DEBUG_LAYOUT
        bool IsDebugLayout() const
        {
            return debug_layout_;
        }

        void SetDebugLayout(bool value);
#endif
 
    public:
        //*************** region: events ***************
        Event<events::UiEventArgs> activated_event;
        Event<events::UiEventArgs> deactivated_event;

        Event<events::WindowNativeMessageEventArgs> native_message_event;
 
    private:
        //*************** region: native operations ***************
 
        //Get the client rect in pixel.
        RECT GetClientRectPixel();
 
        bool IsMessageInQueue(UINT message);
 
        void SetCursorInternal(HCURSOR cursor);
 
 
        //*************** region: native messages ***************
 
        void OnDestroyInternal();
        void OnPaintInternal();
        void OnResizeInternal(int new_width, int new_height);
 
        void OnSetFocusInternal();
        void OnKillFocusInternal();
 
        void OnMouseMoveInternal(POINT point);
        void OnMouseLeaveInternal();
        void OnMouseDownInternal(MouseButton button, POINT point);
        void OnMouseUpInternal(MouseButton button, POINT point);

        void OnMouseWheelInternal(short delta, POINT point);
        void OnKeyDownInternal(int virtual_code);
        void OnKeyUpInternal(int virtual_code);
        void OnCharInternal(wchar_t c);
 
        void OnActivatedInternal();
        void OnDeactivatedInternal();
 
        //*************** region: event dispatcher helper ***************
 
        void DispatchMouseHoverControlChangeEvent(Control* old_control, Control * new_control, const Point& point);
 
    private:
        bool delete_this_on_destroy_ = true;

        HWND hwnd_ = nullptr;
        Window* parent_window_ = nullptr;
        std::shared_ptr<graph::WindowRenderTarget> render_target_{};
 
        Control* mouse_hover_control_ = nullptr;
 
        bool window_focus_ = false;
        Control* focus_control_ = this; // "focus_control_" can't be nullptr
 
        Control* mouse_capture_control_ = nullptr;

        bool is_layout_invalid_ = false;

#ifdef CRU_DEBUG_LAYOUT
        bool debug_layout_ = false;
#endif
    };
}
//--------------------------------------------------------
//-------end of file: src\ui\window.hpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\cru_debug.hpp
//--------------------------------------------------------

// ReSharper disable once CppUnusedIncludeDirective

#include <functional>


namespace cru::debug
{
    void DebugMessage(const StringView& message);

#ifdef CRU_DEBUG
    inline void DebugTime(const std::function<void()>& action, const StringView& hint_message)
    {
        const auto before = std::chrono::steady_clock::now();
        action();
        const auto after = std::chrono::steady_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
        DebugMessage(Format(L"{}: {}ms.\n", hint_message, duration.count()));
    }

    template<typename TReturn>
    TReturn DebugTime(const std::function<TReturn()>& action, const StringView& hint_message)
    {
        const auto before = std::chrono::steady_clock::now();
        auto&& result = action();
        const auto after = std::chrono::steady_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
        DebugMessage(Format(L"{}: {}ms.\n", hint_message, duration.count()));
        return std::move(result);
    }
#else
    inline void DebugTime(const std::function<void()>& action, const StringView& hint_message)
    {
        action();
    }

    template<typename TReturn>
    TReturn DebugTime(const std::function<TReturn()>& action, const StringView& hint_message)
    {
        return action();
    }
#endif
}
//--------------------------------------------------------
//-------end of file: src\cru_debug.hpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\controls\linear_layout.hpp
//--------------------------------------------------------

// ReSharper disable once CppUnusedIncludeDirective


namespace cru::ui::controls
{
    // Min length of main side in layout params is of no meaning.
    // All children will layout from start and redundant length is blank.
    class LinearLayout : public Control
    {
    public:
        static constexpr auto control_type = L"LinearLayout";

        enum class Orientation
        {
            Horizontal,
            Vertical
        };

        static LinearLayout* Create(const Orientation orientation = Orientation::Vertical, const std::initializer_list<Control*>& children = std::initializer_list<Control*>())
        {
            const auto linear_layout = new LinearLayout(orientation);
                        for (const auto control : children)
                linear_layout->AddChild(control);
            return linear_layout;
        }

    protected:
        explicit LinearLayout(Orientation orientation = Orientation::Vertical);

    public:
        LinearLayout(const LinearLayout& other) = delete;
        LinearLayout(LinearLayout&& other) = delete;
        LinearLayout& operator=(const LinearLayout& other) = delete;
        LinearLayout& operator=(LinearLayout&& other) = delete;
        ~LinearLayout() override = default;

        StringView GetControlType() const override final;

    protected:
        Size OnMeasureContent(const Size& available_size) override;
        void OnLayoutContent(const Rect& rect) override;

    private:
        Orientation orientation_;
    };
}
//--------------------------------------------------------
//-------end of file: src\ui\controls\linear_layout.hpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\controls\text_block.hpp
//--------------------------------------------------------

// ReSharper disable once CppUnusedIncludeDirective

//--------------------------------------------------------
//-------begin of file: src\ui\controls\text_control.hpp
//--------------------------------------------------------

// ReSharper disable once CppUnusedIncludeDirective


namespace cru::ui::controls
{
    class TextControl : public Control
    {
    protected:
        TextControl(
            const Microsoft::WRL::ComPtr<IDWriteTextFormat>& init_text_format,
            const Microsoft::WRL::ComPtr<ID2D1Brush>& init_brush
        );
    public:
        TextControl(const TextControl& other) = delete;
        TextControl(TextControl&& other) = delete;
        TextControl& operator=(const TextControl& other) = delete;
        TextControl& operator=(TextControl&& other) = delete;
        ~TextControl() override = default;

        String GetText() const
        {
            return text_;
        }

        void SetText(const String& text);

        Microsoft::WRL::ComPtr<ID2D1Brush> GetBrush() const
        {
            return brush_;
        }

        void SetBrush(const Microsoft::WRL::ComPtr<ID2D1Brush>& brush);

        Microsoft::WRL::ComPtr<IDWriteTextFormat> GetTextFormat() const
        {
            return text_format_;
        }

        void SetTextFormat(const Microsoft::WRL::ComPtr<IDWriteTextFormat>& text_format);

        bool IsSelectable() const
        {
            return is_selectable_;
        }

        std::optional<TextRange> GetSelectedRange() const
        {
            return selected_range_;
        }

        void SetSelectedRange(std::optional<TextRange> text_range);

        void ClearSelection()
        {
            SetSelectedRange(std::nullopt);
        }

    protected:
        void SetSelectable(bool is_selectable);

        Size OnMeasureContent(const Size& available_size) override final;

        virtual void RequestChangeCaretPosition(unsigned position);

    private:
        void OnTextChangedCore(const String& old_text, const String& new_text);

        void RecreateTextLayout();

        // param point is the mouse point relative to this control.
        void UpdateCursor(const std::optional<Point>& point);

    private:
        String text_;

        Microsoft::WRL::ComPtr<ID2D1Brush> brush_;
        Microsoft::WRL::ComPtr<ID2D1Brush> selection_brush_;
        Microsoft::WRL::ComPtr<IDWriteTextFormat> text_format_;
    protected:
        Microsoft::WRL::ComPtr<IDWriteTextLayout> text_layout_;

    private:
        bool is_selectable_ = false;

        bool is_selecting_ = false;
        unsigned mouse_down_position_ = 0;
        std::optional<TextRange> selected_range_ = std::nullopt;
    };
}
//--------------------------------------------------------
//-------end of file: src\ui\controls\text_control.hpp
//--------------------------------------------------------

namespace cru::ui::controls
{
    class TextBlock : public TextControl
    {
    public:
        static constexpr auto control_type = L"TextBlock";

        static TextBlock* Create(const String& text = L"")
        {
            const auto text_block = new TextBlock();
            text_block->SetText(text);
            return text_block;
        }

    protected:
        TextBlock();
    public:
        TextBlock(const TextBlock& other) = delete;
        TextBlock(TextBlock&& other) = delete;
        TextBlock& operator=(const TextBlock& other) = delete;
        TextBlock& operator=(TextBlock&& other) = delete;
        ~TextBlock() override = default;

        StringView GetControlType() const override final;

        using TextControl::SetSelectable; // Make this public.
    };
}
//--------------------------------------------------------
//-------end of file: src\ui\controls\text_block.hpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\controls\toggle_button.hpp
//--------------------------------------------------------

// ReSharper disable once CppUnusedIncludeDirective


namespace cru::ui::controls
{
    class ToggleButton : public Control
    {
    public:
        static constexpr auto control_type = L"ToggleButton";

        static ToggleButton* Create()
        {
            return new ToggleButton();
        }

    protected:
        ToggleButton();

    public:
        ToggleButton(const ToggleButton& other) = delete;
        ToggleButton(ToggleButton&& other) = delete;
        ToggleButton& operator=(const ToggleButton& other) = delete;
        ToggleButton& operator=(ToggleButton&& other) = delete;
        ~ToggleButton() override = default;

        StringView GetControlType() const override final;

        bool IsPointInside(const Point& point) override;

        bool GetState() const
        {
            return state_;
        }

        void SetState(bool state);

        void Toggle();

        Event<events::ToggleEventArgs> toggle_event;

    protected:
        Size OnMeasureContent(const Size& available_size) override;

    private:
        bool state_ = false;

        float current_circle_position_;

        Microsoft::WRL::ComPtr<ID2D1RoundedRectangleGeometry> frame_path_;
        Microsoft::WRL::ComPtr<ID2D1Brush> on_brush_;
        Microsoft::WRL::ComPtr<ID2D1Brush> off_brush_;
    };
}
//--------------------------------------------------------
//-------end of file: src\ui\controls\toggle_button.hpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\controls\button.hpp
//--------------------------------------------------------

// ReSharper disable once CppUnusedIncludeDirective

#include <initializer_list>


namespace cru::ui::controls
{
    class Button : public Control
    {
    public:
        static constexpr auto control_type = L"Button";

        static Button* Create(const std::initializer_list<Control*>& children = std::initializer_list<Control*>())
        {
            const auto button = new Button();
            for (const auto control : children)
                button->AddChild(control);
            return button;
        }

    protected:
        Button();

    public:
        Button(const Button& other) = delete;
        Button(Button&& other) = delete;
        Button& operator=(const Button& other) = delete;
        Button& operator=(Button&& other) = delete;
        ~Button() override = default;

        StringView GetControlType() const override final;

    protected:
        void OnMouseClickBegin(MouseButton button) override final;
        void OnMouseClickEnd(MouseButton button) override final;

    private:
        BorderProperty normal_border_;
        BorderProperty pressed_border_;
    };
}
//--------------------------------------------------------
//-------end of file: src\ui\controls\button.hpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\controls\text_box.hpp
//--------------------------------------------------------

// ReSharper disable once CppUnusedIncludeDirective


namespace cru::ui::controls
{
    class TextBox : public TextControl
    {
    public:
        static constexpr auto control_type = L"TextBox";

        static TextBox* Create()
        {
            return new TextBox();
        }

    protected:
        TextBox();
    public:
        TextBox(const TextBox& other) = delete;
        TextBox(TextBox&& other) = delete;
        TextBox& operator=(const TextBox& other) = delete;
        TextBox& operator=(TextBox&& other) = delete;
        ~TextBox() override;

        StringView GetControlType() const override final;

    protected:
        void RequestChangeCaretPosition(unsigned position) override final;

    private:
        // return true if left
        bool GetCaretSelectionSide() const;
        void ShiftLeftSelectionRange(int count);
        void ShiftRightSelectionRange(int count);

    private:
        unsigned caret_position_ = 0;
        std::optional<TimerTask> caret_timer_{};
        Microsoft::WRL::ComPtr<ID2D1Brush> caret_brush_;
        bool is_caret_show_ = false;
    };
}
//--------------------------------------------------------
//-------end of file: src\ui\controls\text_box.hpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\controls\list_item.hpp
//--------------------------------------------------------

// ReSharper disable once CppUnusedIncludeDirective

#include <map>
#include <initializer_list>


namespace cru::ui::controls
{
    class ListItem : public Control
    {
    public:
        static constexpr auto control_type = L"ListItem";

        enum class State
        {
            Normal,
            Hover,
            Select
        };

    private:
        struct StateBrush
        {
            Microsoft::WRL::ComPtr<ID2D1Brush> border_brush;
            Microsoft::WRL::ComPtr<ID2D1Brush> fill_brush;
        };

    public:
        static ListItem* Create(const std::initializer_list<Control*>& children)
        {
            const auto list_item = new ListItem();
            for (auto control : children)
                list_item->AddChild(control);
            return list_item;
        }

    private:
        ListItem();
    public:
        ListItem(const ListItem& other) = delete;
        ListItem(ListItem&& other) = delete;
        ListItem& operator=(const ListItem& other) = delete;
        ListItem& operator=(ListItem&& other) = delete;
        ~ListItem() override = default;

        StringView GetControlType() const override;

        State GetState() const
        {
            return state_;
        }

        void SetState(State state);

    private:
        State state_ = State::Normal;
        std::map<State, StateBrush> brushes_{};
    };
}
//--------------------------------------------------------
//-------end of file: src\ui\controls\list_item.hpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\controls\popup_menu.hpp
//--------------------------------------------------------

// ReSharper disable once CppUnusedIncludeDirective

#include <vector>
#include <utility>
#include <functional>


namespace cru::ui
{
    class Window;
}

namespace cru::ui::controls
{
    using MenuItemInfo = std::pair<String, std::function<void()>>;

    Window* CreatePopupMenu(const Point& anchor, const std::vector<MenuItemInfo>& items, Window* parent = nullptr);
}
//--------------------------------------------------------
//-------end of file: src\ui\controls\popup_menu.hpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\controls\frame_layout.hpp
//--------------------------------------------------------

// ReSharper disable once CppUnusedIncludeDirective

#include <initializer_list>


namespace cru::ui::controls
{
    class FrameLayout : public Control
    {
    public:
        static constexpr auto control_type = L"FrameLayout";

        static FrameLayout* Create(const std::initializer_list<Control*>& children = std::initializer_list<Control*>{})
        {
            const auto layout = new FrameLayout();
            for (auto child : children)
                layout->AddChild(child);
            return layout;
        }

    protected:
        FrameLayout();
    public:
        FrameLayout(const FrameLayout& other) = delete;
        FrameLayout(FrameLayout&& other) = delete;
        FrameLayout& operator=(const FrameLayout& other) = delete;
        FrameLayout& operator=(FrameLayout&& other) = delete;
        ~FrameLayout() override;

        StringView GetControlType() const override final;
    };
}
//--------------------------------------------------------
//-------end of file: src\ui\controls\frame_layout.hpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\controls\scroll_control.hpp
//--------------------------------------------------------

// ReSharper disable once CppUnusedIncludeDirective

#include <optional>
#include <initializer_list>


namespace cru::ui::controls
{
    // Done: OnMeasureContent
    // Done: OnLayoutContent
    // Done: HitTest(no need)
    // Done: Draw(no need)
    // Done: API
    // Done: ScrollBar
    // Done: MouseEvent
    class ScrollControl : public Control
    {
    private:
        struct ScrollBarInfo
        {
            Rect border = Rect();
            Rect bar = Rect();
        };

        enum class Orientation
        {
            Horizontal,
            Vertical
        };

    public:
        enum class ScrollBarVisibility
        {
            None,
            Auto,
            Always
        };

        static ScrollControl* Create(const std::initializer_list<Control*>& children = std::initializer_list<Control*>{})
        {
            const auto control = new ScrollControl(true);
            for (auto child : children)
                control->AddChild(child);
            return control;
        }

        static constexpr auto control_type = L"ScrollControl";

    protected:
        explicit ScrollControl(bool container);
    public:
        ScrollControl(const ScrollControl& other) = delete;
        ScrollControl(ScrollControl&& other) = delete;
        ScrollControl& operator=(const ScrollControl& other) = delete;
        ScrollControl& operator=(ScrollControl&& other) = delete;
        ~ScrollControl() override;

        StringView GetControlType() const override final;

        bool IsHorizontalScrollEnabled() const
        {
            return horizontal_scroll_enabled_;
        }

        void SetHorizontalScrollEnabled(bool enable);

        bool IsVerticalScrollEnabled() const
        {
            return vertical_scroll_enabled_;
        }

        void SetVerticalScrollEnabled(bool enable);


        ScrollBarVisibility GetHorizontalScrollBarVisibility() const
        {
            return horizontal_scroll_bar_visibility_;
        }

        void SetHorizontalScrollBarVisibility(ScrollBarVisibility visibility);

        ScrollBarVisibility GetVerticalScrollBarVisibility() const
        {
            return vertical_scroll_bar_visibility_;
        }

        void SetVerticalScrollBarVisibility(ScrollBarVisibility visibility);

        float GetViewWidth() const
        {
            return view_width_;
        }

        float GetViewHeight() const
        {
            return view_height_;
        }

        float GetScrollOffsetX() const
        {
            return offset_x_;
        }

        float GetScrollOffsetY() const
        {
            return offset_y_;
        }

        // nullopt for not set. value is auto-coerced.
        void SetScrollOffset(std::optional<float> x, std::optional<float> y);

    protected:
        void SetViewWidth(float length);
        void SetViewHeight(float length);

        Size OnMeasureContent(const Size& available_size) override final;
        void OnLayoutContent(const Rect& rect) override final;

        void AfterLayoutSelf() override;

    private:
        void CoerceAndSetOffsets(float offset_x, float offset_y, bool update_children = true);
        void UpdateScrollBarVisibility();
        void UpdateScrollBarBorderInfo();
        void UpdateScrollBarBarInfo();

    private:
        bool horizontal_scroll_enabled_ = true;
        bool vertical_scroll_enabled_ = true;

        ScrollBarVisibility horizontal_scroll_bar_visibility_ = ScrollBarVisibility::Auto;
        ScrollBarVisibility vertical_scroll_bar_visibility_ = ScrollBarVisibility::Auto;

        bool is_horizontal_scroll_bar_visible_ = false;
        bool is_vertical_scroll_bar_visible_ = false;

        float offset_x_ = 0.0f;
        float offset_y_ = 0.0f;

        float view_width_ = 0.0f;
        float view_height_ = 0.0f;

        ScrollBarInfo horizontal_bar_info_;
        ScrollBarInfo vertical_bar_info_;

        std::optional<Orientation> is_pressing_scroll_bar_ = std::nullopt;
        float pressing_delta_ = 0.0f;
    };
}
//--------------------------------------------------------
//-------end of file: src\ui\controls\scroll_control.hpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\graph\graph.hpp
//--------------------------------------------------------

// ReSharper disable once CppUnusedIncludeDirective

#include <memory>
#include <functional>



namespace cru::graph
{
    class GraphManager;

    //Represents a window render target.
    class WindowRenderTarget : public Object
    {
    public:
        WindowRenderTarget(GraphManager* graph_manager, HWND hwnd);
        WindowRenderTarget(const WindowRenderTarget& other) = delete;
        WindowRenderTarget(WindowRenderTarget&& other) = delete;
        WindowRenderTarget& operator=(const WindowRenderTarget& other) = delete;
        WindowRenderTarget& operator=(WindowRenderTarget&& other) = delete;
        ~WindowRenderTarget() override;

    public:
        //Get the graph manager that created the render target.
        GraphManager* GetGraphManager() const
        {
            return graph_manager_;
        }

        //Get the d2d device context.
        inline Microsoft::WRL::ComPtr<ID2D1DeviceContext> GetD2DDeviceContext() const;

        //Get the target bitmap which can be set as the ID2D1DeviceContext's target.
        Microsoft::WRL::ComPtr<ID2D1Bitmap1> GetTargetBitmap() const
        {
            return target_bitmap_;
        }

        //Resize the underlying buffer.
        void ResizeBuffer(int width, int height);

        //Set this render target as the d2d device context's target.
        void SetAsTarget();

        //Present the data of the underlying buffer to the window.
        void Present();

    private:
        void CreateTargetBitmap();

    private:
        GraphManager* graph_manager_;
        Microsoft::WRL::ComPtr<IDXGISwapChain1> dxgi_swap_chain_;
        Microsoft::WRL::ComPtr<ID2D1Bitmap1> target_bitmap_;
    };

    struct Dpi
    {
        float x;
        float y;
    };

    class GraphManager final : public Object
    {
    public:
        static GraphManager* GetInstance();

    private:
        GraphManager();
    public:
        GraphManager(const GraphManager& other) = delete;
        GraphManager(GraphManager&& other) = delete;
        GraphManager& operator=(const GraphManager& other) = delete;
        GraphManager& operator=(GraphManager&& other) = delete;
        ~GraphManager() override;

    public:
        Microsoft::WRL::ComPtr<ID2D1Factory1> GetD2D1Factory() const
        {
            return d2d1_factory_;
        }

        Microsoft::WRL::ComPtr<ID2D1DeviceContext> GetD2D1DeviceContext() const
        {
            return d2d1_device_context_;
        }

        Microsoft::WRL::ComPtr<ID3D11Device> GetD3D11Device() const
        {
            return d3d11_device_;
        }

        Microsoft::WRL::ComPtr<IDXGIFactory2> GetDxgiFactory() const
        {
            return dxgi_factory_;
        }

        Microsoft::WRL::ComPtr<IDWriteFactory> GetDWriteFactory() const
        {
            return dwrite_factory_;
        }


        //Create a window render target with the HWND.
        std::shared_ptr<WindowRenderTarget> CreateWindowRenderTarget(HWND hwnd);

        //Get the desktop dpi.
        Dpi GetDpi() const;

        //Reload system metrics including desktop dpi.
        void ReloadSystemMetrics();

        Microsoft::WRL::ComPtr<IDWriteFontCollection> GetSystemFontCollection() const
        {
            return dwrite_system_font_collection_.Get();
        }

    private:
        Microsoft::WRL::ComPtr<ID3D11Device> d3d11_device_;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3d11_device_context_;
        Microsoft::WRL::ComPtr<ID2D1Factory1> d2d1_factory_;
        Microsoft::WRL::ComPtr<ID2D1Device> d2d1_device_;
        Microsoft::WRL::ComPtr<ID2D1DeviceContext> d2d1_device_context_;
        Microsoft::WRL::ComPtr<IDXGIFactory2> dxgi_factory_;

        Microsoft::WRL::ComPtr<IDWriteFactory> dwrite_factory_;
        Microsoft::WRL::ComPtr<IDWriteFontCollection> dwrite_system_font_collection_;
    };

    inline int DipToPixelInternal(const float dip, const float dpi)
    {
        return static_cast<int>(dip * dpi / 96.0f);
    }

    inline int DipToPixelX(const float dip_x)
    {
        return DipToPixelInternal(dip_x, GraphManager::GetInstance()->GetDpi().x);
    }

    inline int DipToPixelY(const float dip_y)
    {
        return DipToPixelInternal(dip_y, GraphManager::GetInstance()->GetDpi().y);
    }

    inline float DipToPixelInternal(const int pixel, const float dpi)
    {
        return static_cast<float>(pixel) * 96.0f / dpi;
    }

    inline float PixelToDipX(const int pixel_x)
    {
        return DipToPixelInternal(pixel_x, GraphManager::GetInstance()->GetDpi().x);
    }

    inline float PixelToDipY(const int pixel_y)
    {
        return DipToPixelInternal(pixel_y, GraphManager::GetInstance()->GetDpi().y);
    }

    Microsoft::WRL::ComPtr<ID2D1DeviceContext> WindowRenderTarget::GetD2DDeviceContext() const
    {
        return graph_manager_->GetD2D1DeviceContext();
    }

    inline void WithTransform(ID2D1DeviceContext* device_context, const D2D1_MATRIX_3X2_F matrix, const std::function<void(ID2D1DeviceContext*)>& action)
    {
        D2D1_MATRIX_3X2_F old_transform;
        device_context->GetTransform(&old_transform);
        device_context->SetTransform(old_transform * matrix);
        action(device_context);
        device_context->SetTransform(old_transform);
    }

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> CreateSolidColorBrush(const D2D1_COLOR_F& color);
}
//--------------------------------------------------------
//-------end of file: src\graph\graph.hpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\ui_manager.hpp
//--------------------------------------------------------

// ReSharper disable once CppUnusedIncludeDirective



namespace cru::graph
{
    class GraphManager;
}

namespace cru::ui
{
    struct CaretInfo
    {
        std::chrono::milliseconds caret_blink_duration;
        float half_caret_width;
    };


    class PredefineResources : public Object
    {
    public:
        explicit PredefineResources(graph::GraphManager* graph_manager);
        PredefineResources(const PredefineResources& other) = delete;
        PredefineResources(PredefineResources&& other) = delete;
        PredefineResources& operator=(const PredefineResources& other) = delete;
        PredefineResources& operator=(PredefineResources&& other) = delete;
        ~PredefineResources() override = default;

        //region BorderProperty
        Microsoft::WRL::ComPtr<ID2D1Brush> border_property_brush;

        //region Button
        BorderProperty button_normal_border;
        BorderProperty button_press_border;

        //region TextControl
        Microsoft::WRL::ComPtr<ID2D1Brush> text_control_selection_brush;

        //region TextBox
        BorderProperty text_box_border;
        Microsoft::WRL::ComPtr<ID2D1Brush> text_box_text_brush;
        Microsoft::WRL::ComPtr<IDWriteTextFormat> text_box_text_format;
        Microsoft::WRL::ComPtr<ID2D1Brush> text_box_caret_brush;

        //region TextBlock
        Microsoft::WRL::ComPtr<ID2D1Brush> text_block_text_brush;
        Microsoft::WRL::ComPtr<IDWriteTextFormat> text_block_text_format;

        //region ToggleButton
        Microsoft::WRL::ComPtr<ID2D1Brush> toggle_button_on_brush;
        Microsoft::WRL::ComPtr<ID2D1Brush> toggle_button_off_brush;

        //region ListItem
        Microsoft::WRL::ComPtr<ID2D1Brush> list_item_normal_border_brush;
        Microsoft::WRL::ComPtr<ID2D1Brush> list_item_normal_fill_brush;
        Microsoft::WRL::ComPtr<ID2D1Brush> list_item_hover_border_brush;
        Microsoft::WRL::ComPtr<ID2D1Brush> list_item_hover_fill_brush;
        Microsoft::WRL::ComPtr<ID2D1Brush> list_item_select_border_brush;
        Microsoft::WRL::ComPtr<ID2D1Brush> list_item_select_fill_brush;

        //region ScrollControl
        Microsoft::WRL::ComPtr<ID2D1Brush> scroll_bar_background_brush;
        Microsoft::WRL::ComPtr<ID2D1Brush> scroll_bar_border_brush;
        Microsoft::WRL::ComPtr<ID2D1Brush> scroll_bar_brush;

#ifdef CRU_DEBUG_LAYOUT
        //region debug
        Microsoft::WRL::ComPtr<ID2D1Brush> debug_layout_out_border_brush;
        Microsoft::WRL::ComPtr<ID2D1Brush> debug_layout_margin_brush;
        Microsoft::WRL::ComPtr<ID2D1Brush> debug_layout_padding_brush;
#endif
    };

    class UiManager : public Object
    {
    public:
        static UiManager* GetInstance();
    private:
        UiManager();
    public:
        UiManager(const UiManager& other) = delete;
        UiManager(UiManager&& other) = delete;
        UiManager& operator=(const UiManager& other) = delete;
        UiManager& operator=(UiManager&& other) = delete;
        ~UiManager() override = default;

        CaretInfo GetCaretInfo() const
        {
            return caret_info_;
        }

        const PredefineResources* GetPredefineResources() const
        {
            return &predefine_resources_;
        }

    private:
        CaretInfo caret_info_;

        PredefineResources predefine_resources_;
    };
}
//--------------------------------------------------------
//-------end of file: src\ui\ui_manager.hpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\convert_util.hpp
//--------------------------------------------------------

// ReSharper disable once CppUnusedIncludeDirective



namespace cru::ui
{
    inline D2D1_POINT_2F Convert(const Point& point)
    {
        return D2D1::Point2F(point.x, point.y);
    }

    inline D2D1_RECT_F Convert(const Rect& rect)
    {
        return D2D1::RectF(rect.left, rect.top, rect.left + rect.width, rect.top + rect.height);
    }
}
//--------------------------------------------------------
//-------end of file: src\ui\convert_util.hpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\math_util.hpp
//--------------------------------------------------------

// ReSharper disable once CppUnusedIncludeDirective

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
//--------------------------------------------------------
//-------end of file: src\math_util.hpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\animations\animation.hpp
//--------------------------------------------------------

// ReSharper disable once CppUnusedIncludeDirective

#include <unordered_map>


namespace cru::ui::animations
{
    using AnimationTimeUnit = FloatSecond;

    struct IAnimationDelegate : virtual Interface
    {
        virtual void Cancel() = 0;
    };

    using AnimationDelegatePtr = std::shared_ptr<IAnimationDelegate>;

    using AnimationStepHandler = std::function<void(AnimationDelegatePtr, double)>;
    using AnimationStartHandler = std::function<void(AnimationDelegatePtr)>;
    using AnimationFinishHandler = std::function<void()>;
    using AnimationCancelHandler = std::function<void()>;

    namespace details
    {
        class Animation;
        using AnimationPtr = std::unique_ptr<Animation>;

        class AnimationInfo
        {
        public:
            AnimationInfo(String tag, const AnimationTimeUnit duration)
                : tag(std::move(tag)),
                  duration(duration)
            {

            }
            AnimationInfo(const AnimationInfo& other) = default;
            AnimationInfo(AnimationInfo&& other) = default;
            AnimationInfo& operator=(const AnimationInfo& other) = default;
            AnimationInfo& operator=(AnimationInfo&& other) = default;
            ~AnimationInfo() = default;

            String tag;
            AnimationTimeUnit duration;
            std::vector<AnimationStepHandler> step_handlers{};
            std::vector<AnimationStartHandler> start_handlers{};
            std::vector<AnimationFinishHandler> finish_handlers{};
            std::vector<AnimationCancelHandler> cancel_handlers{};
        };

        class AnimationManager : public Object
        {
        public:
            static AnimationManager* GetInstance();
        private:
            AnimationManager();
        public:
            AnimationManager(const AnimationManager& other) = delete;
            AnimationManager(AnimationManager&& other) = delete;
            AnimationManager& operator=(const AnimationManager& other) = delete;
            AnimationManager& operator=(AnimationManager&& other) = delete;
            ~AnimationManager() override;

            AnimationDelegatePtr CreateAnimation(AnimationInfo info);
            void RemoveAnimation(const String& tag);

        private:
            void SetTimer();
            void KillTimer();

        private:
            std::unordered_map<String, AnimationPtr> animations_;
            std::optional<TimerTask> timer_;
        };
    }

    class AnimationBuilder : public Object
    {
    public:
        AnimationBuilder(String tag, const AnimationTimeUnit duration)
            : info_(std::move(tag), duration)
        {

        }

        AnimationBuilder& AddStepHandler(const AnimationStepHandler& handler)
        {
            CheckValid();
            info_.step_handlers.push_back(handler);
            return *this;
        }

        AnimationBuilder& AddStartHandler(const AnimationStartHandler& handler)
        {
            CheckValid();
            info_.start_handlers.push_back(handler);
            return *this;
        }

        AnimationBuilder& AddFinishHandler(const AnimationFinishHandler& handler)
        {
            CheckValid();
            info_.finish_handlers.push_back(handler);
            return *this;
        }

        AnimationBuilder& AddCancelHandler(const AnimationCancelHandler& handler)
        {
            CheckValid();
            info_.cancel_handlers.push_back(handler);
            return *this;
        }

        AnimationDelegatePtr Start();

    private:
        void CheckValid() const
        {
            if (!valid_)
                throw std::runtime_error("The animation builder is invalid.");
        }

        bool valid_ = true;
        details::AnimationInfo info_;
    };
}
//--------------------------------------------------------
//-------end of file: src\ui\animations\animation.hpp
//--------------------------------------------------------
