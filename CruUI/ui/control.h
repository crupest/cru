#pragma once

#include "system_headers.h"
#include <unordered_map>
#include <any>
#include <typeinfo>
#include <fmt/format.h>

#include "base.h"
#include "ui_base.h"
#include "layout_base.h"
#include "events/ui_event.h"

namespace cru
{
    namespace ui
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

            bool IsContainer() const
            {
                return is_container_;
            }

            //Get parent of control, return nullptr if it has no parent.
            Control* GetParent() const
            {
                return parent_;
            }

            //Traverse the children
            void ForeachChild(Function<void(Control*)>&& predicate) const;
            void ForeachChild(Function<FlowControl(Control*)>&& predicate) const;

            //Return a vector of all children. This function will create a
            //temporary copy of vector of children. If you just want to
            //traverse all children, just call ForeachChild.
            Vector<Control*> GetChildren() const
            {
                return children_;
            }

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
            void TraverseDescendants(Function<void(Control*)>&& predicate);

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
            Point LocalToAbsolute(const Point& point) const;

            //Absolute point to local point.
            Point AbsoluteToLocal(const Point& point) const;

            virtual bool IsPointInside(const Point& point);


            //*************** region: graphic ***************

            //Draw this control and its child controls.
            void Draw(ID2D1DeviceContext* device_context);

            virtual void Repaint();

            //*************** region: focus ***************

            bool RequestFocus();

            bool HasFocus();


            //*************** region: layout ***************

            void Relayout();

            void Measure(const Size& available_size);

            void Layout(const Rect& rect);

            Size GetDesiredSize() const;

            void SetDesiredSize(const Size& desired_size);

            BasicLayoutParams* GetLayoutParams()
            {
                return &layout_params_;
            }

            //*************** region: additional properties ***************
            template <typename T>
            std::optional<T> GetAdditionalProperty(const String& key)
            {
                try
                {
                    const auto find_result = additional_properties_.find(key);
                    if (find_result != additional_properties_.cend())
                        return std::any_cast<T>(find_result->second);
                    else
                        return std::nullopt;
                }
                catch (const std::bad_any_cast&)
                {
                    throw std::runtime_error(fmt::format("Key \"{}\" is not of the type {}.", ToUtf8String(key), typeid(T).name()));
                }
            }

            template <typename T>
            void SetAdditionalProperty(const String& key, const T& value)
            {
                additional_properties_[key] = std::make_any<T>(value);
            }

            template <typename T>
            void SetAdditionalProperty(const String& key, T&& value)
            {
                additional_properties_[key] = std::make_any<T>(std::move(value));
            }

            //*************** region: events ***************
            //Raised when mouse enter the control.
            events::MouseEvent mouse_enter_event;
            //Raised when mouse is leave the control.
            events::MouseEvent mouse_leave_event;
            //Raised when mouse is move in the control.
            events::MouseEvent mouse_move_event;
            //Raised when a mouse button is pressed in the control.
            events::MouseButtonEvent mouse_down_event;
            //Raised when a mouse button is released in the control.
            events::MouseButtonEvent mouse_up_event;
            //Raised when a mouse button is pressed in the control and released in the control with mouse not leaving it between two operations.
            events::MouseButtonEvent mouse_click_event;

            events::FocusChangeEvent get_focus_event;
            events::FocusChangeEvent lose_focus_event;

            events::DrawEvent draw_event;

            events::PositionChangedEvent position_changed_event;
            events::SizeChangedEvent size_changed_event;

        protected:
            //Invoked when a child is added. Overrides should invoke base.
            virtual void OnAddChild(Control* child);
            //Invoked when a child is removed. Overrides should invoke base.
            virtual void OnRemoveChild(Control* child);

            //Invoked when the control is attached to a window. Overrides should invoke base.
            virtual void OnAttachToWindow(Window* window);
            //Invoked when the control is detached to a window. Overrides should invoke base.
            virtual void OnDetachToWindow(Window* window);

            virtual void OnDraw(ID2D1DeviceContext* device_context);


