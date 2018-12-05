#include "pre.hpp"

#include "application.hpp"
#include "ui/window.hpp"
#include "ui/controls/linear_layout.hpp"
#include "ui/controls/text_block.hpp"
#include "ui/controls/toggle_button.hpp"
#include "ui/controls/button.hpp"
#include "ui/controls/text_box.hpp"
#include "ui/controls/list_item.hpp"
#include "ui/controls/popup_menu.hpp"
#include "ui/controls/frame_layout.hpp"
#include "ui/controls/scroll_control.hpp"
#include "graph/graph.hpp"

using cru::String;
using cru::StringView;
using cru::Application;
using cru::ui::Rect;
using cru::ui::Window;
using cru::ui::Alignment;
using cru::ui::LayoutSideParams;
using cru::ui::Thickness;
using cru::ui::ControlList;
using cru::ui::CreateWithLayout;
using cru::ui::controls::LinearLayout;
using cru::ui::controls::TextBlock;
using cru::ui::controls::ToggleButton;
using cru::ui::controls::Button;
using cru::ui::controls::TextBox;
using cru::ui::controls::ListItem;
using cru::ui::controls::FrameLayout;
using cru::ui::controls::ScrollControl;

int APIENTRY wWinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPWSTR    lpCmdLine,
    int       nCmdShow) {

#ifdef CRU_DEBUG
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );  
#endif

    Application application(hInstance);

    const auto window = Window::CreateOverlapped();
    /*
    window.native_message_event.AddHandler([](cru::ui::events::WindowNativeMessageEventArgs& args)
    {
        if (args.GetWindowMessage().msg == WM_PAINT)
        {
            OutputDebugStringW(L"Paint!\n");
            //args.SetResult(0);
        }
    });
    */
    /*
    // test1
    cru::ui::controls::TextBlock text_block;
    text_block.SetText(L"Hello world!");
    text_block.SetSize(cru::ui::Size(200, 30));
    window.AddChild(&text_block);

    std::array<D2D_COLOR_F, 4> colors =
    {
        D2D1::ColorF(D2D1::ColorF::Blue),
        D2D1::ColorF(D2D1::ColorF::Yellow),
        D2D1::ColorF(D2D1::ColorF::Green),
        D2D1::ColorF(D2D1::ColorF::Red)
    };

    std::random_device rd;     // only used once to initialise (seed) engine
    std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<decltype(colors.size())> uni(0, colors.size() - 1); // guaranteed unbiased


    window.draw_event.AddHandler([&](cru::ui::events::DrawEventArgs& args) {
        auto device_context = args.GetDeviceContext();

        ID2D1SolidColorBrush* brush;
        device_context->CreateSolidColorBrush(colors[uni(rng)], &brush);

        device_context->FillRectangle(D2D1::RectF(100.0f, 100.0f, 300.0f, 200.0f), brush);

        brush->Release();
    });

    cru::SetTimeout(2.0, [&window]() {
        window.InvalidateDraw();

        auto task = cru::SetInterval(0.5, [&window]() {
            window.InvalidateDraw();
        });

        cru::SetTimeout(4, [task]() {
            task->Cancel();
            task->Cancel(); // test for idempotency.
        });
    });
    */

    
    //test 2
    const auto layout = CreateWithLayout<LinearLayout>(LayoutSideParams::Exactly(500), LayoutSideParams::Content());

    layout->mouse_click_event.bubble.AddHandler([layout](cru::ui::events::MouseButtonEventArgs& args)
    {
        if (args.GetSender() == args.GetOriginalSender())
            layout->AddChild(TextBlock::Create(L"Layout is clicked!"));
    });

    {
        const auto inner_layout = CreateWithLayout<LinearLayout>(LayoutSideParams::Content(Alignment::End), LayoutSideParams::Content(), LinearLayout::Orientation::Horizontal);

        inner_layout->AddChild(TextBlock::Create(L"Toggle debug border"));

        const auto l = FrameLayout::Create();
        l->GetLayoutParams()->padding.SetLeftRight(20.0f);
        const auto toggle_button = ToggleButton::Create();
#ifdef CRU_DEBUG_LAYOUT
        toggle_button->toggle_event.AddHandler([&window](cru::ui::events::ToggleEventArgs& args)
        {
            window->SetDebugLayout(args.GetNewState());
        });
#endif
        l->AddChild(toggle_button);
        inner_layout->AddChild(l);
        layout->AddChild(inner_layout);
    }

    {
        const auto button = Button::Create();
        button->GetLayoutParams()->padding = Thickness(20, 5);
        button->SetChild(TextBlock::Create(L"Show popup window parenting this."));
        button->mouse_click_event.bubble.AddHandler([window, button](auto)
        {
            std::vector<cru::ui::controls::MenuItemInfo> items;
            items.emplace_back(L"Hello world!", []{});
            items.emplace_back(L"Item 2", []{});
            items.emplace_back(L"Close parent window.", [window]{ window->Close(); });

            cru::ui::controls::CreatePopupMenu(window->PointToScreen(button->GetPositionAbsolute()), items, window)->Show();
        });
        layout->AddChild(button);
    }

    {
        const auto button = Button::Create();
        button->GetLayoutParams()->padding = Thickness(20, 5);
        button->SetChild(TextBlock::Create(L"Show popup window parenting null."));
        button->SetBackgroundBrush(cru::graph::CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gold)));
        button->mouse_click_event.bubble.AddHandler([](auto)
        {
            auto popup = Window::CreatePopup(nullptr);
            popup->SetWindowRect(Rect(100, 100, 300, 300));
            popup->Show();
        });
        layout->AddChild(button);
    }
    
    {
        const auto button = Button::Create();
        button->GetLayoutParams()->padding = Thickness(20, 5);
        button->SetChild(TextBlock::Create(L"Show popup window with caption."));
        button->mouse_click_event.bubble.AddHandler([](auto)
        {
            auto popup = Window::CreatePopup(nullptr, true);
            popup->SetWindowRect(Rect(100, 100, 300, 300));
            popup->Show();
        });
        layout->AddChild(button);
    }

    {
        const auto text_block = CreateWithLayout<TextBlock>(LayoutSideParams::Exactly(200), LayoutSideParams::Exactly(80), L"Hello World!!!");

        text_block->mouse_click_event.bubble.AddHandler([layout](cru::ui::events::MouseButtonEventArgs& args)
        {
            layout->AddChild(TextBlock::Create(L"Hello world is clicked!"));
        });

        layout->AddChild(text_block);
    }

    {
        const auto text_box = TextBox::Create();
        text_box->GetLayoutParams()->width.min = 50.0f;
        text_box->GetLayoutParams()->width.max = 100.0f;
        text_box->char_event.tunnel.AddHandler([](cru::ui::events::CharEventArgs& args)
        {
            if (args.GetChar() == L'1')
                args.SetHandled();
        });
        layout->AddChild(text_box);
    }

    {
        const auto scroll_view = CreateWithLayout<ScrollControl>(LayoutSideParams::Stretch(), LayoutSideParams::Stretch());

        scroll_view->SetVerticalScrollBarVisibility(ScrollControl::ScrollBarVisibility::Always);

        const auto text_block = TextBlock::Create(
            L"Love myself I do. Not everything, but I love the good as well as the bad. I love my crazy lifestyle, and I love my hard discipline. I love my freedom of speech and the way my eyes get dark when I'm tired. I love that I have learned to trust people with my heart, even if it will get broken. I am proud of everything that I am and will become.");
        text_block->SetSelectable(true);

        scroll_view->SetChild(text_block);
        layout->AddChild(scroll_view);
    }

    layout->AddChild(CreateWithLayout<TextBlock>(LayoutSideParams::Content(Alignment::Start), LayoutSideParams::Content(), L"This is a little short sentence!!!"));
    layout->AddChild(CreateWithLayout<TextBlock>(LayoutSideParams::Content(Alignment::End), LayoutSideParams::Stretch(), L"By crupest!!!"));


    window->SetChild(layout);

    /*
    window.AddChild(
        CreateWithLayout<Border>(LayoutSideParams::Exactly(200), LayoutSideParams::Content(),
            std::initializer_list<cru::ui::Control*>{
                CreateWithLayout<TextBox>(LayoutSideParams::Stretch(), LayoutSideParams::Content())
            }
    ));
    */

    /* test 3
    const auto linear_layout = CreateWithLayout<LinearLayout>(Thickness(50, 50), Thickness(50, 50), LinearLayout::Orientation::Vertical, ControlList{
            Button::Create({
                TextBlock::Create(L"Button")
            }),
            CreateWithLayout<TextBox>(Thickness(30), Thickness(20))
        });

    linear_layout->SetBordered(true);

    window.AddChild(linear_layout);
    */

    window->Show();

    return application.Run();
}
