#include "control.hpp"

#include <algorithm>

#include "window.hpp"
#include "application.hpp"
#include "graph/graph.hpp"
#include "exception.hpp"
#include "cru_debug.hpp"
#include "convert_util.hpp"

#ifdef CRU_DEBUG_LAYOUT
#include "ui_manager.hpp"
#endif

namespace cru::ui
{
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

    Point Control::ControlToWindow(const Point& point) const
    {
        return Point(point.x + position_cache_.lefttop_position_absolute.x,
            point.y + position_cache_.lefttop_position_absolute.y);
    }

    Point Control::WindowToControl(const Point & point) const
    {
        return Point(point.x - position_cache_.lefttop_position_absolute.x,
            point.y - position_cache_.lefttop_position_absolute.y);
    }

    bool Control::IsPointInside(const Point & point)
    {
        if (border_geometry_ != nullptr)
        {
            if (IsBordered())
            {
                BOOL contains;
                border_geometry_->FillContainsPoint(Convert(point), D2D1::Matrix3x2F::Identity(), &contains);
                if (!contains)
                    border_geometry_->StrokeContainsPoint(Convert(point), GetBorderProperty().GetStrokeWidth(), nullptr, D2D1::Matrix3x2F::Identity(), &contains);
                return contains != 0;
            }
            else
            {
                BOOL contains;
                border_geometry_->FillContainsPoint(Convert(point), D2D1::Matrix3x2F::Identity(), &contains);
                return contains != 0;
            }
        }
        return false;
    }

