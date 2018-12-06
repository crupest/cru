#include "control.hpp"

#include <algorithm>
#include <cassert>

#include "window.hpp"
#include "application.hpp"
#include "graph/graph.hpp"
#include "exception.hpp"
#include "cru_debug.hpp"
#include "convert_util.hpp"
#include "math_util.hpp"

#ifdef CRU_DEBUG_LAYOUT
#include "ui_manager.hpp"
#endif

namespace cru::ui
{
    Control::Control()
    {
        mouse_leave_event.bubble.AddHandler([this](events::MouseEventArgs& args)
        {
            if (args.GetOriginalSender() != this)
                return;
            for (auto& is_mouse_click_valid : is_mouse_click_valid_map_)
            {
                if (is_mouse_click_valid.second)
                {
                    is_mouse_click_valid.second = false;
                    OnMouseClickEnd(is_mouse_click_valid.first);
                }
            }
        });

        mouse_down_event.bubble.AddHandler([this](events::MouseButtonEventArgs& args)
        {
            if (args.GetOriginalSender() != this)
                return;

            if (is_focus_on_pressed_ && args.GetSender() == args.GetOriginalSender())
                RequestFocus();
            const auto button = args.GetMouseButton();
            is_mouse_click_valid_map_[button] = true;
            OnMouseClickBegin(button);
        });

        mouse_up_event.bubble.AddHandler([this](events::MouseButtonEventArgs& args)
        {
            if (args.GetOriginalSender() != this)
                return;

            const auto button = args.GetMouseButton();
            if (is_mouse_click_valid_map_[button])
            {
                is_mouse_click_valid_map_[button] = false;
                OnMouseClickEnd(button);
                const auto point = args.GetPoint(GetWindow());
                InvokeLater([this, button, point]
                {
                    DispatchEvent(this, &Control::mouse_click_event, nullptr, point, button);
                });
            }
        });
    }


    void Control::SetParent(Control* parent)
    {
        const auto old_parent = GetParent();
        parent_ = parent;
        const auto new_parent = GetParent();
        if (old_parent != new_parent)
            OnParentChanged(old_parent, new_parent);
    }

    void Control::SetInternalParent(Control* internal_parent)
    {
        const auto old_internal_parent = GetInternalParent();
        const auto old_parent = GetParent();
        internal_parent_ = internal_parent;
        const auto new_internal_parent = GetInternalParent();
        const auto new_parent = GetParent();
        if (old_parent != new_parent)
            OnParentChanged(old_parent, new_parent);
        if (old_internal_parent != new_internal_parent)
            OnInternalParentChanged(old_internal_parent, new_internal_parent);
    }

    void Control::SetDescendantWindow(Window* window)
    {
        if (window == nullptr && window_ == nullptr)
            return;

        //You can only attach or detach window.
        assert((window != nullptr && window_ == nullptr) || (window == nullptr && window_ != nullptr));

        if (window == nullptr)
        {
            const auto old = window_;
            TraverseDescendants([old](Control* control)
            {
                control->window_ = nullptr;
                control->OnDetachToWindow(old);
            });
        }
        else
            TraverseDescendants([window](Control* control)
            {
                control->window_ = window;
                control->OnAttachToWindow(window);
            });            
    }


    void TraverseDescendantsInternal(Control* control, const std::function<void(Control*)>& predicate)
    {
        predicate(control);
        for (auto c: control->GetInternalChildren())
            TraverseDescendantsInternal(c, predicate);
    }

    void Control::TraverseDescendants(const std::function<void(Control*)>& predicate)
    {
        TraverseDescendantsInternal(this, predicate);
    }

    Point Control::GetOffset()
    {
        return rect_.GetLeftTop();
    }

    Size Control::GetSize()
    {
        return rect_.GetSize();
    }

