#pragma once

#include "system_headers.hpp"

#include "any_map.h"
#include "application.hpp"

namespace cru::ui::predefine
{
#define CRU_DEFINE_KEY(name, key) \
    constexpr const wchar_t* key_##name = L#key;

    //region BorderProperty
    CRU_DEFINE_KEY(border_property_brush, BorderProperty_Brush)

    //region Button
    CRU_DEFINE_KEY(button_normal_border, Button_Normal_Border)
    CRU_DEFINE_KEY(button_press_border, Button_Press_Border)

    //region TextControl
    CRU_DEFINE_KEY(text_control_selection_brush, TextControl_SelectionBrush)

    //region TextBox
    CRU_DEFINE_KEY(text_box_border, TextBox_Border)
    CRU_DEFINE_KEY(text_box_text_brush, TextBox_TextBrush)
    CRU_DEFINE_KEY(text_box_text_format, TextBox_TextFormat)
    CRU_DEFINE_KEY(text_box_caret_brush, TextBox_CaretBrush)

    //region TextBlock
    CRU_DEFINE_KEY(text_block_text_brush, TextBlock_TextBrush)
    CRU_DEFINE_KEY(text_block_text_format, TextBlock_TextFormat)

    //region ToggleButton
    CRU_DEFINE_KEY(toggle_button_on_brush, ToggleButton_On_Brush)
    CRU_DEFINE_KEY(toggle_button_off_brush, ToggleButton_Off_Brush)

#ifdef CRU_DEBUG_LAYOUT
    //region debug
    CRU_DEFINE_KEY(debug_layout_out_border_brush, Debug_Layout_Out_Border_Brush)
    CRU_DEFINE_KEY(debug_layout_margin_brush, Debug_Layout_Margin_Brush)
    CRU_DEFINE_KEY(debug_layout_padding_brush, Debug_Layout_Padding_Brush)
#endif

#undef  CRU_DEFINE_KEY

    void InitThemes(AnyMap* resource_map);

    template<typename T>
    T GetPredefineResource(const String& key)
    {
        return Application::GetInstance()->GetPredefineResourceMap()->GetValue<T>(key);
    }

    template<typename T>
    Microsoft::WRL::ComPtr<T> GetPredefineResourceComPtr(const String& key)
    {
        return GetPredefineResource<Microsoft::WRL::ComPtr<T>>(key);
    }
}