            // For a event, the window event system will first dispatch event to core functions.
            // Therefore for particular controls, you should do essential actions in core functions,
            // and override version should invoke base version. The base core function
            // in "Control" class will call corresponding non-core function and call "Raise" on
            // event objects. So user custom actions should be done by overriding non-core function
            // and calling the base version is optional.

            //*************** region: position and size event ***************
            virtual void OnPositionChanged(events::PositionChangedEventArgs& args);
            virtual void OnSizeChanged(events::SizeChangedEventArgs& args);

            virtual void OnPositionChangedCore(events::PositionChangedEventArgs& args);
            virtual void OnSizeChangedCore(events::SizeChangedEventArgs& args);

            void RaisePositionChangedEvent(events::PositionChangedEventArgs& args);
            void RaiseSizeChangedEvent(events::SizeChangedEventArgs& args);

            //*************** region: mouse event ***************
            virtual void OnMouseEnter(events::MouseEventArgs& args);
            virtual void OnMouseLeave(events::MouseEventArgs& args);
            virtual void OnMouseMove(events::MouseEventArgs& args);
            virtual void OnMouseDown(events::MouseButtonEventArgs& args);
            virtual void OnMouseUp(events::MouseButtonEventArgs& args);
            virtual void OnMouseClick(events::MouseButtonEventArgs& args);

            virtual void OnMouseEnterCore(events::MouseEventArgs& args);
            virtual void OnMouseLeaveCore(events::MouseEventArgs& args);
            virtual void OnMouseMoveCore(events::MouseEventArgs& args);
            virtual void OnMouseDownCore(events::MouseButtonEventArgs& args);
            virtual void OnMouseUpCore(events::MouseButtonEventArgs& args);
            virtual void OnMouseClickCore(events::MouseButtonEventArgs& args);

            void RaiseMouseEnterEvent(events::MouseEventArgs& args);
            void RaiseMouseLeaveEvent(events::MouseEventArgs& args);
            void RaiseMouseMoveEvent(events::MouseEventArgs& args);
            void RaiseMouseDownEvent(events::MouseButtonEventArgs& args);
            void RaiseMouseUpEvent(events::MouseButtonEventArgs& args);
            void RaiseMouseClickEvent(events::MouseButtonEventArgs& args);

            //*************** region: focus event ***************
            virtual void OnGetFocus(events::FocusChangeEventArgs& args);
            virtual void OnLoseFocus(events::FocusChangeEventArgs& args);

            virtual void OnGetFocusCore(events::FocusChangeEventArgs& args);
            virtual void OnLoseFocusCore(events::FocusChangeEventArgs& args);

            void RaiseGetFocusEvent(events::FocusChangeEventArgs& args);
            void RaiseLoseFocusEvent(events::FocusChangeEventArgs& args);

            //*************** region: layout ***************
            virtual Size OnMeasure(const Size& available_size);
            virtual void OnLayout(const Rect& rect);

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

        protected:
            Window * window_ = nullptr; // protected for Window class to write it as itself in constructor.

        private:
            Control * parent_ = nullptr;
            Vector<Control*> children_{};

            // When position is changed and notification hasn't been
            // sent, it will be the old position. When position is changed
            // more than once, it will be the oldest position since last
            // notification. If notification has been sent, it will be updated
            // to position_.
            Point old_position_ = Point::Zero();
            Point position_ = Point::Zero();
            Size size_ = Size::Zero();

            ControlPositionCache position_cache_{};

            bool is_mouse_inside_ = false;

            std::unordered_map<MouseButton, bool> is_mouse_leave_
            {
                { MouseButton::Left, true },
                { MouseButton::Middle, true },
                { MouseButton::Right, true }
            }; // used for clicking determination

            BasicLayoutParams layout_params_{};
            Size desired_size_ = Size::Zero();

            std::unordered_map<String, std::any> additional_properties_{};
        };

        // Find the lowest common ancestor.
        // Return nullptr if "left" and "right" are not in the same tree.
        Control* FindLowestCommonAncestor(Control* left, Control* right);

        // Return the ancestor if one control is the ancestor of the other one, otherwise nullptr.
        Control* IsAncestorOrDescendant(Control* left, Control* right);
    }
}