    void Control::SetRect(const Rect& rect)
    {
        const auto old_rect = rect_;
        rect_ = rect;

        RegenerateGeometryInfo();

        OnRectChange(old_rect, rect);

        if (auto window = GetWindow())
            window->InvalidateDraw();
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

    void Control::RefreshDescendantPositionCache()
    {
        auto point = Point::Zero();
        auto parent = this;
        while ((parent = parent->GetParent()))
        {
            const auto p = parent->GetOffset();
            point.x += p.x;
            point.y += p.y;
        }
        RefreshControlPositionCacheInternal(this, point);
    }

    void Control::RefreshControlPositionCacheInternal(Control* control, const Point& parent_lefttop_absolute)
    {
        const auto position = control->GetOffset();
        const Point lefttop(
            parent_lefttop_absolute.x + position.x,
            parent_lefttop_absolute.y + position.y
        );
        control->position_cache_.lefttop_position_absolute = lefttop;
        for(auto c : control->GetInternalChildren())
        {
            RefreshControlPositionCacheInternal(c, lefttop);
        }
    }

    bool Control::IsPointInside(const Point & point)
    {
        const auto border_geometry = geometry_info_.border_geometry;
        if (border_geometry != nullptr)
        {
            if (IsBordered())
            {
                BOOL contains;
                border_geometry->FillContainsPoint(Convert(point), D2D1::Matrix3x2F::Identity(), &contains);
                if (!contains)
                    border_geometry->StrokeContainsPoint(Convert(point), GetBorderProperty().GetStrokeWidth(), nullptr, D2D1::Matrix3x2F::Identity(), &contains);
                return contains != 0;
            }
            else
            {
                BOOL contains;
                border_geometry->FillContainsPoint(Convert(point), D2D1::Matrix3x2F::Identity(), &contains);
                return contains != 0;
            }
        }
        return false;
    }

    Control* Control::HitTest(const Point& point)
    {
        const auto point_inside = IsPointInside(point);

        if (IsClipContent())
        {
            if (!point_inside)
                return nullptr;
            if (geometry_info_.content_geometry != nullptr)
            {
                BOOL contains;
                ThrowIfFailed(geometry_info_.content_geometry->FillContainsPoint(Convert(point), D2D1::Matrix3x2F::Identity(), &contains));
                if (contains == 0)
                    return this;
            }
        }

        const auto& children = GetInternalChildren();

        for (auto i = children.crbegin(); i != children.crend(); ++i)
        {
            const auto&& lefttop = (*i)->GetOffset();
            const auto&& coerced_point = Point(point.x - lefttop.x, point.y - lefttop.y);
            const auto child_hit_test_result = (*i)->HitTest(coerced_point);
            if (child_hit_test_result != nullptr)
                return child_hit_test_result;
        }

        return point_inside ? this : nullptr;
    }

    void Control::SetClipContent(const bool clip)
    {
        if (clip_content_ == clip)
            return;

        clip_content_ = clip;
        InvalidateDraw();
    }

    void Control::Draw(ID2D1DeviceContext* device_context)
    {
        D2D1::Matrix3x2F old_transform;
        device_context->GetTransform(&old_transform);

        const auto position = GetOffset();
        device_context->SetTransform(old_transform * D2D1::Matrix3x2F::Translation(position.x, position.y));

        OnDrawDecoration(device_context);

        const auto set_layer = geometry_info_.content_geometry != nullptr && IsClipContent();
        if (set_layer)
            device_context->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), geometry_info_.content_geometry.Get()), nullptr);

        OnDrawCore(device_context);

        for (auto child : GetInternalChildren())
            child->Draw(device_context);

        if (set_layer)
            device_context->PopLayer();

        device_context->SetTransform(old_transform);
    }

    void Control::InvalidateDraw()
    {
        if (window_ != nullptr)
            window_->InvalidateDraw();
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

    void Control::Measure(const Size& available_size, const AdditionalMeasureInfo& additional_info)
    {
        SetDesiredSize(OnMeasureCore(available_size, additional_info));
    }

    void Control::Layout(const Rect& rect, const AdditionalLayoutInfo& additional_info)
    {
        auto my_additional_info = additional_info;
        my_additional_info.total_offset.x += rect.left;
        my_additional_info.total_offset.y += rect.top;
        position_cache_.lefttop_position_absolute.x = my_additional_info.total_offset.x;
        position_cache_.lefttop_position_absolute.y = my_additional_info.total_offset.y;

        SetRect(rect);
        OnLayoutCore(Rect(Point::Zero(), rect.GetSize()), my_additional_info);
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

    void Control::UpdateBorder()
    {
        RegenerateGeometryInfo();
        InvalidateLayout();
        InvalidateDraw();
    }

    void Control::SetBordered(const bool bordered)
    {
        if (bordered != is_bordered_)
        {
            is_bordered_ = bordered;
            UpdateBorder();
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

    void Control::OnParentChanged(Control* old_parent, Control* new_parent)
    {

    }

    void Control::OnInternalParentChanged(Control* old_internal_parent, Control* new_internal_parent)
    {

    }

    void Control::OnAttachToWindow(Window* window)
    {
        window_ = window;
    }

    void Control::OnDetachToWindow(Window * window)
    {
        window_ = nullptr;
    }

    void Control::OnDrawDecoration(ID2D1DeviceContext* device_context)
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

        if (is_bordered_ && geometry_info_.border_geometry != nullptr)
            device_context->DrawGeometry(
                geometry_info_.border_geometry.Get(),
                GetBorderProperty().GetBrush().Get(),
                GetBorderProperty().GetStrokeWidth(),
                GetBorderProperty().GetStrokeStyle().Get()
            );
    }

    void Control::OnDrawCore(ID2D1DeviceContext* device_context)
    {
        const auto ground_geometry = geometry_info_.padding_content_geometry;
        //draw background.
        if (ground_geometry != nullptr && background_brush_ != nullptr)
            device_context->FillGeometry(ground_geometry.Get(), background_brush_.Get());
        const auto padding_rect = GetRect(RectRange::Padding);
        graph::WithTransform(device_context, D2D1::Matrix3x2F::Translation(padding_rect.left, padding_rect.top),
            [this](ID2D1DeviceContext* device_context)
            {
                events::DrawEventArgs args(this, this, device_context);
                draw_background_event.Raise(args);
            });


        const auto rect = GetRect(RectRange::Content);
        graph::WithTransform(device_context, D2D1::Matrix3x2F::Translation(rect.left, rect.top),
            [this](ID2D1DeviceContext* device_context)
            {
                events::DrawEventArgs args(this, this, device_context);
                draw_content_event.Raise(args);
            });


        //draw foreground.
        if (ground_geometry != nullptr && foreground_brush_ != nullptr)
            device_context->FillGeometry(ground_geometry.Get(), foreground_brush_.Get());
        graph::WithTransform(device_context, D2D1::Matrix3x2F::Translation(padding_rect.left, padding_rect.top),
            [this](ID2D1DeviceContext* device_context)
            {
                events::DrawEventArgs args(this, this, device_context);
                draw_foreground_event.Raise(args);
            });
    }

    void Control::OnRectChange(const Rect& old_rect, const Rect& new_rect)
    {

    }

    void Control::RegenerateGeometryInfo()
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
            geometry_info_.border_geometry = std::move(geometry);

            const auto padding_rect = GetRect(RectRange::Padding);
            const auto in_border_rounded_rect = D2D1::RoundedRect(Convert(padding_rect),
                GetBorderProperty().GetRadiusX() - GetBorderProperty().GetStrokeWidth() / 2.0f,
                GetBorderProperty().GetRadiusY() - GetBorderProperty().GetStrokeWidth() / 2.0f);

            Microsoft::WRL::ComPtr<ID2D1RoundedRectangleGeometry> geometry2;
            ThrowIfFailed(
                graph::GraphManager::GetInstance()->GetD2D1Factory()->CreateRoundedRectangleGeometry(in_border_rounded_rect, &geometry2)
            );
            geometry_info_.padding_content_geometry = geometry2;


            Microsoft::WRL::ComPtr<ID2D1RectangleGeometry> geometry3;
            ThrowIfFailed(
                graph::GraphManager::GetInstance()->GetD2D1Factory()->CreateRectangleGeometry(Convert(GetRect(RectRange::Content)), &geometry3)
            );
            Microsoft::WRL::ComPtr<ID2D1PathGeometry> geometry4;
            ThrowIfFailed(
                graph::GraphManager::GetInstance()->GetD2D1Factory()->CreatePathGeometry(&geometry4)
            );
            Microsoft::WRL::ComPtr<ID2D1GeometrySink> sink;
            geometry4->Open(&sink);
            ThrowIfFailed(
                geometry3->CombineWithGeometry(geometry2.Get(), D2D1_COMBINE_MODE_INTERSECT, D2D1::Matrix3x2F::Identity(), sink.Get())
            );
            sink->Close();
            geometry_info_.content_geometry = std::move(geometry4);
        }
        else
        {
            const auto bound_rect = GetRect(RectRange::Padding);
            Microsoft::WRL::ComPtr<ID2D1RectangleGeometry> geometry;
            ThrowIfFailed(
                graph::GraphManager::GetInstance()->GetD2D1Factory()->CreateRectangleGeometry(Convert(bound_rect), &geometry)
            );
            geometry_info_.border_geometry = geometry;
            geometry_info_.padding_content_geometry = std::move(geometry);

            Microsoft::WRL::ComPtr<ID2D1RectangleGeometry> geometry2;
            ThrowIfFailed(
                graph::GraphManager::GetInstance()->GetD2D1Factory()->CreateRectangleGeometry(Convert(GetRect(RectRange::Content)), &geometry2)
            );
            geometry_info_.content_geometry = std::move(geometry2);
        }

        //TODO: generate debug geometry
#ifdef CRU_DEBUG_LAYOUT
        margin_geometry_ = CalculateSquareRingGeometry(GetRect(RectRange::Margin), GetRect(RectRange::FullBorder));
        padding_geometry_ = CalculateSquareRingGeometry(GetRect(RectRange::Padding), GetRect(RectRange::Content));
#endif
    }

    void Control::OnMouseClickBegin(MouseButton button)
    {
    }

    void Control::OnMouseClickEnd(MouseButton button)
    {
    }

    inline Size ThicknessToSize(const Thickness& thickness)
    {
        return Size(thickness.left + thickness.right, thickness.top + thickness.bottom);
    }

    Size Control::OnMeasureCore(const Size& available_size, const AdditionalMeasureInfo& additional_info)
    {
        const auto layout_params = GetLayoutParams();

        if (!layout_params->Validate())
            throw std::runtime_error("LayoutParams is not valid. Please check it.");

        auto my_additional_info = additional_info;

        if (layout_params->width.mode == MeasureMode::Content)
            my_additional_info.horizontal_stretchable = false;
        else if (layout_params->width.mode == MeasureMode::Exactly)
            my_additional_info.horizontal_stretchable = true;
        // if stretch, then inherent parent's value

        if (layout_params->height.mode == MeasureMode::Content)
            my_additional_info.vertical_stretchable = false;
        else if (layout_params->height.mode == MeasureMode::Exactly)
            my_additional_info.vertical_stretchable = true;
        // if stretch, then inherent parent's value


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

        const auto content_actual_size = OnMeasureContent(content_measure_size, my_additional_info);


         
        auto&& calculate_final_length = [](const bool stretch, const std::optional<float> min_length, const float measure_length, const float actual_length) -> float
        {
            // only use measure length when stretch and actual length is smaller than measure length, that is "stretch"
            if (stretch && actual_length < measure_length)
                return measure_length;
            return Coerce(actual_length, min_length, std::nullopt);
        };

        const auto final_size = Size(
            calculate_final_length(my_additional_info.horizontal_stretchable, layout_params->width.min, content_measure_size.width, content_actual_size.width),
            calculate_final_length(my_additional_info.vertical_stretchable, layout_params->height.min, content_measure_size.height, content_actual_size.height)
        ) + outer_size;

        return final_size;
    }

    void Control::OnLayoutCore(const Rect& rect, const AdditionalLayoutInfo& additional_info)
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

        OnLayoutContent(content_rect, additional_info);
    }

    const std::vector<Control*> NoChildControl::empty_control_vector{};

    std::list<Control*> GetAncestorList(Control* control)
    {
        std::list<Control*> l;
        while (control != nullptr)
        {
            l.push_front(control);
            control = control->GetInternalParent();
        }
        return l;
    }

    void NoChildControl::OnLayoutContent(const Rect& rect, const AdditionalLayoutInfo& additional_info)
    {

    }

    SingleChildControl::SingleChildControl() : child_vector_{nullptr}, child_(child_vector_[0])
    {

    }

    SingleChildControl::~SingleChildControl()
    {
        delete child_;
    }

    void SingleChildControl::SetChild(Control* child)
    {
        if (child == child_)
            return;

        const auto window = GetWindow();
        const auto old_child = child_;
        child_ = child;
        if (old_child)
        {
            old_child->SetInternalParent(nullptr);
            old_child->SetDescendantWindow(nullptr);
        }
        if (child)
        {
            child->SetInternalParent(this);
            child->SetDescendantWindow(window);
        }
        OnChildChanged(old_child, child);
    }

    void SingleChildControl::OnChildChanged(Control* old_child, Control* new_child)
    {

    }

    Size SingleChildControl::OnMeasureContent(const Size& available_size, const AdditionalMeasureInfo& additional_info)
    {
        auto child_size = Size::Zero();
        if (child_)
        {
            child_->Measure(available_size, additional_info);
            child_size = child_->GetDesiredSize();
        }

        return child_size;
    }

    void SingleChildControl::OnLayoutContent(const Rect& rect, const AdditionalLayoutInfo& additional_info)
    {
        if (child_)
        {
            const auto layout_params = child_->GetLayoutParams();
            const auto size = child_->GetDesiredSize();

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

            child_->Layout(Rect(Point(
                calculate_anchor(rect.left, layout_params->width.alignment, rect.width, size.width),
                calculate_anchor(rect.top, layout_params->height.alignment, rect.height, size.height)
            ), size), additional_info);
        }
    }

    void AddChildCheck(Control* control)
    {
        if (control->GetInternalParent() != nullptr)
            throw std::invalid_argument("The control already has a parent.");

        if (dynamic_cast<Window*>(control))
            throw std::invalid_argument("Can't add a window as child.");
    }

    MultiChildControl::~MultiChildControl()
    {
        for (const auto child : children_)
            delete child;
    }

    void MultiChildControl::AddChild(Control* control)
    {
        AddChildCheck(control);

        children_.push_back(control);

        control->SetInternalParent(this);
        control->SetDescendantWindow(GetWindow());

        OnAddChild(control);
    }

    void MultiChildControl::AddChild(Control* control, const int position)
    {
        AddChildCheck(control);

        if (position < 0 || static_cast<decltype(children_.size())>(position) > this->children_.size())
            throw std::invalid_argument("The position is out of range.");

        children_.insert(this->children_.cbegin() + position, control);

        control->SetInternalParent(this);
        control->SetDescendantWindow(GetWindow());

        OnAddChild(control);
    }

    void MultiChildControl::RemoveChild(Control* child)
    {
        const auto i = std::find(this->children_.cbegin(), this->children_.cend(), child);
        if (i == this->children_.cend())
            throw std::invalid_argument("The argument child is not a child of this control.");

        children_.erase(i);

        child->SetInternalParent(nullptr);
        child->SetDescendantWindow(nullptr);

        OnRemoveChild(child);
    }

    void MultiChildControl::RemoveChild(const int position)
    {
        if (position < 0 || static_cast<decltype(this->children_.size())>(position) >= this->children_.size())
            throw std::invalid_argument("The position is out of range.");

        const auto i = children_.cbegin() + position;
        const auto child = *i;

        children_.erase(i);

        child->SetInternalParent(nullptr);
        child->SetDescendantWindow(nullptr);

        OnRemoveChild(child);
    }

    void MultiChildControl::OnAddChild(Control* child)
    {

    }

    void MultiChildControl::OnRemoveChild(Control* child)
    {

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
}
