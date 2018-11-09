#include "text_box.hpp"

#include <cwctype>
#include <cassert>

#include "graph/graph.hpp"
#include "exception.hpp"
#include "application.hpp"
#include "ui/predefine.hpp"

namespace cru::ui::controls
{
    inline Microsoft::WRL::ComPtr<IDWriteFactory> GetDWriteFactory()
    {
        return graph::GraphManager::GetInstance()->GetDWriteFactory();
    }

    TextBox::TextBox() : TextControl(
        predefine::GetPredefineResourceComPtr<IDWriteTextFormat>(predefine::key_text_box_text_format),
        predefine::GetPredefineResourceComPtr<ID2D1Brush>(predefine::key_text_box_text_brush)
    )
    {
        SetSelectable(true);

        caret_brush_ = predefine::GetPredefineResourceComPtr<ID2D1Brush>(predefine::key_text_box_caret_brush);

        GetBorderProperty() = predefine::GetPredefineResource<BorderProperty>(predefine::key_text_box_border);
        SetBordered(true);
    }

    TextBox::~TextBox() = default;

    StringView TextBox::GetControlType() const
    {
        return control_type;
    }

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
        assert(!caret_timer_.has_value());
        is_caret_show_ = true;
        caret_timer_ = SetInterval(Application::GetInstance()->GetCaretInfo().caret_blink_duration, [this]
        {
            is_caret_show_ = !is_caret_show_;
            Repaint();
        });
    }

    void TextBox::OnLoseFocusCore(events::FocusChangeEventArgs& args)
    {
        Control::OnLoseFocusCore(args);
        assert(caret_timer_.has_value());
        caret_timer_->Cancel();
        caret_timer_ = std::nullopt;
        is_caret_show_ = false;
    }

    void TextBox::OnKeyDownCore(events::KeyEventArgs& args)
    {
        Control::OnKeyDownCore(args);
        if (args.GetVirtualCode() == VK_LEFT && caret_position_ > 0)
        {
            if (IsKeyDown(VK_SHIFT))
            {
                if (GetCaretSelectionSide())
                    ShiftLeftSelectionRange(-1);
                else
                    ShiftRightSelectionRange(-1);
            }
            else
            {
                const auto selection = GetSelectedRange();
                if (selection.has_value())
                {
                    ClearSelection();
                    caret_position_ = selection.value().position;
                }
                else
                    caret_position_--;
            }
            Repaint();
        }

        if (args.GetVirtualCode() == VK_RIGHT && caret_position_ < GetText().size())
        {
            if (IsKeyDown(VK_SHIFT))
            {
                if (GetCaretSelectionSide())
                    ShiftLeftSelectionRange(1);
                else
                    ShiftRightSelectionRange(1);
            }
            else
            {
                const auto selection = GetSelectedRange();
                if (selection.has_value())
                {
                    ClearSelection();
                    caret_position_ = selection.value().position + selection.value().count;
                }
                else
                    caret_position_++;
            }
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

    bool TextBox::GetCaretSelectionSide() const
    {
        const auto selection = TextRange::ToTwoSides(GetSelectedRange(), caret_position_);
        if (selection.first == caret_position_)
            return true;
        if (selection.second == caret_position_)
            return false;
        assert(false);
        return true;
    }

    void TextBox::ShiftLeftSelectionRange(const int count)
    {
        const auto selection_range_side = TextRange::ToTwoSides(GetSelectedRange(), caret_position_);
        int new_left = selection_range_side.first + count;
        new_left = new_left < 0 ? 0 : new_left; // at least 0
        caret_position_ = new_left;
        SetSelectedRange(TextRange::FromTwoSides(static_cast<unsigned>(new_left), selection_range_side.second));
    }

    void TextBox::ShiftRightSelectionRange(const int count)
    {
        const auto selection_range_side = TextRange::ToTwoSides(GetSelectedRange(), caret_position_);
        int new_right = selection_range_side.second + count;
        new_right = new_right < 0 ? 0 : new_right; // at least 0
        caret_position_ = new_right;
        SetSelectedRange(TextRange::FromTwoSides(selection_range_side.first, static_cast<unsigned>(new_right)));
    }
}