    void Control::Draw(ID2D1DeviceContext* device_context)
    {
        D2D1::Matrix3x2F old_transform;
        device_context->GetTransform(&old_transform);

        const auto position = GetPositionRelative();
        device_context->SetTransform(old_transform * D2D1::Matrix3x2F::Translation(position.x, position.y));

        OnDrawCore(device_context);

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
            window->WindowInvalidateLayout();
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

    Point Control::TransformPoint(const Point& point, const RectRange from, const RectRange to)
    {
        const auto rect_from = GetRect(from);
        const auto rect_to = GetRect(to);
        auto p = point;
        p.x += rect_from.left;
        p.y += rect_from.top;
        p.x -= rect_to.left;
        p.y -= rect_to.top;
        return p;
    }

    void Control::InvalidateBorder()
    {
        RegenerateBorderGeometry();
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

    void Control::SetCursor(const Cursor::Ptr& cursor)
    {
        if (cursor != cursor_)
        {
            cursor_ = cursor;
            const auto window = GetWindow();
            if (window && window->GetMouseHoverControl() == this)
                window->UpdateCursor();
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

    void Control::OnDrawCore(ID2D1DeviceContext* device_context)
    {
        #ifdef CRU_DEBUG_LAYOUT
        if (GetWindow()->IsDebugLayout())
        {
            if (padding_geometry_ != nullptr)
                device_context->FillGeometry(padding_geometry_.Get(), UiManager::GetInstance()->GetPredefineResources()->debug_layout_padding_brush.Get());
            if (margin_geometry_ != nullptr)
                device_context->FillGeometry(margin_geometry_.Get(), UiManager::GetInstance()->GetPredefineResources()->debug_layout_margin_brush.Get());
            device_context->DrawRectangle(Convert(GetRect(RectRange::Margin)), UiManager::GetInstance()->GetPredefineResources()->debug_layout_out_border_brush.Get());
        }
#endif

        if (is_bordered_ && border_geometry_ != nullptr)
            device_context->DrawGeometry(
                border_geometry_.Get(),
                GetBorderProperty().GetBrush().Get(),
                GetBorderProperty().GetStrokeWidth(),
                GetBorderProperty().GetStrokeStyle().Get()
            );

        //draw background.
        if (in_border_geometry_ != nullptr && background_brush_ != nullptr)
            device_context->FillGeometry(in_border_geometry_.Get(), background_brush_.Get());
        const auto padding_rect = GetRect(RectRange::Padding);
        graph::WithTransform(device_context, D2D1::Matrix3x2F::Translation(padding_rect.left, padding_rect.top),
            [this](ID2D1DeviceContext* device_context)
            {
                OnDrawBackground(device_context);
                DrawEventArgs args(this, this, device_context);
                draw_background_event.Raise(args);
            });


        const auto rect = GetRect(RectRange::Content);
        graph::WithTransform(device_context, D2D1::Matrix3x2F::Translation(rect.left, rect.top),
            [this](ID2D1DeviceContext* device_context)
            {
                OnDrawContent(device_context);
                DrawEventArgs args(this, this, device_context);
                draw_content_event.Raise(args);
            });


        //draw foreground.
        if (in_border_geometry_ != nullptr && foreground_brush_ != nullptr)
            device_context->FillGeometry(in_border_geometry_.Get(), foreground_brush_.Get());
        graph::WithTransform(device_context, D2D1::Matrix3x2F::Translation(padding_rect.left, padding_rect.top),
            [this](ID2D1DeviceContext* device_context)
            {
                OnDrawForeground(device_context);
                DrawEventArgs args(this, this, device_context);
                draw_foreground_event.Raise(args);
            });
    }

    void Control::OnDrawContent(ID2D1DeviceContext * device_context)
    {

    }

    void Control::OnDrawForeground(ID2D1DeviceContext* device_context)
    {

    }

    void Control::OnDrawBackground(ID2D1DeviceContext* device_context)
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
        RegenerateBorderGeometry();
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

    void Control::RegenerateBorderGeometry()
    {
        if (IsBordered())
        {
            const auto bound_rect = GetRect(RectRange::HalfBorder);
            const auto bound_rounded_rect = D2D1::RoundedRect(Convert(bound_rect),
                GetBorderProperty().GetRadiusX(),
                GetBorderProperty().GetRadiusY());

            Microsoft::WRL::ComPtr<ID2D1RoundedRectangleGeometry> geometry;
            ThrowIfFailed(
                graph::GraphManager::GetInstance()->GetD2D1Factory()->CreateRoundedRectangleGeometry(bound_rounded_rect, &geometry)
            );
            border_geometry_ = std::move(geometry);

            const auto in_border_rect = GetRect(RectRange::Padding);
            const auto in_border_rounded_rect = D2D1::RoundedRect(Convert(in_border_rect),
                GetBorderProperty().GetRadiusX() - GetBorderProperty().GetStrokeWidth() / 2.0f,
                GetBorderProperty().GetRadiusY() - GetBorderProperty().GetStrokeWidth() / 2.0f);

            Microsoft::WRL::ComPtr<ID2D1RoundedRectangleGeometry> geometry2;
            ThrowIfFailed(
                graph::GraphManager::GetInstance()->GetD2D1Factory()->CreateRoundedRectangleGeometry(in_border_rounded_rect, &geometry2)
            );
            in_border_geometry_ = std::move(geometry2);
        }
        else
        {
            const auto bound_rect = GetRect(RectRange::Padding);
            Microsoft::WRL::ComPtr<ID2D1RectangleGeometry> geometry;
            ThrowIfFailed(
                graph::GraphManager::GetInstance()->GetD2D1Factory()->CreateRectangleGeometry(Convert(bound_rect), &geometry)
            );
            border_geometry_ = geometry;
            in_border_geometry_ = std::move(geometry);
        }
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

    Size Control::OnMeasureCore(const Size& available_size)
    {
        const auto layout_params = GetLayoutParams();

        if (!layout_params->Validate())
            throw std::runtime_error("LayoutParams is not valid. Please check it.");

        auto border_size = Size::Zero();
        if (is_bordered_)
        {
            const auto border_width = GetBorderProperty().GetStrokeWidth();
            border_size = Size(border_width * 2.0f, border_width * 2.0f);
        }

        // the total size of padding, border and margin
        const auto outer_size = ThicknessToSize(layout_params->padding) +
            ThicknessToSize(layout_params->margin) + border_size;


        auto&& get_content_measure_length = [](const LayoutSideParams& layout_length, const float available_length, const float outer_length) -> float
        {
            float length;
            if (layout_length.mode == MeasureMode::Exactly)
                length = layout_length.length;
            else if (available_length > outer_length)
                length = available_length - outer_length;
            else
                length = 0;
            return Coerce(length, layout_length.min, layout_length.max);
        };

        // if padding, margin and border exceeded, then content size is 0.
        const auto content_measure_size = Size(
            get_content_measure_length(layout_params->width, available_size.width, outer_size.width),
            get_content_measure_length(layout_params->height, available_size.height, outer_size.height)
        );

        const auto content_actual_size = OnMeasureContent(content_measure_size);

        auto stretch_width = false;
        auto stretch_width_determined = true;
        auto stretch_height = false;
        auto stretch_height_determined = true;

        // if it is stretch, init is stretch, and undetermined.
        if (layout_params->width.mode == MeasureMode::Stretch)
        {
            stretch_width = true;
            stretch_width_determined = false;
        }
        if (layout_params->height.mode == MeasureMode::Stretch)
        {
            stretch_height = true;
            stretch_width_determined = false;
        }

        if (!stretch_width_determined || !stretch_height_determined)
        {
            auto parent = GetParent();
            while (parent != nullptr)
            {
                auto lp = parent->GetLayoutParams();

                if (!stretch_width_determined)
                {
                    if (lp->width.mode == MeasureMode::Content) // if the first ancestor that is not stretch is content, then it can't stretch.
                    {
                        stretch_width = false;
                        stretch_width_determined = true;
                    }
                    if (lp->width.mode == MeasureMode::Exactly) // if the first ancestor that is not stretch is content, then it must be stretch.
                    {
                        stretch_width = true;
                        stretch_width_determined = true;
                    }
                }

                if (!stretch_height_determined) // the same as width
                {
                    if (lp->height.mode == MeasureMode::Content) // if the first ancestor that is not stretch is content, then it can't stretch.
                    {
                        stretch_height = false;
                        stretch_height_determined = true;
                    }
                    if (lp->height.mode == MeasureMode::Exactly) // if the first ancestor that is not stretch is content, then it must be stretch.
                    {
                        stretch_height = true;
                        stretch_height_determined = true;
                    }
                }

                if (stretch_width_determined && stretch_height_determined) // if both are determined.
                    break;

                parent = GetParent();
            }
        }
         
        auto&& calculate_final_length = [](const bool stretch, const std::optional<float> min_length, const float measure_length, const float actual_length) -> float
        {
            // only use measure length when stretch and actual length is smaller than measure length, that is "stretch"
            if (stretch && actual_length < measure_length)
                return measure_length;
            return Coerce(actual_length, min_length, std::nullopt);
        };

        const auto final_size = Size(
            calculate_final_length(stretch_width, layout_params->width.min, content_measure_size.width, content_actual_size.width),
            calculate_final_length(stretch_height, layout_params->height.min, content_measure_size.height, content_actual_size.height)
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

        if (content_rect.width < 0.0)
            throw std::runtime_error(Format("Width to layout must sufficient. But in {}, width for content is {}.", ToUtf8String(GetControlType()), content_rect.width));
        if (content_rect.height < 0.0)
            throw std::runtime_error(Format("Height to layout must sufficient. But in {}, height for content is {}.", ToUtf8String(GetControlType()), content_rect.height));

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

        // coerce size fro stretch.
        for (auto control: GetChildren())
        {
            auto size = control->GetDesiredSize();
            const auto layout_params = control->GetLayoutParams();
            if (layout_params->width.mode == MeasureMode::Stretch)
                size.width = max_child_size.width;
            if (layout_params->height.mode == MeasureMode::Stretch)
                size.height = max_child_size.height;
            control->SetDesiredSize(size);
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
