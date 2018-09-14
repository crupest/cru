#pragma once

#include <memory>

#include "ui/control.h"

namespace cru
{
    namespace ui
    {
        namespace controls
        {
            struct TextRange
            {
                TextRange() = default;
                TextRange(const int position, const int count)
                    : position(position), count(count)
                {
                    
                }
                TextRange(const TextRange& other) = default;
                TextRange(TextRange&& other) = default;
                TextRange& operator=(const TextRange& other) = default;
                TextRange& operator=(TextRange&& other) = default;
                ~TextRange() = default;

                unsigned position;
                unsigned count;

                explicit operator DWRITE_TEXT_RANGE() const
                {
                    DWRITE_TEXT_RANGE result;
                    result.startPosition = position;
                    result.length = count;
                    return result;
                }
            };

            class TextBlock : public Control
            {
            public:
                using TextLayoutHandler = Action<Microsoft::WRL::ComPtr<IDWriteTextLayout>>;

                static TextBlock* Create(
                    const String& text = L"",
                    const Microsoft::WRL::ComPtr<IDWriteTextFormat>& init_text_format = nullptr,
                    const Microsoft::WRL::ComPtr<ID2D1Brush>& init_brush = nullptr)
                {
                    const auto text_block = new TextBlock(init_text_format, init_brush);
                    text_block->SetText(text);
                    return text_block;
                }

            private:
                explicit TextBlock(
                    const Microsoft::WRL::ComPtr<IDWriteTextFormat>& init_text_format = nullptr,
                    const Microsoft::WRL::ComPtr<ID2D1Brush>& init_brush = nullptr
                );
            public:
                TextBlock(const TextBlock& other) = delete;
                TextBlock(TextBlock&& other) = delete;
                TextBlock& operator=(const TextBlock& other) = delete;
                TextBlock& operator=(TextBlock&& other) = delete;
                ~TextBlock() override;

                String GetText() const
                {
                    return text_;
                }

                void SetText(const String& text);

                Microsoft::WRL::ComPtr<ID2D1Brush> GetBrush() const
                {
                    return brush_;
                }

                void SetBrush(const Microsoft::WRL::ComPtr<ID2D1Brush>& brush);

                Microsoft::WRL::ComPtr<IDWriteTextFormat> GetTextFormat() const
                {
                    return text_format_;
                }

                void SetTextFormat(const Microsoft::WRL::ComPtr<IDWriteTextFormat>& text_format);


                void AddTextLayoutHandler(std::shared_ptr<TextLayoutHandler> handler)
                {
                    text_layout_handlers_.push_back(std::move(handler));
                }
                void RemoveTextLayoutHandler(const std::shared_ptr<TextLayoutHandler>& handler)
                {
                    const auto find_result = std::find(text_layout_handlers_.cbegin(), text_layout_handlers_.cend(), handler);
                    if (find_result != text_layout_handlers_.cend())
                        text_layout_handlers_.erase(find_result);
                }

            protected:
                void OnSizeChangedCore(events::SizeChangedEventArgs& args) override final;
                void OnDraw(ID2D1DeviceContext* device_context) override;

                void OnMouseDownCore(events::MouseButtonEventArgs& args) override final;
                void OnMouseMoveCore(events::MouseEventArgs& args) override final;
                void OnMouseUpCore(events::MouseButtonEventArgs& args) override final;

                void OnLoseFocusCore(events::FocusChangeEventArgs& args) override final;

                Size OnMeasure(const Size& available_size) override final;

            private:
                void OnTextChangedCore(const String& old_text, const String& new_text);

                void CreateDefaultTextFormat();
                void RecreateTextLayout();

            private:
                String text_;

                Microsoft::WRL::ComPtr<ID2D1Brush> brush_;
                Microsoft::WRL::ComPtr<ID2D1Brush> selection_brush_;
                Microsoft::WRL::ComPtr<IDWriteTextFormat> text_format_;
                Microsoft::WRL::ComPtr<IDWriteTextLayout> text_layout_;

                Vector<std::shared_ptr<TextLayoutHandler>> text_layout_handlers_;

                bool is_selecting_ = false;
                unsigned mouse_down_position_ = 0;
                std::optional<TextRange> selected_range_ = std::nullopt;
            };
        }
    }
}
