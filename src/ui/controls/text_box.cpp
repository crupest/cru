#include "text_box.h"

#include <cwctype>

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
        const Microsoft::WRL::ComPtr<ID2D1Brush>& init_brush) : TextControl(init_text_format, init_brush)
    {
        SetSelectable(true);

        caret_brush_ = CreateSolidBrush(D2D1::ColorF(D2D1::ColorF::Black));

        caret_action_ = CreateActionPtr([this]
        {
            is_caret_show_ = !is_caret_show_;
            Repaint();
        });

        SetBordered(true);
    }

    TextBox::~TextBox() = default;

    void TextBox::OnDrawContent(ID2D1DeviceContext* device_context)
    {
        TextControl::OnDrawContent(device_context);
        if (is_caret_show_)
        {
            const auto caret_half_width = Application::GetInstance()->GetCaretInfo().half_caret_width;
            FLOAT x, y;
            DWRITE_HIT_TEST_METRICS metrics{};
            ThrowIfFailed(text_layout_->HitTestTextPosition(caret_position_, FALSE, &x, &y, &metrics));
            device_context->FillRectangle(D2D1::RectF(metrics.left - caret_half_width, metrics.top, metrics.left + caret_half_width, metrics.top + metrics.height), caret_brush_.Get());
        }
    }

    void TextBox::OnGetFocusCore(events::FocusChangeEventArgs& args)
    {
        TextControl::OnGetFocusCore(args);
        assert(caret_timer_ == nullptr);
        is_caret_show_ = true;
        caret_timer_ = SetInterval(Application::GetInstance()->GetCaretInfo().caret_blink_duration, caret_action_);
    }

    void TextBox::OnLoseFocusCore(events::FocusChangeEventArgs& args)
    {
        Control::OnLoseFocusCore(args);
        assert(caret_timer_ != nullptr);
        caret_timer_->Cancel();
        caret_timer_ = nullptr;
        is_caret_show_ = false;
    }

    void TextBox::OnKeyDownCore(events::KeyEventArgs& args)
    {
        Control::OnKeyDownCore(args);
        if (args.GetVirtualCode() == VK_LEFT && caret_position_ > 0)
        {
            ClearSelection();
            caret_position_--;
            Repaint();
        }

        if (args.GetVirtualCode() == VK_RIGHT && caret_position_ < GetText().size())
        {
            ClearSelection();
            caret_position_++;
            Repaint();
        }
    }

    void TextBox::OnCharCore(events::CharEventArgs& args)
    {
        Control::OnCharCore(args);
        if (args.GetChar() == L'\b')
        {
            if (GetSelectedRange().has_value())
            {
                const auto selection_range = GetSelectedRange().value();
                auto text = GetText();
                text.erase(text.cbegin() + selection_range.position, text.cbegin() + selection_range.position + selection_range.count);
                SetText(text);
                caret_position_ = selection_range.position;
                ClearSelection();
            }
            else
            {
                if (caret_position_ > 0)
                {
                    auto text = GetText();
                    if (!text.empty())
                    {
                        const auto position = --caret_position_;
                        text.erase(text.cbegin() + position);
                        SetText(text);
                    }
                }
            }
            return;
        }

        if (std::iswprint(args.GetChar()))
        {
            if (GetSelectedRange().has_value())
            {
                const auto selection_range = GetSelectedRange().value();
                auto text = GetText();
                text.erase(selection_range.position, selection_range.count);
                text.insert(text.cbegin() + selection_range.position, args.GetChar());
                SetText(text);
                caret_position_ = selection_range.position + 1;
                ClearSelection();
            }
            else
            {
                ClearSelection();
                const auto position = caret_position_++;
                auto text = GetText();
                text.insert(text.cbegin() + position, { args.GetChar() });
                SetText(text);
            }
        }
    }

    void TextBox::RequestChangeCaretPosition(const unsigned position)
    {
        caret_position_ = position;
        Repaint();
    }
}
