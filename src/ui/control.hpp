#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "pre.hpp"

#include "system_headers.hpp"
#include <unordered_map>
#include <any>
#include <utility>

#include "base.hpp"
#include "ui_base.hpp"
#include "layout_base.hpp"
#include "events/ui_event.hpp"
#include "border_property.hpp"
#include "cursor.hpp"
#include "any_map.hpp"

namespace cru::ui
{
    class Control;
    class Window;


    struct AdditionalMeasureInfo
    {
        bool horizontal_stretchable = true;
        bool vertical_stretchable = true;
    };

    struct AdditionalLayoutInfo
    {
        Point total_offset = Point::Zero();
    };

    //the position cache
    struct ControlPositionCache
    {
        //The lefttop relative to the ancestor.
        Point lefttop_position_absolute = Point::Zero();
    };


    class Control : public Object
    {
        friend class Window;

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

        //Get the lefttop relative to its parent.
        virtual Point GetPositionRelative();

        //Get the actual size.
        virtual Size GetSize();

        virtual void SetRect(const Rect& rect);

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

        void Measure(const Size& available_size, const AdditionalMeasureInfo& additional_info);

        void Layout(const Rect& rect, const AdditionalLayoutInfo& additional_info);

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
        virtual void OnRectChange(const Rect& old_rect, const Rect& new_rect);
        
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
        Size OnMeasureCore(const Size& available_size, const AdditionalMeasureInfo& additional_info);
        void OnLayoutCore(const Rect& rect, const AdditionalLayoutInfo& additional_info);

    protected:
        virtual Size OnMeasureContent(const Size& available_size, const AdditionalMeasureInfo& additional_info);
        virtual void OnLayoutContent(const Rect& rect, const AdditionalLayoutInfo& additional_info);


    private:
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

        Rect rect_{};
        
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


    //*************** region: create helper ***************

    template <typename TControl, typename... Args>
    TControl* CreateWithLayout(const LayoutSideParams& width, const LayoutSideParams& height, Args&&... args)
    {
        static_assert(std::is_base_of_v<Control, TControl>, "TControl is not a control class.");
        TControl* control = TControl::Create(std::forward<Args>(args)...);
        control->GetLayoutParams()->width = width;
        control->GetLayoutParams()->height = height;
        return control;
    }

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
