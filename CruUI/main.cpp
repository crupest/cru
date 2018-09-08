#include "application.h"
#include "ui/window.h"
#include "ui/controls/linear_layout.h"
#include "ui/controls/text_block.h"


using cru::Application;
using cru::ui::Window;
using cru::ui::controls::LinearLayout;
using cru::ui::controls::TextBlock;


int APIENTRY wWinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPWSTR    lpCmdLine,
    int       nCmdShow) {

    Application application(hInstance);
    Window window;

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
        window.Repaint();

        auto task = cru::SetInterval(0.5, [&window]() {
            window.Repaint();
        });

        cru::SetTimeout(4, [task]() {
            task->Cancel();
            task->Cancel(); // test for idempotency.
        });
    });
    */

    //test 2

    LinearLayout layout;
    TextBlock text_block_1;
    text_block_1.SetText(L"Hello World!!!");
    TextBlock text_block_2;
    text_block_2.SetText(L"This is a very very very very very long sentence!!!");
    TextBlock text_block_3;
    text_block_3.SetText(L"By crupest!!!");

    layout.AddChild(&text_block_1);
    layout.AddChild(&text_block_2);
    layout.AddChild(&text_block_3);

    window.AddChild(&layout);

    window.Show();

    return application.Run();
}
