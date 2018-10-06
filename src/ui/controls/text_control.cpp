#include "text_control.h"

#include "ui/window.h"
#include "graph/graph.h"
#include "exception.h"

namespace cru::ui::controls
{
    using graph::CreateSolidBrush;

    inline Microsoft::WRL::ComPtr<IDWriteFactory> GetDWriteFactory()
    {
        return graph::GraphManager::GetInstance()->GetDWriteFactory();
    }

    TextControl::TextControl(const Microsoft::WRL::ComPtr<IDWriteTextFormat>& init_text_format,
        const Microsoft::WRL::ComPtr<ID2D1Brush>& init_brush) : Control(false)
    {
        text_format_ = init_text_format == nullptr ? graph::CreateDefaultTextFormat() : init_text_format;

        RecreateTextLayout();

        brush_ = init_brush == nullptr ? CreateSolidBrush(D2D1::ColorF(D2D1::ColorF::Black)) : init_brush;

        selection_brush_ = CreateSolidBrush(D2D1::ColorF(D2D1::ColorF::LightSkyBlue));
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
        Repaint();
    }

    void TextControl::SetTextFormat(const Microsoft::WRL::ComPtr<IDWriteTextFormat>& text_format)
    {
        text_format_ = text_format;
        RecreateTextLayout();
        Repaint();
    }

    void TextControl::AddTextLayoutHandler(TextLayoutHandlerPtr handler)
    {
        text_layout_handlers_.push_back(std::move(handler));
    }

    void TextControl::RemoveTextLayoutHandler(const TextLayoutHandlerPtr& handler)
    {
        const auto find_result = std::find(text_layout_handlers_.cbegin(), text_layout_handlers_.cend(),
            handler);
        if (find_result != text_layout_handlers_.cend())
            text_layout_handlers_.erase(find_result);
    }

    void TextControl::SetSelectable(const bool is_selectable)
    {
        if (!is_selectable)
        {
            if (is_selecting_)
            {
                is_selecting_ = false;
                GetWindow()->ReleaseCurrentMouseCapture();
            }
            selected_range_ = std::nullopt;
            Repaint();
        }
        is_selectable_ = is_selectable;
    }

    void TextControl::SetSelectedRange(std::optional<TextRange> text_range)
    {
        if (is_selectable_)
        {
            selected_range_ = text_range;
            Repaint();
        }
    }

    void TextControl::OnSizeChangedCore(events::SizeChangedEventArgs& args)
    {
        Control::OnSizeChangedCore(args);
        ThrowIfFailed(text_layout_->SetMaxWidth(args.GetNewSize().width));
        ThrowIfFailed(text_layout_->SetMaxHeight(args.GetNewSize().height));
        Repaint();
    }

    namespace
    {
        std::optional<unsigned> TextLayoutHitTest(IDWriteTextLayout* text_layout, const Point& point, const bool test_inside = true)
        {
            BOOL is_trailing, is_inside;
            DWRITE_HIT_TEST_METRICS metrics{};
            text_layout->HitTestPoint(point.x, point.y, &is_trailing, &is_inside, &metrics);
            if (!test_inside || is_inside)
                return is_trailing == 0 ? metrics.textPosition : metrics.textPosition + 1;
            else
                return std::nullopt;
        }

