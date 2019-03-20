#include "text_control.hpp"

#include <cassert>

#include "ui/window.hpp"
#include "graph/graph.hpp"
#include "exception.hpp"
#include "ui/ui_manager.hpp"

namespace cru::ui::controls
{
    namespace
    {
        unsigned TextLayoutHitTest(IDWriteTextLayout* text_layout, const Point& point)
        {
            BOOL is_trailing, is_inside;
            DWRITE_HIT_TEST_METRICS metrics{};
            text_layout->HitTestPoint(point.x, point.y, &is_trailing, &is_inside, &metrics);
            return is_trailing == 0 ? metrics.textPosition : metrics.textPosition + 1;
        }


    }

    TextControl::TextControl(const Microsoft::WRL::ComPtr<IDWriteTextFormat>& init_text_format,
        const Microsoft::WRL::ComPtr<ID2D1Brush>& init_brush)
    {
        text_format_ = init_text_format;

        RecreateTextLayout();

        brush_ = init_brush;

        selection_brush_ = UiManager::GetInstance()->GetPredefineResources()->text_control_selection_brush;

        SetClipContent(true);

        draw_content_event.AddHandler([this](events::DrawEventArgs& args)
        {
        });

        mouse_down_event.bubble.AddHandler([this](events::MouseButtonEventArgs& args)
        {
             if (is_selectable_ && args.GetMouseButton() == MouseButton::Left && GetRect(RectRange::Padding).IsPointInside(args.GetPoint(this, RectRange::Margin)))
            {
                selected_range_ = std::nullopt;
                const auto hit_test_result = TextLayoutHitTest(text_layout_.Get(), args.GetPoint(this));
                RequestChangeCaretPosition(hit_test_result);
                mouse_down_position_ = hit_test_result;
                is_selecting_ = true;
                GetWindow()->CaptureMouseFor(this);
                InvalidateDraw();
            }
        });

        mouse_move_event.bubble.AddHandler([this](events::MouseEventArgs& args)
        {
            if (is_selecting_)
            {
                const auto hit_test_result = TextLayoutHitTest(text_layout_.Get(), args.GetPoint(this));
                RequestChangeCaretPosition(hit_test_result);
                selected_range_ = TextRange::FromTwoSides(hit_test_result, mouse_down_position_);
                InvalidateDraw();
            }
            UpdateCursor(args.GetPoint(this, RectRange::Margin));
        });


        mouse_up_event.bubble.AddHandler([this](events::MouseButtonEventArgs& args)
        {
            if (args.GetMouseButton() == MouseButton::Left)
            {
                if (is_selecting_)
                {
                    is_selecting_ = false;
                    GetWindow()->ReleaseCurrentMouseCapture();
                }
            }
        });

        lose_focus_event.direct.AddHandler([this](events::FocusChangeEventArgs& args)
        {
            if (is_selecting_)
            {
                is_selecting_ = false;
                GetWindow()->ReleaseCurrentMouseCapture();
            }
            if (!args.IsWindow()) // If the focus lose is triggered window-wide, then save the selection state. Otherwise, clear selection.
            {
                selected_range_ = std::nullopt;
                InvalidateDraw();
            }
        });
    }


    void TextControl::SetText(const String& text)
    {
        if (text_ != text)
        {
            const auto old_text = text_;
            text_ = text;
            OnTextChangedCore(old_text, text);
        }
    }

    void TextControl::SetBrush(const Microsoft::WRL::ComPtr<ID2D1Brush>& brush)
    {
        brush_ = brush;
        InvalidateDraw();
    }

    void TextControl::SetTextFormat(const Microsoft::WRL::ComPtr<IDWriteTextFormat>& text_format)
    {
        text_format_ = text_format;
        RecreateTextLayout();
        InvalidateDraw();
    }

    void TextControl::SetSelectable(const bool is_selectable)
    {
        if (is_selectable_ != is_selectable)
        {
            if (!is_selectable)
            {
                if (is_selecting_)
                {
                    is_selecting_ = false;
                    GetWindow()->ReleaseCurrentMouseCapture();
                }
                selected_range_ = std::nullopt;
                InvalidateDraw();
            }
            is_selectable_ = is_selectable;
            UpdateCursor(std::nullopt);
        }
    }

    void TextControl::SetSelectedRange(std::optional<TextRange> text_range)
    {
        if (is_selectable_)
        {
            selected_range_ = text_range;
            InvalidateDraw();
        }
    }



    void TextControl::RequestChangeCaretPosition(unsigned position)
    {

    }

    void TextControl::OnRectChange(const Rect& old_rect, const Rect& new_rect)
    {
        const auto content = GetRect(RectRange::Content);
        ThrowIfFailed(text_layout_->SetMaxWidth(content.width));
        ThrowIfFailed(text_layout_->SetMaxHeight(content.height));
    }

    void TextControl::OnTextChangedCore(const String& old_text, const String& new_text)
    {
        RecreateTextLayout();
        InvalidateLayout();
        InvalidateDraw();
    }

    void TextControl::UpdateCursor(const std::optional<Point>& point)
    {
        if (!is_selectable_)
        {
            SetCursor(nullptr);
            return;
        }

        const auto window = GetWindow();
        if (window == nullptr)
        {
            SetCursor(nullptr);
            return;
        }

        if (is_selecting_)
        {
            SetCursor(cursors::i_beam);
            return;
        }

        const auto p = point.value_or(WindowToControl(window->GetMousePosition()));
        if (GetRect(RectRange::Padding).IsPointInside(p))
            SetCursor(cursors::i_beam);
        else
            SetCursor(nullptr);
    }
}
