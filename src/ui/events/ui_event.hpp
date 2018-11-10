#pragma once

#include "system_headers.hpp"
#include <optional>

#include "base.hpp"
#include "cru_event.hpp"
#include "ui/ui_base.hpp"
#include "ui/layout_base.hpp"

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
            : BasicEventArgs(sender), original_sender_(original_sender)
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

    private:
        Object* original_sender_;
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

    using UiEvent = Event<UiEventArgs>;
    using MouseEvent = Event<MouseEventArgs>;
    using MouseButtonEvent = Event<MouseButtonEventArgs>;
    using DrawEvent = Event<DrawEventArgs>;
    using PositionChangedEvent = Event<PositionChangedEventArgs>;
    using SizeChangedEvent = Event<SizeChangedEventArgs>;
    using FocusChangeEvent = Event<FocusChangeEventArgs>;
    using ToggleEvent = Event<ToggleEventArgs>;
    using WindowNativeMessageEvent = Event<WindowNativeMessageEventArgs>;
    using KeyEvent = Event<KeyEventArgs>;
    using CharEvent = Event<CharEventArgs>;
}