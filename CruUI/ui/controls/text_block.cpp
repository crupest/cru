#include "text_block.h"

#include "ui/window.h"
#include "graph/graph.h"

namespace cru
{
    namespace ui
    {
        namespace controls
        {
            TextBlock::TextBlock()
            {
                const auto device_context = graph::GraphManager::GetInstance()->GetD2D1DeviceContext();

                Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> solid_color_brush;

                device_context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &solid_color_brush);

                brush_ = solid_color_brush;
            }

            TextBlock::~TextBlock()
            {

            }

            void TextBlock::SetText(const String& text)
            {
                const auto old_text = text_;
                text_ = text;
                OnTextChangedCore(old_text, text);
            }

            void TextBlock::OnSizeChangedCore(events::SizeChangedEventArgs& args)
            {
                CreateTextLayout();
                const auto window = GetWindow();
                if (window != nullptr)
                    window->Repaint();
            }

            void TextBlock::OnDraw(ID2D1DeviceContext* device_context)
            {
                device_context->DrawTextLayout(D2D1::Point2F(), text_layout_.Get(), brush_.Get());
            }

            void TextBlock::OnTextChangedCore(const String& old_text, const String& new_text)
            {
                CreateTextLayout();
                const auto window = GetWindow();
                if (window != nullptr)
                    window->Repaint();
            }

            void TextBlock::CreateTextLayout()
            {
                auto dwrite_factory = graph::GraphManager::GetInstance()->GetDWriteFactory();

                Microsoft::WRL::ComPtr<IDWriteTextFormat> text_format;

                dwrite_factory->CreateTextFormat(
                    L"µÈÏß", nullptr,
                    DWRITE_FONT_WEIGHT_NORMAL,
                    DWRITE_FONT_STYLE_NORMAL,
                    DWRITE_FONT_STRETCH_NORMAL,
                    24.0, L"zh-cn",
                    &text_format
                );

                const auto&& size = GetSize();

                dwrite_factory->CreateTextLayout(
                    text_.c_str(), text_.size(),
                    text_format.Get(),
                    size.width, size.height,
                    &text_layout_
                );
            }
        }
    }
}
