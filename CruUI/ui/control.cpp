#include "control.h"

#include <string>
#include <algorithm>
#include <chrono>

#include "window.h"
#include "timer.h"
#include "debug_base.h"

namespace cru {
    namespace ui {
        using namespace events;

        Control::Control(const bool container) :
            is_container_(container),
            window_(nullptr),
            parent_(nullptr),
            children_(),
            old_position_(),
            position_(),
            size_(),
            position_cache_(),
            is_mouse_inside_(false),
            is_mouse_leave_{
                { MouseButton::Left, true },
                { MouseButton::Middle, true },
                { MouseButton::Right, true }
            },
            layout_params_(new BasicLayoutParams()),
            desired_size_()
        {

        }

        Control::Control(WindowConstructorTag, Window* window) : Control(true)
        {
            window_ = window;
        }

        Control::~Control()
        {
            ForeachChild([](auto control)
            {
                delete control;
            });
        }

        void Control::ForeachChild(Function<void(Control*)>&& predicate) const
        {
            if (is_container_)
                for (const auto child : children_)
                    predicate(child);
        }

        void Control::ForeachChild(Function<FlowControl(Control*)>&& predicate) const
        {
            if (is_container_)
                for (const auto child : children_)
                {
                    if (predicate(child) == FlowControl::Break)
                        break;
                }
        }

        void AddChildCheck(Control* control)
        {
            if (control->GetParent() != nullptr)
                throw std::invalid_argument("The control already has a parent.");

            if (dynamic_cast<Window*>(control))
                throw std::invalid_argument("Can't add a window as child.");
        }

        void Control::AddChild(Control* control)
        {
            ThrowIfNotContainer();
            AddChildCheck(control);

            this->children_.push_back(control);

            control->parent_ = this;

            this->OnAddChild(control);
        }

        void Control::AddChild(Control* control, int position)
        {
            ThrowIfNotContainer();
            AddChildCheck(control);

            if (position < 0 || static_cast<decltype(this->children_.size())>(position) > this->children_.size())
                throw std::invalid_argument("The position is out of range.");

            this->children_.insert(this->children_.cbegin() + position, control);

            control->parent_ = this;

            this->OnAddChild(this);
        }

        void Control::RemoveChild(Control* child)
        {
            ThrowIfNotContainer();
            const auto i = std::find(this->children_.cbegin(), this->children_.cend(), child);
            if (i == this->children_.cend())
                throw std::invalid_argument("The argument child is not a child of this control.");

            this->children_.erase(i);

            child->parent_ = nullptr;

            this->OnRemoveChild(this);
        }

        void Control::RemoveChild(const int position)
        {
            ThrowIfNotContainer();
            if (position < 0 || static_cast<decltype(this->children_.size())>(position) >= this->children_.size())
                throw std::invalid_argument("The position is out of range.");

            const auto p = children_.cbegin() + position;
            const auto child = *p;
            children_.erase(p);

            child->parent_ = nullptr;

            this->OnRemoveChild(child);
        }

        Control* Control::GetAncestor()
        {
            // if attached to window, the window is the ancestor.
            if (window_)
                return window_;

            // otherwise find the ancestor
            auto ancestor = this;
            while (const auto parent = ancestor->GetParent())
                ancestor = parent;
            return ancestor;
        }

        void TraverseDescendantsInternal(Control* control, Function<void(Control*)>& predicate)
        {
            predicate(control);
            control->ForeachChild([&predicate](Control* c) {
                TraverseDescendantsInternal(c, predicate);
            });
        }

        void Control::TraverseDescendants(Function<void(Control*)>&& predicate)
        {
            if (is_container_)
                TraverseDescendantsInternal(this, predicate);
            else
                predicate(this);
        }

        Point Control::GetPositionRelative()
        {
            return position_;
        }

