#include "control.h"

#include <algorithm>

#include "window.h"

namespace cru {
    namespace ui {
        using namespace events;

        Control::Control() :
            window_(nullptr),
            parent_(nullptr),
            position_(Point::zero),
            size_(Size::zero),
            is_mouse_inside_(false),
            layout_params_(nullptr),
            desired_size_(Size::zero)
        {

        }

        void Control::ForeachChild(Action<Control*>&& predicate)
        {
            for (const auto child : children_)
                predicate(child);
        }

        void Control::ForeachChild(FlowControlAction<Control*>&& predicate)
        {
            for (const auto child : children_)
            {
                if (predicate(child) == FlowControl::Break)
                    break;
            }
        }

        std::vector<Control*> Control::GetChildren()
        {
            return this->children_;
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
            AddChildCheck(control);

            this->children_.push_back(control);

            control->parent_ = this;

            this->OnAddChild(control);
        }

        void Control::AddChild(Control* control, int position)
        {
            AddChildCheck(control);

            if (position < 0 || static_cast<decltype(this->children_.size())>(position) > this->children_.size())
                throw std::invalid_argument("The position is out of range.");

            this->children_.insert(this->children_.cbegin() + position, control);

            control->parent_ = this;

            this->OnAddChild(this);
        }

        void Control::RemoveChild(Control* child)
        {
            const auto i = std::find(this->children_.cbegin(), this->children_.cend(), child);
            if (i == this->children_.cend())
                throw std::invalid_argument("The argument child is not a child of this control.");

            this->children_.erase(i);

            child->parent_ = nullptr;

            this->OnRemoveChild(this);
        }

