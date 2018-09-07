#include "text_block.h"

#include "ui/window.h"
#include "graph/graph.h"
#include "exception.h"

namespace cru
{
    namespace ui
    {
        namespace controls
        {
            inline Microsoft::WRL::ComPtr<IDWriteFactory> GetDWriteFactory()
            {
                return graph::GraphManager::GetInstance()->GetDWriteFactory();
            }

            TextBlock::TextBlock(const Microsoft::WRL::ComPtr<IDWriteTextFormat>& init_text_format,
                const Microsoft::WRL::ComPtr<ID2D1Brush>& init_brush)
            {
                text_format_ = init_text_format;
                if (init_brush == nullptr)
                    CreateDefaultBrush();
            }

            TextBlock::~TextBlock() = default;

            void TextBlock::SetText(const String& text)
            {
                const auto old_text = text_;
                text_ = text;
                OnTextChangedCore(old_text, text);
            }

            void TextBlock::SetBrush(const Microsoft::WRL::ComPtr<ID2D1Brush>& brush)
            {
                brush_ = brush;
                Repaint();
            }

            void TextBlock::SetTextFormat(const Microsoft::WRL::ComPtr<IDWriteTextFormat>& text_format)
            {
                text_format_ = text_format;
                RecreateTextLayout();
                Repaint();
            }

            void TextBlock::OnSizeChangedCore(events::SizeChangedEventArgs& args)
            {
                RecreateTextLayout();
                Repaint();
            }

            void TextBlock::OnDraw(ID2D1DeviceContext* device_context)
            {
                device_context->DrawTextLayout(D2D1::Point2F(), text_layout_.Get(), brush_.Get());
            }

            Size TextBlock::OnMeasure(const Size& available_size)
            {
                if (text_.empty())
                    return Size::zero;

                const auto layout_params = GetLayoutParams();


                if (layout_params->width.mode == MeasureMode::Stretch && layout_params->height.mode == MeasureMode::Stretch)
                    return available_size;

                Size measure_size;

                auto&& get_measure_length = [](const MeasureLength& layout_length, const float available_length) -> float
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

                measure_size.width = get_measure_length(layout_params->width, available_size.width);
                measure_size.height = get_measure_length(layout_params->height, available_size.height);


                Microsoft::WRL::ComPtr<IDWriteTextLayout> measure_text_layout;

                const auto dwrite_factory = GetDWriteFactory();

                dwrite_factory->CreateTextLayout(text_.c_str(), text_.size(),
                    text_format_.Get(), measure_size.width, measure_size.height, &measure_text_layout);

                DWRITE_TEXT_METRICS metrics{};
                measure_text_layout->GetMetrics(&metrics);

                const Size measure_result(metrics.width, metrics.height);

                auto&& calculate_final_length = [](const MeasureLength& layout_length, const float measure_length, const float measure_result_length) -> float
                {
                    if ((layout_length.mode == MeasureMode::Stretch ||
                        layout_length.mode == MeasureMode::Exactly)
                        && measure_result_length < measure_length)
                        return measure_length;
                    else
                        return measure_result_length;
                };

                return Size(
                    calculate_final_length(layout_params->width, measure_size.width, measure_result.width),
                    calculate_final_length(layout_params->height, measure_size.height, measure_result.height)
                );
            }

            void TextBlock::OnTextChangedCore(const String& old_text, const String& new_text)
            {
                RecreateTextLayout();
                Repaint();
            }

            void TextBlock::CreateDefaultBrush()
            {
                const auto device_context = graph::GraphManager::GetInstance()->GetD2D1DeviceContext();
                Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> solid_color_brush;
                device_context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &solid_color_brush);
                brush_ = solid_color_brush;
            }

            void TextBlock::CreateDefaultTextFormat()
            {
                const auto dwrite_factory = GetDWriteFactory();

                ThrowIfFailed(dwrite_factory->CreateTextFormat(
                    L"µÈÏß", nullptr,
                    DWRITE_FONT_WEIGHT_NORMAL,
                    DWRITE_FONT_STYLE_NORMAL,
                    DWRITE_FONT_STRETCH_NORMAL,
                    24.0, L"zh-cn",
                    &text_format_
                ));
            }

            void TextBlock::RecreateTextLayout()
            {
                if (text_.empty())
                {
                    text_layout_ = nullptr;
                    return;
                }

                const auto dwrite_factory = GetDWriteFactory();

                if (text_format_ == nullptr)
                    CreateDefaultTextFormat();

                const auto&& size = GetSize();

                dwrite_factory->CreateTextLayout(
                    text_.c_str(), text_.size(),
                    text_format_.Get(),
                    size.width, size.height,
                    &text_layout_
                );

                std::for_each(text_layout_handlers_.cbegin(), text_layout_handlers_.cend(), [this](const std::shared_ptr<TextLayoutHandler>& handler)
                {
                    (*handler)(text_layout_);
                });
            }
        }
    }
}