        void Control::SetPositionRelative(const Point & position)
        {
            if (position != position_)
            {
                if (old_position_ == position) // if cache has been refreshed and no pending notify
                    old_position_ = position_;
                position_ = position;
                LayoutManager::GetInstance()->InvalidateControlPositionCache(this);
                if (auto window = GetWindow())
                {
                    window->Repaint();
                }
            }
        }

        Size Control::GetSize()
        {
            return size_;
        }

        void Control::SetSize(const Size & size)
        {
            const auto old_size = size_;
            size_ = size;
            SizeChangedEventArgs args(this, this, old_size, size);
            RaiseSizeChangedEvent(args);
            if (auto window = GetWindow())
                window->Repaint();
        }

        Point Control::GetPositionAbsolute() const
        {
            return position_cache_.lefttop_position_absolute;
        }

        Point Control::LocalToAbsolute(const Point& point) const
        {
            return Point(point.x + position_cache_.lefttop_position_absolute.x,
                point.y + position_cache_.lefttop_position_absolute.y);
        }

        Point Control::AbsoluteToLocal(const Point & point) const
        {
            return Point(point.x - position_cache_.lefttop_position_absolute.x,
                point.y - position_cache_.lefttop_position_absolute.y);
        }

        bool Control::IsPointInside(const Point & point)
        {
            const auto size = GetSize();
            return point.x >= 0.0f && point.x < size.width && point.y >= 0.0f && point.y < size.height;
        }

        void Control::Draw(ID2D1DeviceContext* device_context)
        {
            D2D1::Matrix3x2F old_transform;
            device_context->GetTransform(&old_transform);

            const auto position = GetPositionRelative();
            device_context->SetTransform(old_transform * D2D1::Matrix3x2F::Translation(position.x, position.y));

            OnDraw(device_context);
            DrawEventArgs args(this, this, device_context);
            draw_event.Raise(args);

            for (auto child : GetChildren())
                child->Draw(device_context);

            device_context->SetTransform(old_transform);
        }

        void Control::Repaint()
        {
            if (window_ != nullptr)
                window_->Repaint();
        }

        bool Control::RequestFocus()
        {
            auto window = GetWindow();
            if (window == nullptr)
                return false;

            return window->RequestFocusFor(this);
        }

        bool Control::HasFocus()
        {
            auto window = GetWindow();
            if (window == nullptr)
                return false;

            return window->GetFocusControl() == this;
        }

        void Control::Relayout()
        {
            OnMeasure(GetSize());
            OnLayout(Rect(GetPositionRelative(), GetSize()));
        }

        void Control::Measure(const Size& available_size)
        {
            SetDesiredSize(OnMeasure(available_size));
        }

        void Control::Layout(const Rect& rect)
        {
            SetPositionRelative(rect.GetLeftTop());
            SetSize(rect.GetSize());
            OnLayout(rect);
        }

        Size Control::GetDesiredSize() const
        {
            return desired_size_;
        }

        void Control::SetDesiredSize(const Size& desired_size)
        {
            desired_size_ = desired_size;
        }

        void Control::OnAddChild(Control* child)
        {
            if (auto window = GetWindow())
            {
                child->TraverseDescendants([window](Control* control) {
                    control->OnAttachToWindow(window);
                });
                window->RefreshControlList();
            }
            Relayout();
        }

        void Control::OnRemoveChild(Control* child)
        {
            if (auto window = GetWindow())
            {
                child->TraverseDescendants([window](Control* control) {
                    control->OnDetachToWindow(window);
                });
                window->RefreshControlList();
            }
            Relayout();
        }

        void Control::OnAttachToWindow(Window* window)
        {
            window_ = window;
        }

        void Control::OnDetachToWindow(Window * window)
        {
            window_ = nullptr;
        }

        void Control::OnDraw(ID2D1DeviceContext * device_context)
        {
#ifdef CRU_DEBUG
            auto brush = Application::GetInstance()->GetDebugBorderBrush();
            const auto size = GetSize();
            device_context->DrawRectangle(D2D1::RectF(0, 0, size.width, size.height), brush.Get());
#endif
        }

