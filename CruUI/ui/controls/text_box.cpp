#include "text_box.h"

#include "graph/graph.h"
#include "exception.h"

namespace cru::ui::controls
{
    using graph::CreateSolidBrush;

    inline Microsoft::WRL::ComPtr<IDWriteFactory> GetDWriteFactory()
    {
        return graph::GraphManager::GetInstance()->GetDWriteFactory();
    }

    TextBox::TextBox(const Microsoft::WRL::ComPtr<IDWriteTextFormat>& init_text_format,
        const Microsoft::WRL::ComPtr<ID2D1Brush>& init_brush) : Control(false)
    {
        text_format_ = init_text_format;

        if (init_brush == nullptr)
            brush_ = CreateSolidBrush(D2D1::ColorF(D2D1::ColorF::Black));
        else
            brush_ = init_brush;

        caret_brush_ = CreateSolidBrush(D2D1::ColorF(D2D1::ColorF::Black));

        //selection_brush_ = CreateSolidBrush(D2D1::ColorF(D2D1::ColorF::LightSkyBlue));
    }

    TextBox::~TextBox() = default;

    void TextBox::SetText(const String& text)
    {
        if (text_ != text)
        {
            const auto old_text = text_;
            text_ = text;
            OnTextChangedCore(old_text, text);
        }
    }

    void TextBox::SetBrush(const Microsoft::WRL::ComPtr<ID2D1Brush>& brush)
    {
        brush_ = brush;
        Repaint();
    }

    void TextBox::SetTextFormat(const Microsoft::WRL::ComPtr<IDWriteTextFormat>& text_format)
    {
        text_format_ = text_format;
        RecreateTextLayout();
        Repaint();
    }

    void TextBox::OnSizeChangedCore(events::SizeChangedEventArgs& args)
    {
        Control::OnSizeChangedCore(args);
        text_layout_->SetMaxWidth(args.GetNewSize().width);
        text_layout_->SetMaxHeight(args.GetNewSize().height);
        Repaint();
    }

    void TextBox::OnDraw(ID2D1DeviceContext* device_context)
    {
        Control::OnDraw(device_context);
        if (text_layout_ != nullptr)
        {
            device_context->DrawTextLayout(D2D1::Point2F(), text_layout_.Get(), brush_.Get());
        }
    }

    namespace
    {
        std::optional<unsigned> TextLayoutHitTest(IDWriteTextLayout* text_layout, const Point& point, bool test_inside = true)
        {
            BOOL is_trailing, is_inside;
            DWRITE_HIT_TEST_METRICS metrics{};
            text_layout->HitTestPoint(point.x, point.y, &is_trailing, &is_inside, &metrics);
            if (!test_inside || is_inside)
                return is_trailing == 0 ? metrics.textPosition : metrics.textPosition + 1;
            else
                return std::nullopt;
        }
    }

    void TextBox::OnMouseDownCore(events::MouseButtonEventArgs& args)
    {
        Control::OnMouseDownCore(args);
        if (args.GetMouseButton() == MouseButton::Left)
        {
            position_ = TextLayoutHitTest(text_layout_.Get(), args.GetPoint(this), false).value();

            Repaint();
        }
    }

    void TextBox::OnGetFocusCore(events::FocusChangeEventArgs& args)
    {
        Control::OnGetFocusCore(args);
        assert(caret_timer_ == nullptr);
        caret_timer_ = SetInterval(Application::GetInstance()->GetCaretBlinkDuration(), caret_action_);
    }

    void TextBox::OnLoseFocusCore(events::FocusChangeEventArgs& args)
    {
        Control::OnLoseFocusCore(args);
        assert(caret_timer_ != nullptr);
        caret_timer_->Cancel();
    }

    Size TextBox::OnMeasure(const Size& available_size)
    {
        if (text_.empty())
            return Size::Zero();

        const auto layout_params = GetLayoutParams();

        if (layout_params->width.mode == MeasureMode::Stretch && layout_params->height.mode == MeasureMode::Stretch)
            return available_size;

        auto&& get_measure_length = [](const LayoutSideParams& layout_length, const float available_length) -> float
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

        const Size measure_size(get_measure_length(layout_params->width, available_size.width),
            get_measure_length(layout_params->height, available_size.height));

        ThrowIfFailed(text_layout_->SetMaxWidth(measure_size.width));
        ThrowIfFailed(text_layout_->SetMaxHeight(measure_size.height));

        DWRITE_TEXT_METRICS metrics{};

        ThrowIfFailed(text_layout_->GetMetrics(&metrics));

        const Size measure_result(metrics.width, metrics.height);

        auto&& calculate_final_length = [](const LayoutSideParams& layout_length, const float measure_length, const float measure_result_length) -> float
        {
            if ((layout_length.mode == MeasureMode::Stretch ||
                layout_length.mode == MeasureMode::Exactly)
                && measure_result_length < measure_length)
                return measure_length;
            else
                return measure_result_length;
        };

        const Size result_size(
            calculate_final_length(layout_params->width, measure_size.width, measure_result.width),
            calculate_final_length(layout_params->height, measure_size.height, measure_result.height)
        );

        return result_size;
    }

    void TextBox::OnTextChangedCore(const String& old_text, const String& new_text)
    {
        RecreateTextLayout();
        Repaint();
    }

    void TextBox::RecreateTextLayout()
    {
        if (text_.empty())
        {
            text_layout_ = nullptr;
            return;
        }

        const auto dwrite_factory = GetDWriteFactory();

        if (text_format_ == nullptr)
            text_format_ = graph::CreateDefaultTextFormat();

        const auto&& size = GetSize();

        ThrowIfFailed(dwrite_factory->CreateTextLayout(
            text_.c_str(), static_cast<UINT32>(text_.size()),
            text_format_.Get(),
            size.width, size.height,
            &text_layout_
        ));
    }
}
