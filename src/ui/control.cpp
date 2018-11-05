#include "control.h"

#include <algorithm>

#include "window.h"
#include "graph/graph.h"
#include "exception.h"

namespace cru {
    namespace ui {
        using namespace events;

        Control::Control(const bool container) :
            is_container_(container)
        {

        }

        Control::Control(WindowConstructorTag, Window* window) : Control(true)
        {
            window_ = window;
        }

        Control::~Control()
        {
            for (auto control: GetChildren())
            {
                delete control;
            }
        }

        void AddChildCheck(Control* control)
        {
            if (control->GetParent() != nullptr)
                throw std::invalid_argument("The control already has a parent.");

            if (dynamic_cast<Window*>(control))
                throw std::invalid_argument("Can't add a window as child.");
        }

        const std::vector<Control*>& Control::GetChildren() const
        {
            return children_;
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

        void TraverseDescendantsInternal(Control* control, const std::function<void(Control*)>& predicate)
        {
            predicate(control);
            if (control->IsContainer())
                for (auto c: control->GetChildren())
                {
                    TraverseDescendantsInternal(c, predicate);
                }
        }

        void Control::TraverseDescendants(const std::function<void(Control*)>& predicate)
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

            const auto rect = GetRect(RectRange::Content);
            graph::WithTransform(device_context, D2D1::Matrix3x2F::Translation(rect.left, rect.top),
                [this](ID2D1DeviceContext* device_context)
                {
                    OnDrawContent(device_context);
                    DrawEventArgs args(this, this, device_context);
                    draw_event.Raise(args);
                });

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

        void Control::InvalidateLayout()
        {
            if (const auto window = GetWindow())
                LayoutManager::GetInstance()->InvalidateWindowLayout(window);
        }

        void Control::Measure(const Size& available_size)
        {
            SetDesiredSize(OnMeasureCore(available_size));
        }

        void Control::Layout(const Rect& rect)
        {
            SetPositionRelative(rect.GetLeftTop());
            SetSize(rect.GetSize());
            OnLayoutCore(Rect(Point::Zero(), rect.GetSize()));
        }

        Size Control::GetDesiredSize() const
        {
            return desired_size_;
        }

        void Control::SetDesiredSize(const Size& desired_size)
        {
            desired_size_ = desired_size;
        }

        inline void Shrink(Rect& rect, const Thickness& thickness)
        {
            rect.left += thickness.left;
            rect.top += thickness.top;
            rect.width -= thickness.GetHorizontalTotal();
            rect.height -= thickness.GetVerticalTotal();
        }

        Rect Control::GetRect(const RectRange range)
        {
            if (GetSize() == Size::Zero())
                return Rect();

            const auto layout_params = GetLayoutParams();

            auto result = Rect(Point::Zero(), GetSize());

            if (range == RectRange::Margin)
                return result;

            Shrink(result, layout_params->margin);

            if (range == RectRange::FullBorder)
                return result;

            if (is_bordered_)
                Shrink(result, Thickness(GetBorderProperty().GetStrokeWidth() / 2.0f));

            if (range == RectRange::HalfBorder)
                return result;

            if (is_bordered_)
                Shrink(result, Thickness(GetBorderProperty().GetStrokeWidth() / 2.0f));

            if (range == RectRange::Padding)
                return result;

            Shrink(result, layout_params->padding);

            return result;
        }

        void Control::InvalidateBorder()
        {
            InvalidateLayout();
            Repaint();
        }

        void Control::SetBordered(const bool bordered)
        {
            if (bordered != is_bordered_)
            {
                is_bordered_ = bordered;
                InvalidateBorder();
            }
        }

        void Control::OnAddChild(Control* child)
        {
            if (auto window = GetWindow())
            {
                child->TraverseDescendants([window](Control* control) {
                    control->OnAttachToWindow(window);
                });
                window->RefreshControlList();
                InvalidateLayout();
            }
        }

        void Control::OnRemoveChild(Control* child)
        {
            if (auto window = GetWindow())
            {
                child->TraverseDescendants([window](Control* control) {
                    control->OnDetachToWindow(window);
                });
                window->RefreshControlList();
                InvalidateLayout();
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

        inline D2D1_RECT_F Convert(const Rect& rect)
        {
            return D2D1::RectF(rect.left, rect.top, rect.left + rect.width, rect.top + rect.height);
        }

        void Control::OnDraw(ID2D1DeviceContext* device_context)
        {
#ifdef CRU_DEBUG_LAYOUT
            if (GetWindow()->IsDebugLayout())
            {
                const auto resource = Application::GetInstance()->GetDebugLayoutResource();
                if (padding_geometry_ != nullptr)
                    device_context->FillGeometry(padding_geometry_.Get(), resource->padding_brush.Get());
                if (margin_geometry_ != nullptr)
                    device_context->FillGeometry(margin_geometry_.Get(), resource->margin_brush.Get());
                device_context->DrawRectangle(Convert(GetRect(RectRange::Margin)), resource->out_border_brush.Get());
            }
#endif

            if (is_bordered_)
            {
                const auto border_rect = GetRect(RectRange::HalfBorder);
                device_context->DrawRoundedRectangle(
                    D2D1::RoundedRect(
                        Convert(border_rect),
                        GetBorderProperty().GetRadiusX(),
                        GetBorderProperty().GetRadiusY()
                    ),
                    GetBorderProperty().GetBrush().Get(),
                    GetBorderProperty().GetStrokeWidth(),
                    GetBorderProperty().GetStrokeStyle().Get()
                );
            }
        }

        void Control::OnDrawContent(ID2D1DeviceContext * device_context)
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

        }

        namespace
        {
#ifdef CRU_DEBUG_LAYOUT
            Microsoft::WRL::ComPtr<ID2D1Geometry> CalculateSquareRingGeometry(const Rect& out, const Rect& in)
            {
                const auto d2d1_factory = graph::GraphManager::GetInstance()->GetD2D1Factory();
                Microsoft::WRL::ComPtr<ID2D1RectangleGeometry> out_geometry;
                ThrowIfFailed(d2d1_factory->CreateRectangleGeometry(Convert(out), &out_geometry));
                Microsoft::WRL::ComPtr<ID2D1RectangleGeometry> in_geometry;
                ThrowIfFailed(d2d1_factory->CreateRectangleGeometry(Convert(in), &in_geometry));
                Microsoft::WRL::ComPtr<ID2D1PathGeometry> result_geometry;
                ThrowIfFailed(d2d1_factory->CreatePathGeometry(&result_geometry));
                Microsoft::WRL::ComPtr<ID2D1GeometrySink> sink;
                ThrowIfFailed(result_geometry->Open(&sink));
                ThrowIfFailed(out_geometry->CombineWithGeometry(in_geometry.Get(), D2D1_COMBINE_MODE_EXCLUDE, D2D1::Matrix3x2F::Identity(), sink.Get()));
                ThrowIfFailed(sink->Close());
                return result_geometry;
            }
#endif
        }

        void Control::OnSizeChangedCore(SizeChangedEventArgs & args)
        {
#ifdef CRU_DEBUG_LAYOUT
            margin_geometry_ = CalculateSquareRingGeometry(GetRect(RectRange::Margin), GetRect(RectRange::FullBorder));
            padding_geometry_ = CalculateSquareRingGeometry(GetRect(RectRange::Padding), GetRect(RectRange::Content));
#endif
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
            for (auto& is_mouse_click_valid : is_mouse_click_valid_map_)
            {
                if (is_mouse_click_valid.second)
                {
                    is_mouse_click_valid.second = false;
                    OnMouseClickEnd(is_mouse_click_valid.first);
                }
            }
        }

        void Control::OnMouseMoveCore(MouseEventArgs & args)
        {

        }

        void Control::OnMouseDownCore(MouseButtonEventArgs & args)
        {
            if (is_focus_on_pressed_ && args.GetSender() == args.GetOriginalSender())
                RequestFocus();
            is_mouse_click_valid_map_[args.GetMouseButton()] = true;
            OnMouseClickBegin(args.GetMouseButton());
        }

        void Control::OnMouseUpCore(MouseButtonEventArgs & args)
        {
            if (is_mouse_click_valid_map_[args.GetMouseButton()])
            {
                is_mouse_click_valid_map_[args.GetMouseButton()] = false;
                RaiseMouseClickEvent(args);
                OnMouseClickEnd(args.GetMouseButton());
            }
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

        void Control::OnMouseClickBegin(MouseButton button)
        {

        }

        void Control::OnMouseClickEnd(MouseButton button)
        {

        }

        void Control::OnKeyDown(KeyEventArgs& args)
        {
        }

        void Control::OnKeyUp(KeyEventArgs& args)
        {
        }

        void Control::OnChar(CharEventArgs& args)
        {
        }

        void Control::OnKeyDownCore(KeyEventArgs& args)
        {
        }

        void Control::OnKeyUpCore(KeyEventArgs& args)
        {
        }

        void Control::OnCharCore(CharEventArgs& args)
        {
        }

        void Control::RaiseKeyDownEvent(KeyEventArgs& args)
        {
            OnKeyDownCore(args);
            OnKeyDown(args);
            key_down_event.Raise(args);
        }

        void Control::RaiseKeyUpEvent(KeyEventArgs& args)
        {
            OnKeyUpCore(args);
            OnKeyUp(args);
            key_up_event.Raise(args);
        }

        void Control::RaiseCharEvent(CharEventArgs& args)
        {
            OnCharCore(args);
            OnChar(args);
            char_event.Raise(args);
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

        inline Size ThicknessToSize(const Thickness& thickness)
        {
            return Size(thickness.left + thickness.right, thickness.top + thickness.bottom);
        }

        inline float AtLeast0(const float value)
        {
            return value < 0 ? 0 : value;
        }

        inline Size AtLeast0(const Size& size)
        {
            return Size(AtLeast0(size.width), AtLeast0(size.height));
        }

        Size Control::OnMeasureCore(const Size& available_size)
        {
            const auto layout_params = GetLayoutParams();

            auto border_size = Size::Zero();
            if (is_bordered_)
            {
                const auto border_width = GetBorderProperty().GetStrokeWidth();
                border_size = Size(border_width * 2.0f, border_width * 2.0f);
            }

            const auto outer_size = ThicknessToSize(layout_params->padding) +
                ThicknessToSize(layout_params->margin) + border_size;

            if (!layout_params->Validate())
                throw std::runtime_error("LayoutParams is not valid. Please check it.");

            auto&& get_available_length_for_child = [](const LayoutSideParams& layout_length, const float available_length) -> float
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

            const auto size_for_children = AtLeast0(Size(
                get_available_length_for_child(layout_params->width, available_size.width),
                get_available_length_for_child(layout_params->height, available_size.height)
            ) - outer_size);

            const auto actual_size_for_children = OnMeasureContent(size_for_children);

            auto&& calculate_final_length = [](const LayoutSideParams& layout_length, const float length_for_children, const float actual_length_for_children) -> float
            {
                switch (layout_length.mode)
                {
                case MeasureMode::Exactly:
                case MeasureMode::Stretch:
                    return length_for_children;
                case MeasureMode::Content:
                    return actual_length_for_children;
                default:
                    UnreachableCode();
                }
            };

            const auto final_size = Size(
                calculate_final_length(layout_params->width, size_for_children.width, actual_size_for_children.width),
                calculate_final_length(layout_params->height, size_for_children.height, actual_size_for_children.height)
            ) + outer_size;

            return final_size;
        }

        void Control::OnLayoutCore(const Rect& rect)
        {
            const auto layout_params = GetLayoutParams();

            auto border_width = 0.0f;
            if (is_bordered_)
            {
                border_width = GetBorderProperty().GetStrokeWidth();
            }

            const Rect content_rect(
                rect.left + layout_params->padding.left + layout_params->margin.right + border_width,
                rect.top + layout_params->padding.top + layout_params->margin.top + border_width,
                rect.width - layout_params->padding.GetHorizontalTotal() - layout_params->margin.GetHorizontalTotal() - border_width * 2.0f,
                rect.height - layout_params->padding.GetVerticalTotal() - layout_params->margin.GetVerticalTotal() - border_width * 2.0f
            );

            OnLayoutContent(content_rect);
        }

        Size Control::OnMeasureContent(const Size& available_size)
        {
            auto max_child_size = Size::Zero();
            for (auto control: GetChildren())
            {
                control->Measure(available_size);
                const auto&& size = control->GetDesiredSize();
                if (max_child_size.width < size.width)
                    max_child_size.width = size.width;
                if (max_child_size.height < size.height)
                    max_child_size.height = size.height;
            }
            return max_child_size;
        }

        void Control::OnLayoutContent(const Rect& rect)
        {
            for (auto control: GetChildren())
            {
                const auto layout_params = control->GetLayoutParams();
                const auto size = control->GetDesiredSize();

                auto&& calculate_anchor = [](const float anchor, const Alignment alignment, const float layout_length, const float control_length) -> float
                {
                    switch (alignment)
                    {
                    case Alignment::Center:
                        return anchor + (layout_length - control_length) / 2;
                    case Alignment::Start:
                        return anchor;
                    case Alignment::End:
                        return anchor + layout_length - control_length;
                    default:
                        UnreachableCode();
                    }
                };

                control->Layout(Rect(Point(
                    calculate_anchor(rect.left, layout_params->width.alignment, rect.width, size.width),
                    calculate_anchor(rect.top, layout_params->height.alignment, rect.height, size.height)
                ), size));
            }
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

            // find the last same control or the last control (one is ancestor of the other)
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