        void Control::OnPositionChanged(PositionChangedEventArgs & args)
        {

        }

        void Control::OnSizeChanged(SizeChangedEventArgs & args)
        {
        }

        void Control::OnPositionChangedCore(PositionChangedEventArgs & args)
        {

        }

        void Control::OnSizeChangedCore(SizeChangedEventArgs & args)
        {

        }

        void Control::RaisePositionChangedEvent(PositionChangedEventArgs& args)
        {
            OnPositionChangedCore(args);
            OnPositionChanged(args);
            position_changed_event.Raise(args);
        }

        void Control::RaiseSizeChangedEvent(SizeChangedEventArgs& args)
        {
            OnSizeChangedCore(args);
            OnSizeChanged(args);
            size_changed_event.Raise(args);
        }

        void Control::OnMouseEnter(MouseEventArgs & args)
        {
        }

        void Control::OnMouseLeave(MouseEventArgs & args)
        {
        }

        void Control::OnMouseMove(MouseEventArgs & args)
        {
        }

        void Control::OnMouseDown(MouseButtonEventArgs & args)
        {
        }

        void Control::OnMouseUp(MouseButtonEventArgs & args)
        {
        }

        void Control::OnMouseClick(MouseButtonEventArgs& args)
        {

        }

        void Control::OnMouseEnterCore(MouseEventArgs & args)
        {
            is_mouse_inside_ = true;
        }

        void Control::OnMouseLeaveCore(MouseEventArgs & args)
        {
            is_mouse_inside_ = false;
            for (auto& is_mouse_leave : is_mouse_leave_)
                is_mouse_leave.second = true;
        }

        void Control::OnMouseMoveCore(MouseEventArgs & args)
        {

        }

        void Control::OnMouseDownCore(MouseButtonEventArgs & args)
        {
            is_mouse_leave_[args.GetMouseButton()] = false;
        }

        void Control::OnMouseUpCore(MouseButtonEventArgs & args)
        {
            if (!is_mouse_leave_[args.GetMouseButton()])
                RaiseMouseClickEvent(args);
        }

        void Control::OnMouseClickCore(MouseButtonEventArgs& args)
        {

        }

        void Control::RaiseMouseEnterEvent(MouseEventArgs& args)
        {
            OnMouseEnterCore(args);
            OnMouseEnter(args);
            mouse_enter_event.Raise(args);
        }

        void Control::RaiseMouseLeaveEvent(MouseEventArgs& args)
        {
            OnMouseLeaveCore(args);
            OnMouseLeave(args);
            mouse_leave_event.Raise(args);
        }

        void Control::RaiseMouseMoveEvent(MouseEventArgs& args)
        {
            OnMouseMoveCore(args);
            OnMouseMove(args);
            mouse_move_event.Raise(args);
        }

        void Control::RaiseMouseDownEvent(MouseButtonEventArgs& args)
        {
            OnMouseDownCore(args);
            OnMouseDown(args);
            mouse_down_event.Raise(args);
        }

        void Control::RaiseMouseUpEvent(MouseButtonEventArgs& args)
        {
            OnMouseUpCore(args);
            OnMouseUp(args);
            mouse_up_event.Raise(args);
        }

        void Control::RaiseMouseClickEvent(MouseButtonEventArgs& args)
        {
            OnMouseClickCore(args);
            OnMouseClick(args);
            mouse_click_event.Raise(args);
        }

        void Control::OnGetFocus(FocusChangeEventArgs& args)
        {

        }

        void Control::OnLoseFocus(FocusChangeEventArgs& args)
        {

        }

        void Control::OnGetFocusCore(FocusChangeEventArgs& args)
        {

        }

        void Control::OnLoseFocusCore(FocusChangeEventArgs& args)
        {

        }

        void Control::RaiseGetFocusEvent(FocusChangeEventArgs& args)
        {
            OnGetFocusCore(args);
            OnGetFocus(args);
            get_focus_event.Raise(args);
        }

