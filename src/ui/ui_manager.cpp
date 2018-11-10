#include "ui_manager.hpp"

#include "application.hpp"
#include "border_property.hpp"
#include "graph/graph.hpp"
#include "exception.hpp"


namespace cru::ui
{
    namespace
    {
        void GetSystemCaretInfo(CaretInfo* caret_info)
        {
            caret_info->caret_blink_duration = std::chrono::milliseconds(::GetCaretBlinkTime());
            DWORD caret_width;
            if (!::SystemParametersInfoW(SPI_GETCARETWIDTH, 0 , &caret_width, 0))
                throw Win32Error(::GetLastError(), "Failed to get system caret width.");
            caret_info->half_caret_width = caret_width / 2.0f;
        }

        Microsoft::WRL::ComPtr<ID2D1Brush> CreateSolidBrush(graph::GraphManager* graph_manager, const D2D1_COLOR_F& color)
        {
            const auto device_context = graph_manager->GetD2D1DeviceContext();
            Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> solid_color_brush;
            device_context->CreateSolidColorBrush(color, &solid_color_brush);
            return solid_color_brush;
        }

        Microsoft::WRL::ComPtr<IDWriteTextFormat> CreateDefaultTextFormat(graph::GraphManager* graph_manager)
        {
            const auto dwrite_factory = graph_manager->GetDWriteFactory();

            Microsoft::WRL::ComPtr<IDWriteTextFormat> text_format;

            ThrowIfFailed(dwrite_factory->CreateTextFormat(
                L"等线", nullptr,
                DWRITE_FONT_WEIGHT_NORMAL,
                DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL,
                24.0, L"zh-cn",
                &text_format
            ));

            ThrowIfFailed(text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));
            ThrowIfFailed(text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER));

            return text_format;
        }
    }


    //!!! never use default constructor of border at here, because it will recursively call this method!
    PredefineResources::PredefineResources(graph::GraphManager* graph_manager) :
        border_property_brush           {CreateSolidBrush(graph_manager, D2D1::ColorF(D2D1::ColorF::Black))},

        button_normal_border            {CreateSolidBrush(graph_manager, D2D1::ColorF(D2D1::ColorF::RoyalBlue)), 2, 6, 6},
        button_press_border             {CreateSolidBrush(graph_manager, D2D1::ColorF(D2D1::ColorF::Blue)), 2, 6, 6},

        text_control_selection_brush    {CreateSolidBrush(graph_manager, D2D1::ColorF(D2D1::ColorF::LightSkyBlue))},

        text_box_border                 {CreateSolidBrush(graph_manager, D2D1::ColorF(D2D1::ColorF::Black))},
        text_box_text_brush             {CreateSolidBrush(graph_manager, D2D1::ColorF(D2D1::ColorF::Black))},
        text_box_text_format            {CreateDefaultTextFormat(graph_manager)},
        text_box_caret_brush            {CreateSolidBrush(graph_manager, D2D1::ColorF(D2D1::ColorF::Black))},

        text_block_text_brush           {CreateSolidBrush(graph_manager, D2D1::ColorF(D2D1::ColorF::Black))},
        text_block_text_format          {CreateDefaultTextFormat(graph_manager)},

        toggle_button_on_brush          {CreateSolidBrush(graph_manager, D2D1::ColorF(D2D1::ColorF::DeepSkyBlue))},
        toggle_button_off_brush         {CreateSolidBrush(graph_manager, D2D1::ColorF(D2D1::ColorF::LightGray))},

        list_item_normal_border_brush   {CreateSolidBrush(graph_manager, D2D1::ColorF(D2D1::ColorF::White, 0))},
        list_item_normal_fill_brush     {CreateSolidBrush(graph_manager, D2D1::ColorF(D2D1::ColorF::White, 0))},
        list_item_hover_border_brush    {CreateSolidBrush(graph_manager, D2D1::ColorF(D2D1::ColorF::SkyBlue))},
        list_item_hover_fill_brush      {CreateSolidBrush(graph_manager, D2D1::ColorF(D2D1::ColorF::SkyBlue, 0.3f))},
        list_item_select_border_brush   {CreateSolidBrush(graph_manager, D2D1::ColorF(D2D1::ColorF::MediumBlue))},
        list_item_select_fill_brush     {CreateSolidBrush(graph_manager, D2D1::ColorF(D2D1::ColorF::SkyBlue, 0.3f))}

#ifdef CRU_DEBUG_LAYOUT
        ,
        debug_layout_out_border_brush   {CreateSolidBrush(graph_manager, D2D1::ColorF(D2D1::ColorF::Crimson))},
        debug_layout_margin_brush       {CreateSolidBrush(graph_manager, D2D1::ColorF(D2D1::ColorF::LightCoral, 0.25f))},
        debug_layout_padding_brush      {CreateSolidBrush(graph_manager, D2D1::ColorF(D2D1::ColorF::SkyBlue, 0.25f))}
#endif
    {
        
    }

    UiManager* UiManager::GetInstance()
    {
        return Application::GetInstance()->ResolveSingleton<UiManager>([](auto)
        {
            return new UiManager{};
        });
    }

    UiManager::UiManager()
        : predefine_resources_(graph::GraphManager::GetInstance())
    {
        GetSystemCaretInfo(&caret_info_);
    }
}