        void Control::RemoveChild(int position)
        {
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

        Window * Control::GetWindow()
        {
            return window_;
        }

        void TraverseDescendantsInternal(Control* control,
            const std::function<void(Control*)>& predicate)
        {
            predicate(control);
            control->ForeachChild([predicate](Control* c) {
                TraverseDescendantsInternal(c, predicate);
            });
        }

        void Control::TraverseDescendants(
            const std::function<void(Control*)>& predicate)
        {
            TraverseDescendantsInternal(this, predicate);
        }

        Point Control::GetPositionRelative()
        {
            return position_;
        }

        void Control::SetPositionRelative(const Point & position)
        {
            position_ = position;
            if (auto window = GetWindow())
            {
                window->GetLayoutManager()->InvalidateControlPositionCache(this);
                window->Repaint();
            }
            //TODO: Position change notify.
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
            OnSizeChangedCore(args);
            if (auto window = GetWindow())
                window->Repaint();
        }

        Point Control::GetPositionAbsolute()
        {
            return position_cache_.lefttop_position_absolute;
        }

        Point Control::LocalToAbsolute(const Point& point)
        {
            return Point(point.x + position_cache_.lefttop_position_absolute.x,
                point.y + position_cache_.lefttop_position_absolute.y);
        }

        Point Control::AbsoluteToLocal(const Point & point)
        {
            return Point(point.x - position_cache_.lefttop_position_absolute.x,
                point.y - position_cache_.lefttop_position_absolute.y);
        }

        bool Control::IsPointInside(const Point & point)
        {
            auto size = GetSize();
            return point.x >= 0.0f && point.x < size.width && point.y >= 0.0f && point.y < size.height;
        }

        void Control::Draw(ID2D1DeviceContext* device_context)
        {
            D2D1::Matrix3x2F old_transform;
            device_context->GetTransform(&old_transform);

            auto position = GetPositionRelative();
            device_context->SetTransform(old_transform * D2D1::Matrix3x2F::Translation(position.x, position.y));

            OnDraw(device_context);
            DrawEventArgs args(this, this, device_context);
            draw_event.Raise(args);

            for (auto child : GetChildren())
                child->Draw(device_context);

            device_context->SetTransform(old_transform);
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

        Size Control::GetDesiredSize()
        {
            return desired_size_;
        }

        void Control::SetDesiredSize(const Size& desired_size)
        {
            desired_size_ = desired_size;
        }

        void Control::OnAddChild(Control* child)
        {
            if (auto window = dynamic_cast<Window*>(GetAncestor()))
            {
                child->TraverseDescendants([window](Control* control) {
                    control->OnAttachToWindow(window);
                });
                window->RefreshControlList();

            }
        }

        void Control::OnRemoveChild(Control* child)
        {
            if (auto window = dynamic_cast<Window*>(GetAncestor()))
            {
                child->TraverseDescendants([window](Control* control) {
                    control->OnDetachToWindow(window);
                });
                window->RefreshControlList();
            }
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

        }

        void Control::OnPositionChanged(PositionChangedEventArgs & args)
        {

        }

        void Control::OnSizeChanged(SizeChangedEventArgs & args)
        {
        }

        void Control::OnPositionChangedCore(PositionChangedEventArgs & args)
        {
            OnPositionChanged(args);
            position_changed_event.Raise(args);
        }

        void Control::OnSizeChangedCore(SizeChangedEventArgs & args)
        {
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

        void Control::OnMouseEnterCore(MouseEventArgs & args)
        {
            is_mouse_inside_ = true;
            OnMouseEnter(args);
            mouse_enter_event.Raise(args);
        }

        void Control::OnMouseLeaveCore(MouseEventArgs & args)
        {
            is_mouse_inside_ = false;
            OnMouseLeave(args);
            mouse_leave_event.Raise(args);
        }

        void Control::OnMouseMoveCore(MouseEventArgs & args)
        {
            OnMouseMove(args);
            mouse_move_event.Raise(args);
        }

        void Control::OnMouseDownCore(MouseButtonEventArgs & args)
        {
            OnMouseDown(args);
            mouse_down_event.Raise(args);
        }

        void Control::OnMouseUpCore(MouseButtonEventArgs & args)
        {
            OnMouseUp(args);
            mouse_up_event.Raise(args);
        }

        void Control::OnGetFocus(UiEventArgs & args)
        {
        }

        void Control::OnLoseFocus(UiEventArgs & args)
        {
        }

        void Control::OnGetFocusCore(UiEventArgs & args)
        {
            OnGetFocus(args);
            get_focus_event.Raise(args);
        }

        void Control::OnLoseFocusCore(UiEventArgs & args)
        {
            OnLoseFocus(args);
            lose_focus_event.Raise(args);
        }

        Size Control::OnMeasure(const Size& available_size)
        {
            const auto layout_params = GetLayoutParams();

#ifdef _DEBUG
            if (!layout_params->Validate())
                ::OutputDebugStringW(L"LayoutParams is not valid.");
#endif

            auto&& f = [](
                const MeasureLength& layout_length,
                const float available_length,
                const std::optional<float> max_length,
                const std::optional<float> min_length
                ) -> float
            {
                switch (layout_length.mode)
                {
                case MeasureMode::Exactly:
                {
                    auto length = layout_length.length;
                    if (min_length.has_value())
                        length = std::max(min_length.value(), length);
                    if (max_length.has_value())
                        length = std::min(max_length.value(), length);
                    if (available_length < length)
                    {
                        length = available_length;
                        ::OutputDebugStringW(L"Available length is not enough");
                    }
                    return length;
                }
                case MeasureMode::Stretch:
                case MeasureMode::Content:
                {
                    auto length = available_length;
                    if (min_length.has_value() && min_length.value() > length)
                        ::OutputDebugStringW(L"Min length is less than available length.");
                    if (max_length.has_value())
                        length = std::min(max_length.value(), length);
                    return length;
                }
                default:
                    throw std::logic_error("Unreachable code.");
                }
            };

            Size size_for_children;
            size_for_children.width = f(layout_params->size.width, available_size.width, layout_params->max_size.width, layout_params->min_size.width);
            size_for_children.height = f(layout_params->size.height, available_size.height, layout_params->max_size.height, layout_params->min_size.height);;

            auto max_child_size = Size::zero;
            ForeachChild([&](Control* control)
            {
                control->Measure(size_for_children);
                const auto&& size = control->GetDesiredSize();
                if (max_child_size.width < size.width)
                    max_child_size.width = size.width;
                if (max_child_size.height < size.height)
                    max_child_size.height = size.height;
            });


            //TODO!
        }

        void Control::OnLayout(const Rect& rect)
        {
            //TODO!
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