        void Control::RaiseLoseFocusEvent(FocusChangeEventArgs& args)
        {
            OnLoseFocusCore(args);
            OnLoseFocus(args);
            lose_focus_event.Raise(args);
        }

        Size Control::OnMeasure(const Size& available_size)
        {
            const auto layout_params = GetLayoutParams();

            if (!layout_params->Validate())
                throw std::runtime_error("LayoutParams is not valid. Please check it.");

            auto&& get_available_length_for_child = [](const MeasureLength& layout_length, const float available_length) -> float
            {
                switch (layout_length.mode)
                {
                case MeasureMode::Exactly:
                {
                    return std::min(layout_length.length, available_length);
                }
                case MeasureMode::Stretch:
                case MeasureMode::Content:
                {
                    return available_length;
                }
                default:
                    UnreachableCode();
                }
            };

            const Size size_for_children(get_available_length_for_child(layout_params->width, available_size.width),
                get_available_length_for_child(layout_params->height, available_size.height));

            auto max_child_size = Size::Zero();
            ForeachChild([&](Control* control)
            {
                control->Measure(size_for_children);
                const auto&& size = control->GetDesiredSize();
                if (max_child_size.width < size.width)
                    max_child_size.width = size.width;
                if (max_child_size.height < size.height)
                    max_child_size.height = size.height;
            });

            auto&& calculate_final_length = [](const MeasureLength& layout_length, const float length_for_children, const float max_child_length) -> float
            {
                switch (layout_length.mode)
                {
                case MeasureMode::Exactly:
                case MeasureMode::Stretch:
                    return length_for_children;
                case MeasureMode::Content:
                    return max_child_length;
                default:
                    UnreachableCode();
                }
            };

            return Size(
                calculate_final_length(layout_params->width, size_for_children.width, max_child_size.width),
                calculate_final_length(layout_params->height, size_for_children.height, max_child_size.height)
            );
        }

        void Control::OnLayout(const Rect& rect)
        {
            ForeachChild([](Control* control)
            {
                control->Layout(Rect(Point::Zero(), control->GetDesiredSize()));
            });
        }

        void Control::CheckAndNotifyPositionChanged()
        {
            if (this->old_position_ != this->position_)
            {
                PositionChangedEventArgs args(this, this, this->old_position_, this->position_);
                this->RaisePositionChangedEvent(args);
                this->old_position_ = this->position_;
            }
        }

        std::list<Control*> GetAncestorList(Control* control)
        {
            std::list<Control*> l;
            while (control != nullptr)
            {
                l.push_front(control);
                control = control->GetParent();
            }
            return l;
        }

        Control* FindLowestCommonAncestor(Control * left, Control * right)
        {
            if (left == nullptr || right == nullptr)
                return nullptr;

            auto&& left_list = GetAncestorList(left);
            auto&& right_list = GetAncestorList(right);

            // the root is different
            if (left_list.front() != right_list.front())
                return nullptr;

            // find the last same control or the last control (one is the other's ancestor)
            auto left_i = left_list.cbegin();
            auto right_i = right_list.cbegin();
            while (true)
            {
                if (left_i == left_list.cend())
                    return *(--left_i);
                if (right_i == right_list.cend())
                    return *(--right_i);
                if (*left_i != *right_i)
                    return *(--left_i);
                ++left_i;
                ++right_i;
            }
        }

        Control * IsAncestorOrDescendant(Control * left, Control * right)
        {
            //Search up along the trunk from "left". Return if find "right".
            auto control = left;
            while (control != nullptr)
            {
                if (control == right)
                    return control;
                control = control->GetParent();
            }
            //Search up along the trunk from "right". Return if find "left".
            control = right;
            while (control != nullptr)
            {
                if (control == left)
                    return control;
                control = control->GetParent();
            }
            return nullptr;
        }
    }
}