        void DrawSelectionRect(ID2D1DeviceContext* device_context, IDWriteTextLayout* layout, ID2D1Brush* brush, const std::optional<TextRange> range)
        {
            if (range.has_value())
            {
                DWRITE_TEXT_METRICS text_metrics{};
                ThrowIfFailed(layout->GetMetrics(&text_metrics));
                const auto metrics_count = text_metrics.lineCount * text_metrics.maxBidiReorderingDepth;

                Vector<DWRITE_HIT_TEST_METRICS> hit_test_metrics(metrics_count);
                UINT32 actual_count;
                layout->HitTestTextRange(
                    range.value().position, range.value().count,
                    0, 0,
                    hit_test_metrics.data(), metrics_count, &actual_count
                );

                hit_test_metrics.erase(hit_test_metrics.cbegin() + actual_count, hit_test_metrics.cend());

                for (const auto& metrics : hit_test_metrics)
                    device_context->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(metrics.left, metrics.top, metrics.left + metrics.width, metrics.top + metrics.height), 3, 3), brush);
            }
        }
    }
    void TextControl::OnDrawContent(ID2D1DeviceContext* device_context)
    {
        Control::OnDrawContent(device_context);
        DrawSelectionRect(device_context, text_layout_.Get(), selection_brush_.Get(), selected_range_);
        device_context->DrawTextLayout(D2D1::Point2F(), text_layout_.Get(), brush_.Get());
    }

    void TextControl::OnMouseDownCore(events::MouseButtonEventArgs& args)
    {
        Control::OnMouseDownCore(args);
        if (is_selectable_ && args.GetMouseButton() == MouseButton::Left)
        {
            selected_range_ = std::nullopt;
            const auto hit_test_result = TextLayoutHitTest(text_layout_.Get(), args.GetPoint(this), true);
            if (hit_test_result.has_value())
            {
                RequestChangeCaretPosition(hit_test_result.value());
                mouse_down_position_ = hit_test_result.value();
                is_selecting_ = true;
                GetWindow()->CaptureMouseFor(this);
            }
            Repaint();
        }
    }

    void TextControl::OnMouseMoveCore(events::MouseEventArgs& args)
    {
        Control::OnMouseMoveCore(args);
        if (is_selecting_)
        {
            const auto hit_test_result = TextLayoutHitTest(text_layout_.Get(), args.GetPoint(this), false).value();
            RequestChangeCaretPosition(hit_test_result);
            if (hit_test_result > mouse_down_position_)
            {
                selected_range_ = TextRange(mouse_down_position_, hit_test_result - mouse_down_position_);
            }
            else if (hit_test_result < mouse_down_position_)
            {
                selected_range_ = TextRange(hit_test_result, mouse_down_position_ - hit_test_result);
            }
            else
            {
                selected_range_ = std::nullopt;
            }
            Repaint();
        }
    }

    void TextControl::OnMouseUpCore(events::MouseButtonEventArgs& args)
    {
        Control::OnMouseUpCore(args);
        if (args.GetMouseButton() == MouseButton::Left)
        {
            if (is_selecting_)
            {
                is_selecting_ = false;
                GetWindow()->ReleaseCurrentMouseCapture();
            }
        }
    }

    void TextControl::OnLoseFocusCore(events::FocusChangeEventArgs& args)
    {
        Control::OnLoseFocusCore(args);
        if (is_selecting_)
        {
            is_selecting_ = false;
            GetWindow()->ReleaseCurrentMouseCapture();
        }
        if (!args.IsWindow()) // If the focus lose is triggered window-wide, then save the selection state. Otherwise, clear selection.
        {
            selected_range_ = std::nullopt;
            Repaint();
        }
    }


    Size TextControl::OnMeasureContent(const Size& available_size)
    {
        ThrowIfFailed(text_layout_->SetMaxWidth(available_size.width));
        ThrowIfFailed(text_layout_->SetMaxHeight(available_size.height));

        DWRITE_TEXT_METRICS metrics{};

        ThrowIfFailed(text_layout_->GetMetrics(&metrics));

        const Size measure_result(metrics.width, metrics.height);

        return measure_result;
    }

    void TextControl::RequestChangeCaretPosition(unsigned position)
    {

    }

    void TextControl::OnTextChangedCore(const String& old_text, const String& new_text)
    {
        RecreateTextLayout();
        InvalidateLayout();
        Repaint();
    }

    void TextControl::RecreateTextLayout()
    {
        assert(text_format_ != nullptr);

        text_layout_ = nullptr;

        const auto dwrite_factory = GetDWriteFactory();

        const auto&& size = GetSize();

        ThrowIfFailed(dwrite_factory->CreateTextLayout(
            text_.c_str(), static_cast<UINT32>(text_.size()),
            text_format_.Get(),
            size.width, size.height,
            &text_layout_
        ));

        for (const auto& handler : text_layout_handlers_)
            (*handler)(text_layout_);
    }
}
