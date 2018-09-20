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
            using graph::CreateSolidBrush;

            inline Microsoft::WRL::ComPtr<IDWriteFactory> GetDWriteFactory()
            {
                return graph::GraphManager::GetInstance()->GetDWriteFactory();
            }

            TextBlock::TextBlock(const Microsoft::WRL::ComPtr<IDWriteTextFormat>& init_text_format,
                const Microsoft::WRL::ComPtr<ID2D1Brush>& init_brush) : Control(false)
            {
                text_format_ = init_text_format;
                if (init_brush == nullptr)
                    brush_ = CreateSolidBrush(D2D1::ColorF(D2D1::ColorF::Black));

                selection_brush_ = CreateSolidBrush(D2D1::ColorF(D2D1::ColorF::LightSkyBlue));
            }

            TextBlock::~TextBlock() = default;

            void TextBlock::SetText(const String& text)
            {
                if (text_ != text)
                {
                    const auto old_text = text_;
                    text_ = text;
                    OnTextChangedCore(old_text, text);
                }
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

            void TextBlock::AddTextLayoutHandler(TextLayoutHandlerPtr handler)
            {
                text_layout_handlers_.push_back(std::move(handler));
            }

            void TextBlock::RemoveTextLayoutHandler(const TextLayoutHandlerPtr& handler)
            {
                const auto find_result = std::find(text_layout_handlers_.cbegin(), text_layout_handlers_.cend(),
                                                   handler);
                if (find_result != text_layout_handlers_.cend())
                    text_layout_handlers_.erase(find_result);
            }

            void TextBlock::OnSizeChangedCore(events::SizeChangedEventArgs& args)
            {
                Control::OnSizeChangedCore(args);
                text_layout_->SetMaxWidth(args.GetNewSize().width);
                text_layout_->SetMaxHeight(args.GetNewSize().height);
                Repaint();
            }

            void TextBlock::OnDraw(ID2D1DeviceContext* device_context)
            {
                Control::OnDraw(device_context);
                if (text_layout_ != nullptr)
                {
                    if (selected_range_.has_value())
                    {
                        DWRITE_TEXT_METRICS text_metrics{};
                        ThrowIfFailed(text_layout_->GetMetrics(&text_metrics));
                        const auto metrics_count = text_metrics.lineCount * text_metrics.maxBidiReorderingDepth;

                        Vector<DWRITE_HIT_TEST_METRICS> hit_test_metrics(metrics_count);
                        UINT32 actual_count;
                        text_layout_->HitTestTextRange(
                            selected_range_.value().position, selected_range_.value().count,
                            0, 0,
                            hit_test_metrics.data(), metrics_count, &actual_count
                        );

                        hit_test_metrics.erase(hit_test_metrics.cbegin() + actual_count, hit_test_metrics.cend());

                        for (const auto& metrics : hit_test_metrics)
                        {
                            device_context->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(metrics.left, metrics.top, metrics.left + metrics.width, metrics.top + metrics.height), 3, 3), selection_brush_.Get());
                        }
                    }
                    device_context->DrawTextLayout(D2D1::Point2F(), text_layout_.Get(), brush_.Get());
                }
            }

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

            void TextBlock::OnMouseDownCore(events::MouseButtonEventArgs& args)
            {
                Control::OnMouseDownCore(args);
                if (args.GetMouseButton() == MouseButton::Left)
                {
                    RequestFocus();
                    selected_range_ = std::nullopt;
                    const auto hit_test_result = TextLayoutHitTest(text_layout_.Get(), args.GetPoint(this), true);
                    if (hit_test_result.has_value())
                    {
                        mouse_down_position_ = hit_test_result.value();
                        is_selecting_ = true;
                        GetWindow()->CaptureMouseFor(this);
                    }
                    Repaint();
                }
            }

            void TextBlock::OnMouseMoveCore(events::MouseEventArgs& args)
            {
                Control::OnMouseMoveCore(args);
                if (is_selecting_)
                {
                    const auto hit_test_result = TextLayoutHitTest(text_layout_.Get(), args.GetPoint(this), false).value();
                    if (hit_test_result > mouse_down_position_)
                        selected_range_ = TextRange(mouse_down_position_, hit_test_result - mouse_down_position_);
                    else if (hit_test_result < mouse_down_position_)
                        selected_range_ = TextRange(hit_test_result, mouse_down_position_ - hit_test_result);
                    else
                        selected_range_ = std::nullopt;
                    Repaint();
                }
            }

            void TextBlock::OnMouseUpCore(events::MouseButtonEventArgs& args)
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

            void TextBlock::OnLoseFocusCore(events::FocusChangeEventArgs& args)
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

            Size TextBlock::OnMeasure(const Size& available_size)
            {
                if (text_.empty())
                    return Size::Zero();

                const auto layout_params = GetLayoutParams();

                if (layout_params->width.mode == MeasureMode::Stretch && layout_params->height.mode == MeasureMode::Stretch)
                    return available_size;

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

                const Size measure_size(get_measure_length(layout_params->width, available_size.width),
                    get_measure_length(layout_params->height, available_size.height));

                ThrowIfFailed(text_layout_->SetMaxWidth(measure_size.width));
                ThrowIfFailed(text_layout_->SetMaxHeight(measure_size.height));

                DWRITE_TEXT_METRICS metrics{};

                ThrowIfFailed(text_layout_->GetMetrics(&metrics));

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

                const Size result_size(
                    calculate_final_length(layout_params->width, measure_size.width, measure_result.width),
                    calculate_final_length(layout_params->height, measure_size.height, measure_result.height)
                );

                return result_size;
            }

            void TextBlock::OnTextChangedCore(const String& old_text, const String& new_text)
            {
                RecreateTextLayout();
                Repaint();
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

                ThrowIfFailed(text_format_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));
                ThrowIfFailed(text_format_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER));
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

                ThrowIfFailed(dwrite_factory->CreateTextLayout(
                    text_.c_str(), static_cast<UINT32>(text_.size()),
                    text_format_.Get(),
                    size.width, size.height,
                    &text_layout_
                ));

                std::for_each(text_layout_handlers_.cbegin(), text_layout_handlers_.cend(), [this](const TextLayoutHandlerPtr& handler)
                {
                    (*handler)(text_layout_);
                });
            }
        }
    }
}
