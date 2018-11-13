#include "cru_ui.hpp"
//--------------------------------------------------------
//-------begin of file: src\any_map.cpp
//--------------------------------------------------------

namespace cru
{
    AnyMap::ListenerToken AnyMap::RegisterValueChangeListener(const String& key, const Listener& listener)
    {
        const auto token = current_listener_token_++;
        map_[key].second.push_back(token);
        listeners_.emplace(token, listener);
        return token;
    }

    void AnyMap::UnregisterValueChangeListener(const ListenerToken token)
    {
        const auto find_result = listeners_.find(token);
        if (find_result != listeners_.cend())
            listeners_.erase(find_result);
    }

    void AnyMap::InvokeListeners(std::list<ListenerToken>& listener_list, const std::any& value)
    {
        auto i = listener_list.cbegin();
        while (i != listener_list.cend())
        {
            auto current_i = i++;
            const auto find_result = listeners_.find(*current_i);
            if (find_result != listeners_.cend())
                find_result->second(value);
            else
                listener_list.erase(current_i); // otherwise remove the invalid listener token.
        }
    }
}
//--------------------------------------------------------
//-------end of file: src\any_map.cpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\application.cpp
//--------------------------------------------------------


namespace cru {
    constexpr auto god_window_class_name = L"GodWindowClass";
    constexpr int invoke_later_message_id = WM_USER + 2000;


    LRESULT CALLBACK GodWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        const auto app = Application::GetInstance();

        if (app)
        {
            const auto result = app->GetGodWindow()->HandleGodWindowMessage(hWnd, uMsg, wParam, lParam);
            if (result.has_value())
                return result.value();
            else
                return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
        else
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    GodWindow::GodWindow(Application* application)
    {
        const auto h_instance = application->GetInstanceHandle();

        god_window_class_ = std::make_unique<ui::WindowClass>(god_window_class_name, GodWndProc, h_instance);

        hwnd_ = CreateWindowEx(0,
            god_window_class_name,
            L"", 0,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            HWND_MESSAGE, nullptr, h_instance, nullptr
        );

        if (hwnd_ == nullptr)
            throw std::runtime_error("Failed to create window.");
    }

    GodWindow::~GodWindow()
    {
        ::DestroyWindow(hwnd_);
    }

    std::optional<LRESULT> GodWindow::HandleGodWindowMessage(HWND hwnd, int msg, WPARAM w_param, LPARAM l_param)
    {
        switch (msg)
        {
        case invoke_later_message_id:
        {
            const auto p_action = reinterpret_cast<std::function<void()>*>(w_param);
            (*p_action)();
            delete p_action;
            return 0;
        }
        case WM_TIMER:
        {
            const auto id = static_cast<UINT_PTR>(w_param);
            const auto action = TimerManager::GetInstance()->GetAction(id);
            if (action.has_value())
            {
                (action.value().second)();
                if (!action.value().first)
                    TimerManager::GetInstance()->KillTimer(id);
                return 0;
            }
            break;
        }
        default:
            return std::nullopt;
        }
        return std::nullopt;
    }



    Application* Application::instance_ = nullptr;

    Application * Application::GetInstance() {
        return instance_;
    }

    namespace
    {
        void LoadSystemCursor(HINSTANCE h_instance)
        {
            ui::cursors::arrow = std::make_shared<ui::Cursor>(::LoadCursorW(nullptr, IDC_ARROW), false);
            ui::cursors::hand = std::make_shared<ui::Cursor>(::LoadCursorW(nullptr, IDC_HAND), false);
            ui::cursors::i_beam = std::make_shared<ui::Cursor>(::LoadCursorW(nullptr, IDC_IBEAM), false);
        }
    }

    Application::Application(HINSTANCE h_instance)
        : h_instance_(h_instance) {

        if (instance_)
            throw std::runtime_error("A application instance already exists.");

        instance_ = this;

        god_window_ = std::make_unique<GodWindow>(this);

        LoadSystemCursor(h_instance);
    }

    Application::~Application()
    {
        for (auto i = singleton_list_.crbegin(); i != singleton_list_.crend(); ++i)
            delete *i;
        instance_ = nullptr;
    }

    int Application::Run()
    {
        MSG msg;

        while (GetMessage(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        return static_cast<int>(msg.wParam);
    }

    void Application::Quit(const int quit_code) {
        ::PostQuitMessage(quit_code);
    }

    void InvokeLater(const std::function<void()>& action) {
        //copy the action to a safe place
        auto p_action_copy = new std::function<void()>(action);

        if (PostMessageW(Application::GetInstance()->GetGodWindow()->GetHandle(), invoke_later_message_id, reinterpret_cast<WPARAM>(p_action_copy), 0) == 0)
            throw Win32Error(::GetLastError(), "InvokeLater failed to post message.");
    }
}
//--------------------------------------------------------
//-------end of file: src\application.cpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\base.cpp
//--------------------------------------------------------


namespace cru
{
    MultiByteString ToUtf8String(const StringView& string)
    {
        if (string.empty())
            return MultiByteString();

        const auto length = ::WideCharToMultiByte(CP_UTF8, 0, string.data(), -1, nullptr, 0, nullptr, nullptr);
        MultiByteString result;
        result.reserve(length);
        if (::WideCharToMultiByte(CP_UTF8, 0, string.data(), -1, result.data(), static_cast<int>(result.capacity()), nullptr, nullptr) == 0)
            throw Win32Error(::GetLastError(), "Failed to convert wide string to UTF-8.");
        return result;
    }
}
//--------------------------------------------------------
//-------end of file: src\base.cpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\cru_debug.cpp
//--------------------------------------------------------


namespace cru::debug
{
    void DebugMessage(const StringView& message)
    {
        ::OutputDebugStringW(message.data());
    }
}
//--------------------------------------------------------
//-------end of file: src\cru_debug.cpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\exception.cpp
//--------------------------------------------------------


namespace cru
{
    inline std::string HResultMakeMessage(HRESULT h_result, std::optional<MultiByteStringView> message)
    {
        char buffer[10];
        sprintf_s(buffer, "%#08x", h_result);

        if (message.has_value())
            return Format("An HResultError is thrown. HRESULT: {}.\nAdditional message: {}\n", buffer, message.value());
        else
            return Format("An HResultError is thrown. HRESULT: {}.\n", buffer);
    }

    HResultError::HResultError(HRESULT h_result, std::optional<MultiByteStringView> additional_message)
        : runtime_error(HResultMakeMessage(h_result, std::nullopt)), h_result_(h_result)
    {

    }

    inline std::string Win32MakeMessage(DWORD error_code, std::optional<MultiByteStringView> message)
    {
        char buffer[10];
        sprintf_s(buffer, "%#04x", error_code);

        if (message.has_value())
            return Format("Last error code: {}.\nAdditional message: {}\n", buffer, message.value());
        else
            return Format("Last error code: {}.\n", buffer);
    }

    Win32Error::Win32Error(DWORD error_code, std::optional<MultiByteStringView> additional_message)
        : runtime_error(Win32MakeMessage(error_code, std::nullopt)), error_code_(error_code)
    {

    }
}
//--------------------------------------------------------
//-------end of file: src\exception.cpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\main.cpp
//--------------------------------------------------------

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

int APIENTRY wWinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPWSTR    lpCmdLine,
    int       nCmdShow) {

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
    const auto layout = CreateWithLayout<LinearLayout>(LayoutSideParams::Exactly(500), LayoutSideParams::Content());

    layout->mouse_click_event.AddHandler([layout](cru::ui::events::MouseButtonEventArgs& args)
    {
        if (args.GetSender() == args.GetOriginalSender())
            layout->AddChild(TextBlock::Create(L"Layout is clicked!"));
    });

    {
        const auto inner_layout = CreateWithLayout<LinearLayout>(LayoutSideParams::Content(Alignment::End), LayoutSideParams::Content(), LinearLayout::Orientation::Horizontal);

        inner_layout->AddChild(TextBlock::Create(L"Toggle debug border"));

        const auto toggle_button = ToggleButton::Create();
#ifdef CRU_DEBUG_LAYOUT
        toggle_button->toggle_event.AddHandler([&window](cru::ui::events::ToggleEventArgs& args)
        {
            window->SetDebugLayout(args.GetNewState());
        });
#endif
        inner_layout->AddChild(toggle_button);
        layout->AddChild(inner_layout);
    }

    {
        const auto button = Button::Create();
        button->GetLayoutParams()->padding = Thickness(20, 5);
        button->AddChild(TextBlock::Create(L"Show popup window parenting this."));
        button->mouse_click_event.AddHandler([window, button](auto)
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
        button->AddChild(TextBlock::Create(L"Show popup window parenting null."));
        button->mouse_click_event.AddHandler([](auto)
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
        button->AddChild(TextBlock::Create(L"Show popup window with caption."));
        button->mouse_click_event.AddHandler([](auto)
        {
            auto popup = Window::CreatePopup(nullptr, true);
            popup->SetWindowRect(Rect(100, 100, 300, 300));
            popup->Show();
        });
        layout->AddChild(button);
    }

    {
        const auto text_block = CreateWithLayout<TextBlock>(LayoutSideParams::Exactly(200), LayoutSideParams::Exactly(80), L"Hello World!!!");

        text_block->mouse_click_event.AddHandler([layout](cru::ui::events::MouseButtonEventArgs& args)
        {
            layout->AddChild(TextBlock::Create(L"Hello world is clicked!"));
        });

        layout->AddChild(text_block);
    }

    {
        const auto text_box = TextBox::Create();
        text_box->GetLayoutParams()->width.min = 50.0f;
        text_box->GetLayoutParams()->width.max = 100.0f;
        layout->AddChild(text_box);
    }

    {
        const auto text_block = CreateWithLayout<TextBlock>(LayoutSideParams::Stretch(), LayoutSideParams::Stretch(), L"This is a very very very very very long sentence!!!");
        text_block->SetSelectable(true);
        layout->AddChild(text_block);
    }

    layout->AddChild(CreateWithLayout<TextBlock>(LayoutSideParams::Content(Alignment::Start), LayoutSideParams::Content(), L"This is a little short sentence!!!"));
    layout->AddChild(CreateWithLayout<TextBlock>(LayoutSideParams::Content(Alignment::End), LayoutSideParams::Stretch(), L"By crupest!!!"));


    window->AddChild(layout);

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
//--------------------------------------------------------
//-------end of file: src\main.cpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\timer.cpp
//--------------------------------------------------------


namespace cru
{
    TimerManager* TimerManager::GetInstance()
    {
        return Application::GetInstance()->ResolveSingleton<TimerManager>([](auto)
        {
            return new TimerManager{};
        });
    }

    UINT_PTR TimerManager::CreateTimer(const UINT milliseconds, const bool loop, const TimerAction& action)
    {
        const auto id = current_count_++;
        ::SetTimer(Application::GetInstance()->GetGodWindow()->GetHandle(), id, milliseconds, nullptr);
        map_.emplace(id, std::make_pair(loop, action));
        return id;
    }

    void TimerManager::KillTimer(const UINT_PTR id)
    {
        const auto find_result = map_.find(id);
        if (find_result != map_.cend())
        {
            ::KillTimer(Application::GetInstance()->GetGodWindow()->GetHandle(), id);
            map_.erase(find_result);
        }
    }

    std::optional<std::pair<bool, TimerAction>> TimerManager::GetAction(const UINT_PTR id)
    {
        const auto find_result = map_.find(id);
        if (find_result == map_.cend())
            return std::nullopt;
        return find_result->second;
    }

    TimerTask::TimerTask(const UINT_PTR id)
        : id_(id)
    {

    }

    void TimerTask::Cancel() const
    {
        TimerManager::GetInstance()->KillTimer(id_);
    }

    TimerTask SetTimeout(std::chrono::milliseconds milliseconds, const TimerAction& action)
    {
        const auto id = TimerManager::GetInstance()->CreateTimer(static_cast<UINT>(milliseconds.count()), false, action);
        return TimerTask(id);
    }

    TimerTask SetInterval(std::chrono::milliseconds milliseconds, const TimerAction& action)
    {
        const auto id = TimerManager::GetInstance()->CreateTimer(static_cast<UINT>(milliseconds.count()), true, action);
        return TimerTask(id);
    }
}
//--------------------------------------------------------
//-------end of file: src\timer.cpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\graph\graph.cpp
//--------------------------------------------------------


namespace cru::graph
{
    using Microsoft::WRL::ComPtr;

    WindowRenderTarget::WindowRenderTarget(GraphManager* graph_manager, HWND hwnd)
    {
        this->graph_manager_ = graph_manager;

        const auto d3d11_device = graph_manager->GetD3D11Device();
        const auto dxgi_factory = graph_manager->GetDxgiFactory();

        // Allocate a descriptor.
        DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = { 0 };
        swap_chain_desc.Width = 0;                           // use automatic sizing
        swap_chain_desc.Height = 0;
        swap_chain_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // this is the most common swapchain format
        swap_chain_desc.Stereo = false;
        swap_chain_desc.SampleDesc.Count = 1;                // don't use multi-sampling
        swap_chain_desc.SampleDesc.Quality = 0;
        swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swap_chain_desc.BufferCount = 2;                     // use double buffering to enable flip
        swap_chain_desc.Scaling = DXGI_SCALING_NONE;
        swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // all apps must use this SwapEffect
        swap_chain_desc.Flags = 0;



        // Get the final swap chain for this window from the DXGI factory.
        ThrowIfFailed(
            dxgi_factory->CreateSwapChainForHwnd(
                d3d11_device.Get(),
                hwnd,
                &swap_chain_desc,
                nullptr,
                nullptr,
                &dxgi_swap_chain_
            )
        );

        CreateTargetBitmap();
    }

    WindowRenderTarget::~WindowRenderTarget()
    {

    }

    void WindowRenderTarget::ResizeBuffer(const int width, const int height)
    {
        const auto graph_manager = graph_manager_;
        const auto d2d1_device_context = graph_manager->GetD2D1DeviceContext();

        ComPtr<ID2D1Image> old_target;
        d2d1_device_context->GetTarget(&old_target);
        const auto target_this = old_target == this->target_bitmap_;
        if (target_this)
            d2d1_device_context->SetTarget(nullptr);

        old_target = nullptr;
        target_bitmap_ = nullptr;

        ThrowIfFailed(
            dxgi_swap_chain_->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0)
        );

        CreateTargetBitmap();

        if (target_this)
            d2d1_device_context->SetTarget(target_bitmap_.Get());
    }

    void WindowRenderTarget::SetAsTarget()
    {
        GetD2DDeviceContext()->SetTarget(target_bitmap_.Get());
    }

    void WindowRenderTarget::Present()
    {
        ThrowIfFailed(
            dxgi_swap_chain_->Present(1, 0)
        );
    }

    void WindowRenderTarget::CreateTargetBitmap()
    {
        // Direct2D needs the dxgi version of the backbuffer surface pointer.
        ComPtr<IDXGISurface> dxgiBackBuffer;
        ThrowIfFailed(
            dxgi_swap_chain_->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer))
        );

        const auto dpi = graph_manager_->GetDpi();

        auto bitmap_properties =
            D2D1::BitmapProperties1(
                D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
                dpi.x,
                dpi.y
            );

        // Get a D2D surface from the DXGI back buffer to use as the D2D render target.
        ThrowIfFailed(
            graph_manager_->GetD2D1DeviceContext()->CreateBitmapFromDxgiSurface(
                dxgiBackBuffer.Get(),
                &bitmap_properties,
                &target_bitmap_
            )
        );
    }

    GraphManager* GraphManager::GetInstance()
    {
        return Application::GetInstance()->ResolveSingleton<GraphManager>([](auto)
        {
            return new GraphManager{};
        });
    }

    GraphManager::GraphManager()
    {
        UINT creation_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef CRU_DEBUG
            creation_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        const D3D_FEATURE_LEVEL feature_levels[] =
        {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1
        };


        ThrowIfFailed(D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            creation_flags,
            feature_levels,
            ARRAYSIZE(feature_levels),
            D3D11_SDK_VERSION,
            &d3d11_device_,
            nullptr,
            &d3d11_device_context_
        ));

        Microsoft::WRL::ComPtr<IDXGIDevice> dxgi_device;

        ThrowIfFailed(d3d11_device_.As(&dxgi_device));

        ThrowIfFailed(D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED,
            __uuidof(ID2D1Factory1),
            &d2d1_factory_
        ));

        ThrowIfFailed(d2d1_factory_->CreateDevice(dxgi_device.Get(), &d2d1_device_));

        ThrowIfFailed(d2d1_device_->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
            &d2d1_device_context_
        ));

        // Identify the physical adapter (GPU or card) this device is runs on.
        ComPtr<IDXGIAdapter> dxgi_adapter;
        ThrowIfFailed(
            dxgi_device->GetAdapter(&dxgi_adapter)
        );

        // Get the factory object that created the DXGI device.
        ThrowIfFailed(
            dxgi_adapter->GetParent(IID_PPV_ARGS(&dxgi_factory_))
        );


        ThrowIfFailed(DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(dwrite_factory_.GetAddressOf())
        ));

        dwrite_factory_->GetSystemFontCollection(&dwrite_system_font_collection_);
    }

    GraphManager::~GraphManager()
    {

    }

    std::shared_ptr<WindowRenderTarget> GraphManager::CreateWindowRenderTarget(HWND hwnd)
    {
        return std::make_shared<WindowRenderTarget>(this, hwnd);
    }

    Dpi GraphManager::GetDpi() const
    {
        Dpi dpi;
        d2d1_factory_->GetDesktopDpi(&dpi.x, &dpi.y);
        return dpi;
    }

    void GraphManager::ReloadSystemMetrics()
    {
        ThrowIfFailed(
            d2d1_factory_->ReloadSystemMetrics()
        );
    }
}
//--------------------------------------------------------
//-------end of file: src\graph\graph.cpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\border_property.cpp
//--------------------------------------------------------


namespace cru::ui
{
    BorderProperty::BorderProperty(): BorderProperty(UiManager::GetInstance()->GetPredefineResources()->border_property_brush)
    {

    }

    BorderProperty::BorderProperty(Microsoft::WRL::ComPtr<ID2D1Brush> brush): brush_(std::move(brush))
    {

    }

    BorderProperty::BorderProperty(Microsoft::WRL::ComPtr<ID2D1Brush> brush, const float width, const float radius_x,
        const float radius_y, Microsoft::WRL::ComPtr<ID2D1StrokeStyle> stroke_style) :
        brush_(std::move(brush)), stroke_width_(width), radius_x_(radius_x), radius_y_(radius_y), stroke_style_(std::move(stroke_style))
    {

    }
}
//--------------------------------------------------------
//-------end of file: src\ui\border_property.cpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\control.cpp
//--------------------------------------------------------

#include <algorithm>


#ifdef CRU_DEBUG_LAYOUT
#endif

namespace cru::ui
{
    using namespace events;

    Control::Control(const bool container) :
        is_container_(container)
    {

    }

    Control::Control(WindowConstructorTag, Window* window) : Control(true)
    {
        window_ = window;
    }

    Control::~Control()
    {
        for (auto control: GetChildren())
        {
            delete control;
        }
    }

    void AddChildCheck(Control* control)
    {
        if (control->GetParent() != nullptr)
            throw std::invalid_argument("The control already has a parent.");

        if (dynamic_cast<Window*>(control))
            throw std::invalid_argument("Can't add a window as child.");
    }

    const std::vector<Control*>& Control::GetChildren() const
    {
        return children_;
    }

    void Control::AddChild(Control* control)
    {
        ThrowIfNotContainer();
        AddChildCheck(control);

        this->children_.push_back(control);

        control->parent_ = this;

        this->OnAddChild(control);
    }

    void Control::AddChild(Control* control, int position)
    {
        ThrowIfNotContainer();
        AddChildCheck(control);

        if (position < 0 || static_cast<decltype(this->children_.size())>(position) > this->children_.size())
            throw std::invalid_argument("The position is out of range.");

        this->children_.insert(this->children_.cbegin() + position, control);

        control->parent_ = this;

        this->OnAddChild(this);
    }

    void Control::RemoveChild(Control* child)
    {
        ThrowIfNotContainer();
        const auto i = std::find(this->children_.cbegin(), this->children_.cend(), child);
        if (i == this->children_.cend())
            throw std::invalid_argument("The argument child is not a child of this control.");

        this->children_.erase(i);

        child->parent_ = nullptr;

        this->OnRemoveChild(this);
    }

    void Control::RemoveChild(const int position)
    {
        ThrowIfNotContainer();
        if (position < 0 || static_cast<decltype(this->children_.size())>(position) >= this->children_.size())
            throw std::invalid_argument("The position is out of range.");

        const auto p = children_.cbegin() + position;
        const auto child = *p;
        children_.erase(p);

        child->parent_ = nullptr;

        this->OnRemoveChild(child);
    }

    Control* Control::GetAncestor()
    {
        // if attached to window, the window is the ancestor.
        if (window_)
            return window_;

        // otherwise find the ancestor
        auto ancestor = this;
        while (const auto parent = ancestor->GetParent())
            ancestor = parent;
        return ancestor;
    }

    void TraverseDescendantsInternal(Control* control, const std::function<void(Control*)>& predicate)
    {
        predicate(control);
        if (control->IsContainer())
            for (auto c: control->GetChildren())
            {
                TraverseDescendantsInternal(c, predicate);
            }
    }

    void Control::TraverseDescendants(const std::function<void(Control*)>& predicate)
    {
        if (is_container_)
            TraverseDescendantsInternal(this, predicate);
        else
            predicate(this);
    }

    Point Control::GetPositionRelative()
    {
        return position_;
    }

    void Control::SetPositionRelative(const Point & position)
    {
        if (position != position_)
        {
            if (old_position_ == position) // if cache has been refreshed and no pending notify
                old_position_ = position_;
            position_ = position;
            LayoutManager::GetInstance()->InvalidateControlPositionCache(this);
            if (auto window = GetWindow())
            {
                window->Repaint();
            }
        }
    }

    Size Control::GetSize()
    {
        return size_;
    }

    void Control::SetSize(const Size & size)
    {
        const auto old_size = size_;
        size_ = size;
        SizeChangedEventArgs args(this, this, old_size, size);
        RaiseSizeChangedEvent(args);
        if (auto window = GetWindow())
            window->Repaint();
    }

    Point Control::GetPositionAbsolute() const
    {
        return position_cache_.lefttop_position_absolute;
    }

    Point Control::ControlToWindow(const Point& point) const
    {
        return Point(point.x + position_cache_.lefttop_position_absolute.x,
            point.y + position_cache_.lefttop_position_absolute.y);
    }

    Point Control::WindowToControl(const Point & point) const
    {
        return Point(point.x - position_cache_.lefttop_position_absolute.x,
            point.y - position_cache_.lefttop_position_absolute.y);
    }

    bool Control::IsPointInside(const Point & point)
    {
        if (border_geometry_ != nullptr)
        {
            if (IsBordered())
            {
                BOOL contains;
                border_geometry_->FillContainsPoint(Convert(point), D2D1::Matrix3x2F::Identity(), &contains);
                if (!contains)
                    border_geometry_->StrokeContainsPoint(Convert(point), GetBorderProperty().GetStrokeWidth(), nullptr, D2D1::Matrix3x2F::Identity(), &contains);
                return contains != 0;
            }
            else
            {
                BOOL contains;
                border_geometry_->FillContainsPoint(Convert(point), D2D1::Matrix3x2F::Identity(), &contains);
                return contains != 0;
            }
        }
        return false;
    }

    void Control::Draw(ID2D1DeviceContext* device_context)
    {
        D2D1::Matrix3x2F old_transform;
        device_context->GetTransform(&old_transform);

        const auto position = GetPositionRelative();
        device_context->SetTransform(old_transform * D2D1::Matrix3x2F::Translation(position.x, position.y));

        OnDrawCore(device_context);

        for (auto child : GetChildren())
            child->Draw(device_context);

        device_context->SetTransform(old_transform);
    }

    void Control::Repaint()
    {
        if (window_ != nullptr)
            window_->Repaint();
    }

    bool Control::RequestFocus()
    {
        auto window = GetWindow();
        if (window == nullptr)
            return false;

        return window->RequestFocusFor(this);
    }

    bool Control::HasFocus()
    {
        auto window = GetWindow();
        if (window == nullptr)
            return false;

        return window->GetFocusControl() == this;
    }

    void Control::InvalidateLayout()
    {
        if (const auto window = GetWindow())
            window->WindowInvalidateLayout();
    }

    void Control::Measure(const Size& available_size)
    {
        SetDesiredSize(OnMeasureCore(available_size));
    }

    void Control::Layout(const Rect& rect)
    {
        SetPositionRelative(rect.GetLeftTop());
        SetSize(rect.GetSize());
        OnLayoutCore(Rect(Point::Zero(), rect.GetSize()));
    }

    Size Control::GetDesiredSize() const
    {
        return desired_size_;
    }

    void Control::SetDesiredSize(const Size& desired_size)
    {
        desired_size_ = desired_size;
    }

    inline void Shrink(Rect& rect, const Thickness& thickness)
    {
        rect.left += thickness.left;
        rect.top += thickness.top;
        rect.width -= thickness.GetHorizontalTotal();
        rect.height -= thickness.GetVerticalTotal();
    }

    Rect Control::GetRect(const RectRange range)
    {
        if (GetSize() == Size::Zero())
            return Rect();

        const auto layout_params = GetLayoutParams();

        auto result = Rect(Point::Zero(), GetSize());

        if (range == RectRange::Margin)
            return result;

        Shrink(result, layout_params->margin);

        if (range == RectRange::FullBorder)
            return result;

        if (is_bordered_)
            Shrink(result, Thickness(GetBorderProperty().GetStrokeWidth() / 2.0f));

        if (range == RectRange::HalfBorder)
            return result;

        if (is_bordered_)
            Shrink(result, Thickness(GetBorderProperty().GetStrokeWidth() / 2.0f));

        if (range == RectRange::Padding)
            return result;

        Shrink(result, layout_params->padding);

        return result;
    }

    Point Control::TransformPoint(const Point& point, const RectRange from, const RectRange to)
    {
        const auto rect_from = GetRect(from);
        const auto rect_to = GetRect(to);
        auto p = point;
        p.x += rect_from.left;
        p.y += rect_from.top;
        p.x -= rect_to.left;
        p.y -= rect_to.top;
        return p;
    }

    void Control::InvalidateBorder()
    {
        RegenerateBorderGeometry();
        InvalidateLayout();
        Repaint();
    }

    void Control::SetBordered(const bool bordered)
    {
        if (bordered != is_bordered_)
        {
            is_bordered_ = bordered;
            InvalidateBorder();
        }
    }

    void Control::SetCursor(const Cursor::Ptr& cursor)
    {
        if (cursor != cursor_)
        {
            cursor_ = cursor;
            const auto window = GetWindow();
            if (window && window->GetMouseHoverControl() == this)
                window->UpdateCursor();
        }
    }

    void Control::OnAddChild(Control* child)
    {
        if (auto window = GetWindow())
        {
            child->TraverseDescendants([window](Control* control) {
                control->OnAttachToWindow(window);
            });
            window->RefreshControlList();
            InvalidateLayout();
        }
    }

    void Control::OnRemoveChild(Control* child)
    {
        if (auto window = GetWindow())
        {
            child->TraverseDescendants([window](Control* control) {
                control->OnDetachToWindow(window);
            });
            window->RefreshControlList();
            InvalidateLayout();
        }
    }

    void Control::OnAttachToWindow(Window* window)
    {
        window_ = window;
    }

    void Control::OnDetachToWindow(Window * window)
    {
        window_ = nullptr;
    }

    void Control::OnDrawCore(ID2D1DeviceContext* device_context)
    {
        #ifdef CRU_DEBUG_LAYOUT
        if (GetWindow()->IsDebugLayout())
        {
            if (padding_geometry_ != nullptr)
                device_context->FillGeometry(padding_geometry_.Get(), UiManager::GetInstance()->GetPredefineResources()->debug_layout_padding_brush.Get());
            if (margin_geometry_ != nullptr)
                device_context->FillGeometry(margin_geometry_.Get(), UiManager::GetInstance()->GetPredefineResources()->debug_layout_margin_brush.Get());
            device_context->DrawRectangle(Convert(GetRect(RectRange::Margin)), UiManager::GetInstance()->GetPredefineResources()->debug_layout_out_border_brush.Get());
        }
#endif

        if (is_bordered_)
        {
            const auto border_rect = GetRect(RectRange::HalfBorder);
            device_context->DrawRoundedRectangle(
                D2D1::RoundedRect(
                    Convert(border_rect),
                    GetBorderProperty().GetRadiusX(),
                    GetBorderProperty().GetRadiusY()
                ),
                GetBorderProperty().GetBrush().Get(),
                GetBorderProperty().GetStrokeWidth(),
                GetBorderProperty().GetStrokeStyle().Get()
            );
        }

        //draw background.
        const auto padding_rect = GetRect(RectRange::Padding);
        graph::WithTransform(device_context, D2D1::Matrix3x2F::Translation(padding_rect.left, padding_rect.top),
            [this](ID2D1DeviceContext* device_context)
            {
                OnDrawBackground(device_context);
                DrawEventArgs args(this, this, device_context);
                draw_background_event.Raise(args);
            });


        const auto rect = GetRect(RectRange::Content);
        graph::WithTransform(device_context, D2D1::Matrix3x2F::Translation(rect.left, rect.top),
            [this](ID2D1DeviceContext* device_context)
            {
                OnDrawContent(device_context);
                DrawEventArgs args(this, this, device_context);
                draw_content_event.Raise(args);
            });

        graph::WithTransform(device_context, D2D1::Matrix3x2F::Translation(padding_rect.left, padding_rect.top),
            [this](ID2D1DeviceContext* device_context)
            {
                OnDrawForeground(device_context);
                DrawEventArgs args(this, this, device_context);
                draw_foreground_event.Raise(args);
            });
    }

    void Control::OnDrawContent(ID2D1DeviceContext * device_context)
    {

    }

    void Control::OnDrawForeground(ID2D1DeviceContext* device_context)
    {

    }

    void Control::OnDrawBackground(ID2D1DeviceContext* device_context)
    {

    }

    void Control::OnPositionChanged(PositionChangedEventArgs & args)
    {

    }

    void Control::OnSizeChanged(SizeChangedEventArgs & args)
    {
    }

    void Control::OnPositionChangedCore(PositionChangedEventArgs & args)
    {

    }

    namespace
    {
#ifdef CRU_DEBUG_LAYOUT
        Microsoft::WRL::ComPtr<ID2D1Geometry> CalculateSquareRingGeometry(const Rect& out, const Rect& in)
        {
            const auto d2d1_factory = graph::GraphManager::GetInstance()->GetD2D1Factory();
            Microsoft::WRL::ComPtr<ID2D1RectangleGeometry> out_geometry;
            ThrowIfFailed(d2d1_factory->CreateRectangleGeometry(Convert(out), &out_geometry));
            Microsoft::WRL::ComPtr<ID2D1RectangleGeometry> in_geometry;
            ThrowIfFailed(d2d1_factory->CreateRectangleGeometry(Convert(in), &in_geometry));
            Microsoft::WRL::ComPtr<ID2D1PathGeometry> result_geometry;
            ThrowIfFailed(d2d1_factory->CreatePathGeometry(&result_geometry));
            Microsoft::WRL::ComPtr<ID2D1GeometrySink> sink;
            ThrowIfFailed(result_geometry->Open(&sink));
            ThrowIfFailed(out_geometry->CombineWithGeometry(in_geometry.Get(), D2D1_COMBINE_MODE_EXCLUDE, D2D1::Matrix3x2F::Identity(), sink.Get()));
            ThrowIfFailed(sink->Close());
            return result_geometry;
        }
#endif
    }

    void Control::OnSizeChangedCore(SizeChangedEventArgs & args)
    {
        RegenerateBorderGeometry();
#ifdef CRU_DEBUG_LAYOUT
        margin_geometry_ = CalculateSquareRingGeometry(GetRect(RectRange::Margin), GetRect(RectRange::FullBorder));
        padding_geometry_ = CalculateSquareRingGeometry(GetRect(RectRange::Padding), GetRect(RectRange::Content));
#endif
    }

    void Control::RaisePositionChangedEvent(PositionChangedEventArgs& args)
    {
        OnPositionChangedCore(args);
        OnPositionChanged(args);
        position_changed_event.Raise(args);
    }

    void Control::RaiseSizeChangedEvent(SizeChangedEventArgs& args)
    {
        OnSizeChangedCore(args);
        OnSizeChanged(args);
        size_changed_event.Raise(args);
    }

    void Control::RegenerateBorderGeometry()
    {
        if (IsBordered())
        {
            const auto bound_rect = GetRect(RectRange::HalfBorder);
            const auto bound_rounded_rect = D2D1::RoundedRect(Convert(bound_rect),
                GetBorderProperty().GetRadiusX(),
                GetBorderProperty().GetRadiusY());

            Microsoft::WRL::ComPtr<ID2D1RoundedRectangleGeometry> geometry;
            ThrowIfFailed(
                graph::GraphManager::GetInstance()->GetD2D1Factory()->CreateRoundedRectangleGeometry(bound_rounded_rect, &geometry)
            );
            border_geometry_ = std::move(geometry);
        }
        else
        {
            const auto bound_rect = GetRect(RectRange::Padding);
            Microsoft::WRL::ComPtr<ID2D1RectangleGeometry> geometry;
            ThrowIfFailed(
                graph::GraphManager::GetInstance()->GetD2D1Factory()->CreateRectangleGeometry(Convert(bound_rect), &geometry)
            );
            border_geometry_ = std::move(geometry);
        }
    }

    void Control::OnMouseEnter(MouseEventArgs & args)
    {
    }

    void Control::OnMouseLeave(MouseEventArgs & args)
    {
    }

    void Control::OnMouseMove(MouseEventArgs & args)
    {
    }

    void Control::OnMouseDown(MouseButtonEventArgs & args)
    {
    }

    void Control::OnMouseUp(MouseButtonEventArgs & args)
    {
    }

    void Control::OnMouseClick(MouseButtonEventArgs& args)
    {

    }

    void Control::OnMouseEnterCore(MouseEventArgs & args)
    {
        is_mouse_inside_ = true;
    }

    void Control::OnMouseLeaveCore(MouseEventArgs & args)
    {
        is_mouse_inside_ = false;
        for (auto& is_mouse_click_valid : is_mouse_click_valid_map_)
        {
            if (is_mouse_click_valid.second)
            {
                is_mouse_click_valid.second = false;
                OnMouseClickEnd(is_mouse_click_valid.first);
            }
        }
    }

    void Control::OnMouseMoveCore(MouseEventArgs & args)
    {

    }

    void Control::OnMouseDownCore(MouseButtonEventArgs & args)
    {
        if (is_focus_on_pressed_ && args.GetSender() == args.GetOriginalSender())
            RequestFocus();
        is_mouse_click_valid_map_[args.GetMouseButton()] = true;
        OnMouseClickBegin(args.GetMouseButton());
    }

    void Control::OnMouseUpCore(MouseButtonEventArgs & args)
    {
        if (is_mouse_click_valid_map_[args.GetMouseButton()])
        {
            is_mouse_click_valid_map_[args.GetMouseButton()] = false;
            RaiseMouseClickEvent(args);
            OnMouseClickEnd(args.GetMouseButton());
        }
    }

    void Control::OnMouseClickCore(MouseButtonEventArgs& args)
    {

    }

    void Control::RaiseMouseEnterEvent(MouseEventArgs& args)
    {
        OnMouseEnterCore(args);
        OnMouseEnter(args);
        mouse_enter_event.Raise(args);
    }

    void Control::RaiseMouseLeaveEvent(MouseEventArgs& args)
    {
        OnMouseLeaveCore(args);
        OnMouseLeave(args);
        mouse_leave_event.Raise(args);
    }

    void Control::RaiseMouseMoveEvent(MouseEventArgs& args)
    {
        OnMouseMoveCore(args);
        OnMouseMove(args);
        mouse_move_event.Raise(args);
    }

    void Control::RaiseMouseDownEvent(MouseButtonEventArgs& args)
    {
        OnMouseDownCore(args);
        OnMouseDown(args);
        mouse_down_event.Raise(args);
    }

    void Control::RaiseMouseUpEvent(MouseButtonEventArgs& args)
    {
        OnMouseUpCore(args);
        OnMouseUp(args);
        mouse_up_event.Raise(args);
    }

    void Control::RaiseMouseClickEvent(MouseButtonEventArgs& args)
    {
        OnMouseClickCore(args);
        OnMouseClick(args);
        mouse_click_event.Raise(args);
    }

    void Control::OnMouseClickBegin(MouseButton button)
    {

    }

    void Control::OnMouseClickEnd(MouseButton button)
    {

    }

    void Control::OnKeyDown(KeyEventArgs& args)
    {
    }

    void Control::OnKeyUp(KeyEventArgs& args)
    {
    }

    void Control::OnChar(CharEventArgs& args)
    {
    }

    void Control::OnKeyDownCore(KeyEventArgs& args)
    {
    }

    void Control::OnKeyUpCore(KeyEventArgs& args)
    {
    }

    void Control::OnCharCore(CharEventArgs& args)
    {
    }

    void Control::RaiseKeyDownEvent(KeyEventArgs& args)
    {
        OnKeyDownCore(args);
        OnKeyDown(args);
        key_down_event.Raise(args);
    }

    void Control::RaiseKeyUpEvent(KeyEventArgs& args)
    {
        OnKeyUpCore(args);
        OnKeyUp(args);
        key_up_event.Raise(args);
    }

    void Control::RaiseCharEvent(CharEventArgs& args)
    {
        OnCharCore(args);
        OnChar(args);
        char_event.Raise(args);
    }

    void Control::OnGetFocus(FocusChangeEventArgs& args)
    {

    }

    void Control::OnLoseFocus(FocusChangeEventArgs& args)
    {

    }

    void Control::OnGetFocusCore(FocusChangeEventArgs& args)
    {

    }

    void Control::OnLoseFocusCore(FocusChangeEventArgs& args)
    {

    }

    void Control::RaiseGetFocusEvent(FocusChangeEventArgs& args)
    {
        OnGetFocusCore(args);
        OnGetFocus(args);
        get_focus_event.Raise(args);
    }

    void Control::RaiseLoseFocusEvent(FocusChangeEventArgs& args)
    {
        OnLoseFocusCore(args);
        OnLoseFocus(args);
        lose_focus_event.Raise(args);
    }

    inline Size ThicknessToSize(const Thickness& thickness)
    {
        return Size(thickness.left + thickness.right, thickness.top + thickness.bottom);
    }

    inline float AtLeast0(const float value)
    {
        return value < 0 ? 0 : value;
    }

    Size Control::OnMeasureCore(const Size& available_size)
    {
        const auto layout_params = GetLayoutParams();

        if (!layout_params->Validate())
            throw std::runtime_error("LayoutParams is not valid. Please check it.");

        auto border_size = Size::Zero();
        if (is_bordered_)
        {
            const auto border_width = GetBorderProperty().GetStrokeWidth();
            border_size = Size(border_width * 2.0f, border_width * 2.0f);
        }

        // the total size of padding, border and margin
        const auto outer_size = ThicknessToSize(layout_params->padding) +
            ThicknessToSize(layout_params->margin) + border_size;


        auto&& get_content_measure_length = [](const LayoutSideParams& layout_length, const float available_length, const float outer_length) -> float
        {
            float length;
            if (layout_length.mode == MeasureMode::Exactly)
                length = layout_length.length;
            else if (available_length > outer_length)
                length = available_length - outer_length;
            else
                length = 0;
            return Coerce(length, layout_length.min, layout_length.max);
        };

        // if padding, margin and border exceeded, then content size is 0.
        const auto content_measure_size = Size(
            get_content_measure_length(layout_params->width, available_size.width, outer_size.width),
            get_content_measure_length(layout_params->height, available_size.height, outer_size.height)
        );

        const auto content_actual_size = OnMeasureContent(content_measure_size);

        auto stretch_width = false;
        auto stretch_width_determined = true;
        auto stretch_height = false;
        auto stretch_height_determined = true;

        // if it is stretch, init is stretch, and undetermined.
        if (layout_params->width.mode == MeasureMode::Stretch)
        {
            stretch_width = true;
            stretch_width_determined = false;
        }
        if (layout_params->height.mode == MeasureMode::Stretch)
        {
            stretch_height = true;
            stretch_width_determined = false;
        }

        if (!stretch_width_determined || !stretch_height_determined)
        {
            auto parent = GetParent();
            while (parent != nullptr)
            {
                auto lp = parent->GetLayoutParams();

                if (!stretch_width_determined)
                {
                    if (lp->width.mode == MeasureMode::Content) // if the first ancestor that is not stretch is content, then it can't stretch.
                    {
                        stretch_width = false;
                        stretch_width_determined = true;
                    }
                    if (lp->width.mode == MeasureMode::Exactly) // if the first ancestor that is not stretch is content, then it must be stretch.
                    {
                        stretch_width = true;
                        stretch_width_determined = true;
                    }
                }

                if (!stretch_height_determined) // the same as width
                {
                    if (lp->height.mode == MeasureMode::Content) // if the first ancestor that is not stretch is content, then it can't stretch.
                    {
                        stretch_height = false;
                        stretch_height_determined = true;
                    }
                    if (lp->height.mode == MeasureMode::Exactly) // if the first ancestor that is not stretch is content, then it must be stretch.
                    {
                        stretch_height = true;
                        stretch_height_determined = true;
                    }
                }

                if (stretch_width_determined && stretch_height_determined) // if both are determined.
                    break;

                parent = GetParent();
            }
        }
         
        auto&& calculate_final_length = [](const bool stretch, const std::optional<float> min_length, const float measure_length, const float actual_length) -> float
        {
            // only use measure length when stretch and actual length is smaller than measure length, that is "stretch"
            if (stretch && actual_length < measure_length)
                return measure_length;
            return Coerce(actual_length, min_length, std::nullopt);
        };

        const auto final_size = Size(
            calculate_final_length(stretch_width, layout_params->width.min, content_measure_size.width, content_actual_size.width),
            calculate_final_length(stretch_height, layout_params->height.min, content_measure_size.height, content_actual_size.height)
        ) + outer_size;

        return final_size;
    }

    void Control::OnLayoutCore(const Rect& rect)
    {
        const auto layout_params = GetLayoutParams();

        auto border_width = 0.0f;
        if (is_bordered_)
        {
            border_width = GetBorderProperty().GetStrokeWidth();
        }

        const Rect content_rect(
            rect.left + layout_params->padding.left + layout_params->margin.right + border_width,
            rect.top + layout_params->padding.top + layout_params->margin.top + border_width,
            rect.width - layout_params->padding.GetHorizontalTotal() - layout_params->margin.GetHorizontalTotal() - border_width * 2.0f,
            rect.height - layout_params->padding.GetVerticalTotal() - layout_params->margin.GetVerticalTotal() - border_width * 2.0f
        );

        if (content_rect.width < 0.0)
            throw std::runtime_error(Format("Width to layout must sufficient. But in {}, width for content is {}.", ToUtf8String(GetControlType()), content_rect.width));
        if (content_rect.height < 0.0)
            throw std::runtime_error(Format("Height to layout must sufficient. But in {}, height for content is {}.", ToUtf8String(GetControlType()), content_rect.height));

        OnLayoutContent(content_rect);
    }

    Size Control::OnMeasureContent(const Size& available_size)
    {
        auto max_child_size = Size::Zero();
        for (auto control: GetChildren())
        {
            control->Measure(available_size);
            const auto&& size = control->GetDesiredSize();
            if (max_child_size.width < size.width)
                max_child_size.width = size.width;
            if (max_child_size.height < size.height)
                max_child_size.height = size.height;
        }

        // coerce size fro stretch.
        for (auto control: GetChildren())
        {
            auto size = control->GetDesiredSize();
            const auto layout_params = control->GetLayoutParams();
            if (layout_params->width.mode == MeasureMode::Stretch)
                size.width = max_child_size.width;
            if (layout_params->height.mode == MeasureMode::Stretch)
                size.height = max_child_size.height;
            control->SetDesiredSize(size);
        }

        return max_child_size;
    }

    void Control::OnLayoutContent(const Rect& rect)
    {
        for (auto control: GetChildren())
        {
            const auto layout_params = control->GetLayoutParams();
            const auto size = control->GetDesiredSize();

            auto&& calculate_anchor = [](const float anchor, const Alignment alignment, const float layout_length, const float control_length) -> float
            {
                switch (alignment)
                {
                case Alignment::Center:
                    return anchor + (layout_length - control_length) / 2;
                case Alignment::Start:
                    return anchor;
                case Alignment::End:
                    return anchor + layout_length - control_length;
                default:
                    UnreachableCode();
                }
            };

            control->Layout(Rect(Point(
                calculate_anchor(rect.left, layout_params->width.alignment, rect.width, size.width),
                calculate_anchor(rect.top, layout_params->height.alignment, rect.height, size.height)
            ), size));
        }
    }

    void Control::CheckAndNotifyPositionChanged()
    {
        if (this->old_position_ != this->position_)
        {
            PositionChangedEventArgs args(this, this, this->old_position_, this->position_);
            this->RaisePositionChangedEvent(args);
            this->old_position_ = this->position_;
        }
    }

    std::list<Control*> GetAncestorList(Control* control)
    {
        std::list<Control*> l;
        while (control != nullptr)
        {
            l.push_front(control);
            control = control->GetParent();
        }
        return l;
    }

    Control* FindLowestCommonAncestor(Control * left, Control * right)
    {
        if (left == nullptr || right == nullptr)
            return nullptr;

        auto&& left_list = GetAncestorList(left);
        auto&& right_list = GetAncestorList(right);

        // the root is different
        if (left_list.front() != right_list.front())
            return nullptr;

        // find the last same control or the last control (one is ancestor of the other)
        auto left_i = left_list.cbegin();
        auto right_i = right_list.cbegin();
        while (true)
        {
            if (left_i == left_list.cend())
                return *(--left_i);
            if (right_i == right_list.cend())
                return *(--right_i);
            if (*left_i != *right_i)
                return *(--left_i);
            ++left_i;
            ++right_i;
        }
    }

    Control * IsAncestorOrDescendant(Control * left, Control * right)
    {
        //Search up along the trunk from "left". Return if find "right".
        auto control = left;
        while (control != nullptr)
        {
            if (control == right)
                return control;
            control = control->GetParent();
        }
        //Search up along the trunk from "right". Return if find "left".
        control = right;
        while (control != nullptr)
        {
            if (control == left)
                return control;
            control = control->GetParent();
        }
        return nullptr;
    }
}
//--------------------------------------------------------
//-------end of file: src\ui\control.cpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\cursor.cpp
//--------------------------------------------------------


namespace cru::ui
{
    Cursor::Cursor(HCURSOR handle, const bool auto_release)
        : handle_(handle), auto_release_(auto_release)
    {

    }

    Cursor::~Cursor()
    {
        if (auto_release_)
            ::DestroyCursor(handle_);
    }

    namespace cursors
    {
        Cursor::Ptr arrow{};
        Cursor::Ptr hand{};
        Cursor::Ptr i_beam{};
    }
}
//--------------------------------------------------------
//-------end of file: src\ui\cursor.cpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\layout_base.cpp
//--------------------------------------------------------


namespace cru::ui
{
    LayoutManager* LayoutManager::GetInstance()
    {
        return Application::GetInstance()->ResolveSingleton<LayoutManager>([](auto)
        {
            return new LayoutManager{};
        });
    }

    void LayoutManager::InvalidateControlPositionCache(Control * control)
    {
        if (cache_invalid_controls_.count(control) == 1)
            return;

        // find descendant then erase it; find ancestor then just return.
        auto i = cache_invalid_controls_.cbegin();
        while (i != cache_invalid_controls_.cend())
        {
            auto current_i = i++;
            const auto result = IsAncestorOrDescendant(*current_i, control);
            if (result == control)
                cache_invalid_controls_.erase(current_i);
            else if (result != nullptr)
                return; // find a ancestor of "control", just return
        }

        cache_invalid_controls_.insert(control);

        if (cache_invalid_controls_.size() == 1) // when insert just now and not repeat to "InvokeLater".
        {
            InvokeLater([this] {
                for (const auto i : cache_invalid_controls_)
                    RefreshControlPositionCache(i);
                for (const auto i : cache_invalid_controls_) // traverse all descendants of position-invalid controls and notify position change event
                    i->TraverseDescendants([](Control* control)
                {
                    control->CheckAndNotifyPositionChanged();
                });
                cache_invalid_controls_.clear(); // after update and notify, clear the set.

            });
        }
    }

    void LayoutManager::RefreshInvalidControlPositionCache()
    {
        for (const auto i : cache_invalid_controls_)
            RefreshControlPositionCache(i);
        cache_invalid_controls_.clear();
    }

    void LayoutManager::RefreshControlPositionCache(Control * control)
    {
        auto point = Point::Zero();
        auto parent = control;
        while ((parent = parent->GetParent())) {
            const auto p = parent->GetPositionRelative();
            point.x += p.x;
            point.y += p.y;
        }
        RefreshControlPositionCacheInternal(control, point);
    }

    void LayoutManager::RefreshControlPositionCacheInternal(Control * control, const Point & parent_lefttop_absolute)
    {
        const auto position = control->GetPositionRelative();
        const Point lefttop(
            parent_lefttop_absolute.x + position.x,
            parent_lefttop_absolute.y + position.y
        );
        control->position_cache_.lefttop_position_absolute = lefttop;
        for(auto c : control->GetChildren())
        {
            RefreshControlPositionCacheInternal(c, lefttop);
        }
    }
}
//--------------------------------------------------------
//-------end of file: src\ui\layout_base.cpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\ui_base.cpp
//--------------------------------------------------------


namespace cru::ui
{
    bool IsKeyDown(const int virtual_code)
    {
        const auto result = ::GetKeyState(virtual_code);
        return (static_cast<unsigned short>(result) & 0x8000) != 0;
    }

    bool IsKeyToggled(const int virtual_code)
    {
        const auto result = ::GetKeyState(virtual_code);
        return (static_cast<unsigned short>(result) & 1) != 0;
    }

    bool IsAnyMouseButtonDown()
    {
        return IsKeyDown(VK_LBUTTON) || IsKeyDown(VK_RBUTTON) || IsKeyDown(VK_MBUTTON);
    }
}
//--------------------------------------------------------
//-------end of file: src\ui\ui_base.cpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\ui_manager.cpp
//--------------------------------------------------------



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
//--------------------------------------------------------
//-------end of file: src\ui\ui_manager.cpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\window.cpp
//--------------------------------------------------------


namespace cru::ui
{
    WindowClass::WindowClass(const String& name, WNDPROC window_proc, HINSTANCE h_instance)
        : name_(name)
    {
        WNDCLASSEX window_class;
        window_class.cbSize = sizeof(WNDCLASSEX);

        window_class.style = CS_HREDRAW | CS_VREDRAW;
        window_class.lpfnWndProc = window_proc;
        window_class.cbClsExtra = 0;
        window_class.cbWndExtra = 0;
        window_class.hInstance = h_instance;
        window_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
        window_class.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
        window_class.lpszMenuName = NULL;
        window_class.lpszClassName = name.c_str();
        window_class.hIconSm = NULL;

        atom_ = RegisterClassEx(&window_class);
        if (atom_ == 0)
            throw std::runtime_error("Failed to create window class.");
    }

    LRESULT __stdcall GeneralWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
        auto window = WindowManager::GetInstance()->FromHandle(hWnd);

        LRESULT result;
        if (window != nullptr && window->HandleWindowMessage(hWnd, Msg, wParam, lParam, result))
            return result;

        return DefWindowProc(hWnd, Msg, wParam, lParam);
    }

    WindowManager* WindowManager::GetInstance()
    {
        return Application::GetInstance()->ResolveSingleton<WindowManager>([](auto)
        {
            return new WindowManager{};
        });
    }

    WindowManager::WindowManager() {
        general_window_class_ = std::make_unique<WindowClass>(
            L"CruUIWindowClass",
            GeneralWndProc,
            Application::GetInstance()->GetInstanceHandle()
            );
    }

    void WindowManager::RegisterWindow(HWND hwnd, Window * window) {
        const auto find_result = window_map_.find(hwnd);
        if (find_result != window_map_.end())
            throw std::runtime_error("The hwnd is already in the map.");

        window_map_.emplace(hwnd, window);
    }

    void WindowManager::UnregisterWindow(HWND hwnd) {
        const auto find_result = window_map_.find(hwnd);
        if (find_result == window_map_.end())
            throw std::runtime_error("The hwnd is not in the map.");
        window_map_.erase(find_result);

        if (window_map_.empty())
            Application::GetInstance()->Quit(0);
    }

    Window* WindowManager::FromHandle(HWND hwnd) {
        const auto find_result = window_map_.find(hwnd);
        if (find_result == window_map_.end())
            return nullptr;
        else
            return find_result->second;
    }

    std::vector<Window*> WindowManager::GetAllWindows() const
    {
        std::vector<Window*> windows;
        for (auto [key, value] : window_map_)
            windows.push_back(value);
        return windows;
    }

    inline Point PiToDip(const POINT& pi_point)
    {
        return Point(
            graph::PixelToDipX(pi_point.x),
            graph::PixelToDipY(pi_point.y)
        );
    }

    inline POINT DipToPi(const Point& dip_point)
    {
        POINT result;
        result.x = graph::DipToPixelX(dip_point.x);
        result.y = graph::DipToPixelY(dip_point.y);
        return result;
    }


    namespace
    {
        Cursor::Ptr GetCursorInherit(Control* control)
        {
            while (control != nullptr)
            {
                const auto cursor = control->GetCursor();
                if (cursor != nullptr)
                    return cursor;
                control = control->GetParent();
            }
            return cursors::arrow;
        }
    }

    Window* Window::CreateOverlapped()
    {
        return new Window(tag_overlapped_constructor{});
    }

    Window* Window::CreatePopup(Window* parent, const bool caption)
    {
        return new Window(tag_popup_constructor{}, parent, caption);
    }

    Window::Window(tag_overlapped_constructor) : Control(WindowConstructorTag{}, this), control_list_({ this }) {
        const auto window_manager = WindowManager::GetInstance();

        hwnd_ = CreateWindowEx(0,
            window_manager->GetGeneralWindowClass()->GetName(),
            L"", WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            nullptr, nullptr, Application::GetInstance()->GetInstanceHandle(), nullptr
        );

        if (hwnd_ == nullptr)
            throw std::runtime_error("Failed to create window.");

        AfterCreateHwnd(window_manager);
    }

    Window::Window(tag_popup_constructor, Window* parent, const bool caption) : Control(WindowConstructorTag{}, this), control_list_({ this })
    {
        if (parent != nullptr && !parent->IsWindowValid())
            throw std::runtime_error("Parent window is not valid.");

        parent_window_ = parent;

        const auto window_manager = WindowManager::GetInstance();

        hwnd_ = CreateWindowEx(0,
            window_manager->GetGeneralWindowClass()->GetName(),
            L"", caption ? (WS_POPUPWINDOW | WS_CAPTION) : WS_POPUP,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            parent == nullptr ? nullptr : parent->GetWindowHandle(),
            nullptr, Application::GetInstance()->GetInstanceHandle(), nullptr
        );

        if (hwnd_ == nullptr)
            throw std::runtime_error("Failed to create window.");

        AfterCreateHwnd(window_manager);
    }

    void Window::AfterCreateHwnd(WindowManager* window_manager)
    {
        window_manager->RegisterWindow(hwnd_, this);

        render_target_ = graph::GraphManager::GetInstance()->CreateWindowRenderTarget(hwnd_);

        SetCursor(cursors::arrow);
    }

    Window::~Window() {
        if (IsWindowValid())
        {
            SetDeleteThisOnDestroy(false); // avoid double delete.
            Close();
        }
        TraverseDescendants([this](Control* control) {
            control->OnDetachToWindow(this);
        });
    }

    StringView Window::GetControlType() const
    {
        return control_type;
    }

    void Window::SetDeleteThisOnDestroy(bool value)
    {
        delete_this_on_destroy_ = value;
    }

    void Window::Close() {
        if (IsWindowValid())
            DestroyWindow(hwnd_);
    }

    void Window::Repaint() {
        if (IsWindowValid()) {
            InvalidateRect(hwnd_, nullptr, false);
        }
    }

    void Window::Show() {
        if (IsWindowValid()) {
            ShowWindow(hwnd_, SW_SHOWNORMAL);
        }
    }

    void Window::Hide() {
        if (IsWindowValid()) {
            ShowWindow(hwnd_, SW_HIDE);
        }
    }

    Size Window::GetClientSize() {
        if (!IsWindowValid())
            return Size();

        const auto pixel_rect = GetClientRectPixel();
        return Size(
            graph::PixelToDipX(pixel_rect.right),
            graph::PixelToDipY(pixel_rect.bottom)
        );
    }

    void Window::SetClientSize(const Size & size) {
        if (IsWindowValid()) {
            const auto window_style = static_cast<DWORD>(GetWindowLongPtr(hwnd_, GWL_STYLE));
            const auto window_ex_style = static_cast<DWORD>(GetWindowLongPtr(hwnd_, GWL_EXSTYLE));

            RECT rect;
            rect.left = 0;
            rect.top = 0;
            rect.right = graph::DipToPixelX(size.width);
            rect.bottom = graph::DipToPixelY(size.height);
            AdjustWindowRectEx(&rect, window_style, FALSE, window_ex_style);

            SetWindowPos(
                hwnd_, nullptr, 0, 0,
                rect.right - rect.left,
                rect.bottom - rect.top,
                SWP_NOZORDER | SWP_NOMOVE
            );
        }
    }

    Rect Window::GetWindowRect() {
        if (!IsWindowValid())
            return Rect();

        RECT rect;
        ::GetWindowRect(hwnd_, &rect);

        return Rect::FromVertices(
            graph::PixelToDipX(rect.left),
            graph::PixelToDipY(rect.top),
            graph::PixelToDipX(rect.right),
            graph::PixelToDipY(rect.bottom)
        );
    }

    void Window::SetWindowRect(const Rect & rect) {
        if (IsWindowValid()) {
            SetWindowPos(
                hwnd_, nullptr,
                graph::DipToPixelX(rect.left),
                graph::DipToPixelY(rect.top),
                graph::DipToPixelX(rect.GetRight()),
                graph::DipToPixelY(rect.GetBottom()),
                SWP_NOZORDER
            );
        }
    }

    void Window::SetWindowPosition(const Point& position)
    {
        if (IsWindowValid()) {
            SetWindowPos(
                hwnd_, nullptr,
                graph::DipToPixelX(position.x),
                graph::DipToPixelY(position.y),
                0, 0,
                SWP_NOZORDER | SWP_NOSIZE
            );
        }
    }

    Point Window::PointToScreen(const Point& point)
    {
        if (!IsWindowValid())
            return Point::Zero();

        auto p = DipToPi(point);
        if (::ClientToScreen(GetWindowHandle(), &p) == 0)
            throw Win32Error(::GetLastError(), "Failed transform point from window to screen.");
        return PiToDip(p);
    }

    Point Window::PointFromScreen(const Point& point)
    {
        if (!IsWindowValid())
            return Point::Zero();

        auto p = DipToPi(point);
        if (::ScreenToClient(GetWindowHandle(), &p) == 0)
            throw Win32Error(::GetLastError(), "Failed transform point from screen to window.");
        return PiToDip(p);
    }

    bool Window::HandleWindowMessage(HWND hwnd, int msg, WPARAM w_param, LPARAM l_param, LRESULT & result) {

        if (!native_message_event.IsNoHandler())
        {
            events::WindowNativeMessageEventArgs args(this, this, {hwnd, msg, w_param, l_param});
            native_message_event.Raise(args);
            if (args.GetResult().has_value())
            {
                result = args.GetResult().value();
                return true;
            }
        }

        switch (msg) {
        case WM_PAINT:
            OnPaintInternal();
            result = 0;
            return true;
        case WM_ERASEBKGND:
            result = 1;
            return true;
        case WM_SETFOCUS:
            OnSetFocusInternal();
            result = 0;
            return true;
        case WM_KILLFOCUS:
            OnKillFocusInternal();
            result = 0;
            return true;
        case WM_MOUSEMOVE:
        {
            POINT point;
            point.x = GET_X_LPARAM(l_param);
            point.y = GET_Y_LPARAM(l_param);
            OnMouseMoveInternal(point);
            result = 0;
            return true;
        }
        case WM_LBUTTONDOWN:
        {
            POINT point;
            point.x = GET_X_LPARAM(l_param);
            point.y = GET_Y_LPARAM(l_param);
            OnMouseDownInternal(MouseButton::Left, point);
            result = 0;
            return true;
        }
        case WM_LBUTTONUP:
        {
            POINT point;
            point.x = GET_X_LPARAM(l_param);
            point.y = GET_Y_LPARAM(l_param);
            OnMouseUpInternal(MouseButton::Left, point);
            result = 0;
            return true;
        }
        case WM_RBUTTONDOWN:
        {
            POINT point;
            point.x = GET_X_LPARAM(l_param);
            point.y = GET_Y_LPARAM(l_param);
            OnMouseDownInternal(MouseButton::Right, point);
            result = 0;
            return true;
        }
        case WM_RBUTTONUP:
        {
            POINT point;
            point.x = GET_X_LPARAM(l_param);
            point.y = GET_Y_LPARAM(l_param);
            OnMouseUpInternal(MouseButton::Right, point);
            result = 0;
            return true;
        }
        case WM_MBUTTONDOWN:
        {
            POINT point;
            point.x = GET_X_LPARAM(l_param);
            point.y = GET_Y_LPARAM(l_param);
            OnMouseDownInternal(MouseButton::Middle, point);
            result = 0;
            return true;
        }
        case WM_MBUTTONUP:
        {
            POINT point;
            point.x = GET_X_LPARAM(l_param);
            point.y = GET_Y_LPARAM(l_param);
            OnMouseUpInternal(MouseButton::Middle, point);
            result = 0;
            return true;
        }
        case WM_KEYDOWN:
            OnKeyDownInternal(static_cast<int>(w_param));
            result = 0;
            return true;
        case WM_KEYUP:
            OnKeyUpInternal(static_cast<int>(w_param));
            result = 0;
            return true;
        case WM_CHAR:
            OnCharInternal(static_cast<wchar_t>(w_param));
            result = 0;
            return true;
        case WM_SIZE:
            OnResizeInternal(LOWORD(l_param), HIWORD(l_param));
            result = 0;
            return true;
        case WM_ACTIVATE:
            if (w_param == WA_ACTIVE || w_param == WA_CLICKACTIVE)
                OnActivatedInternal();
            else if (w_param == WA_INACTIVE)
                OnDeactivatedInternal();
            result = 0;
            return true;
        case WM_DESTROY:
            OnDestroyInternal();
            result = 0;
            return true;
        default:
            return false;
        }
    }

    Point Window::GetMousePosition()
    {
        if (!IsWindowValid())
            return Point::Zero();
        POINT point;
        ::GetCursorPos(&point);
        ::ScreenToClient(hwnd_, &point);
        return PiToDip(point);
    }

    Point Window::GetPositionRelative()
    {
        return Point();
    }

    void Window::SetPositionRelative(const Point & position)
    {

    }

    Size Window::GetSize()
    {
        return GetClientSize();
    }

    void Window::SetSize(const Size & size)
    {

    }

    bool Window::IsPointInside(const Point& point)
    {
        return Rect(Point::Zero(), GetClientSize()).IsPointInside(point);
    }

    void Window::WindowInvalidateLayout()
    {
        if (is_layout_invalid_)
            return;

        is_layout_invalid_ = true;
        InvokeLater([this]
        {
            if (is_layout_invalid_)
                Relayout();
        });
    }

    void Window::Relayout()
    {
        Measure(GetSize());
        OnLayoutCore(Rect(Point::Zero(), GetSize()));
        is_layout_invalid_ = false;
    }

    void Window::SetSizeFitContent(const Size& max_size)
    {
        Measure(max_size);
        SetClientSize(GetDesiredSize());
        OnLayoutCore(Rect(Point::Zero(), GetSize()));
        is_layout_invalid_ = false;
    }

    void Window::RefreshControlList() {
        control_list_.clear();
        TraverseDescendants([this](Control* control) {
            this->control_list_.push_back(control);
        });
    }

    Control * Window::HitTest(const Point & point)
    {
        for (auto i = control_list_.crbegin(); i != control_list_.crend(); ++i) {
            auto control = *i;
            if (control->IsPointInside(control->WindowToControl(point))) {
                return control;
            }
        }
        return nullptr;
    }

    bool Window::RequestFocusFor(Control * control)
    {
        if (control == nullptr)
            throw std::invalid_argument("The control to request focus can't be null. You can set it as the window.");

        if (!IsWindowValid())
            return false;

        if (!window_focus_)
        {
            focus_control_ = control;
            ::SetFocus(hwnd_);
            return true; // event dispatch will be done in window message handling function "OnSetFocusInternal".
        }

        if (focus_control_ == control)
            return true;

        DispatchEvent(focus_control_, &Control::RaiseLoseFocusEvent, nullptr, false);

        focus_control_ = control;

        DispatchEvent(control, &Control::RaiseGetFocusEvent, nullptr, false);

        return true;
    }

    Control* Window::GetFocusControl()
    {
        return focus_control_;
    }

    Control* Window::CaptureMouseFor(Control* control)
    {
        if (control != nullptr)
        {
            ::SetCapture(hwnd_);
            std::swap(mouse_capture_control_, control);
            DispatchMouseHoverControlChangeEvent(control ? control : mouse_hover_control_, mouse_capture_control_, GetMousePosition());
            return control;
        }
        else
        {
            return ReleaseCurrentMouseCapture();
        }
    }

    Control* Window::ReleaseCurrentMouseCapture()
    {
        if (mouse_capture_control_)
        {
            const auto previous = mouse_capture_control_;
            mouse_capture_control_ = nullptr;
            ::ReleaseCapture();
            DispatchMouseHoverControlChangeEvent(previous, mouse_hover_control_, GetMousePosition());
            return previous;
        }
        else
        {
            return nullptr;
        }
    }

    void Window::UpdateCursor()
    {
        if (IsWindowValid() && mouse_hover_control_ != nullptr)
        {
            SetCursorInternal(GetCursorInherit(mouse_hover_control_)->GetHandle());
        }
    }

#ifdef CRU_DEBUG_LAYOUT
    void Window::SetDebugLayout(const bool value)
    {
        if (debug_layout_ != value)
        {
            debug_layout_ = value;
            Repaint();
        }
    }
#endif

    RECT Window::GetClientRectPixel() {
        RECT rect{ };
        GetClientRect(hwnd_, &rect);
        return rect;
    }

    bool Window::IsMessageInQueue(UINT message)
    {
        MSG msg;
        return ::PeekMessageW(&msg, hwnd_, message, message, PM_NOREMOVE) != 0;
    }

    void Window::SetCursorInternal(HCURSOR cursor)
    {
        if (IsWindowValid())
        {
            ::SetClassLongPtrW(GetWindowHandle(), GCLP_HCURSOR, reinterpret_cast<LONG_PTR>(cursor));
            if (mouse_hover_control_ != nullptr)
                ::SetCursor(cursor);
        }
    }

    void Window::OnDestroyInternal() {
        WindowManager::GetInstance()->UnregisterWindow(hwnd_);
        hwnd_ = nullptr;
        if (delete_this_on_destroy_)
            InvokeLater([this]{ delete this; });
    }

    void Window::OnPaintInternal() {
        render_target_->SetAsTarget();

        auto device_context = render_target_->GetD2DDeviceContext();

        device_context->BeginDraw();

        //Clear the background.
        device_context->Clear(D2D1::ColorF(D2D1::ColorF::White));

        Draw(device_context.Get());

        ThrowIfFailed(
            device_context->EndDraw(), "Failed to draw window."
        );

        render_target_->Present();

        ValidateRect(hwnd_, nullptr);
    }

    void Window::OnResizeInternal(const int new_width, const int new_height) {
        render_target_->ResizeBuffer(new_width, new_height);
        if (!(new_width == 0 && new_height == 0))
            WindowInvalidateLayout();
    }

    void Window::OnSetFocusInternal()
    {
        window_focus_ = true;
        DispatchEvent(focus_control_, &Control::RaiseGetFocusEvent, nullptr, true);
    }

    void Window::OnKillFocusInternal()
    {
        window_focus_ = false;
        DispatchEvent(focus_control_, &Control::RaiseLoseFocusEvent, nullptr, true);
    }

    void Window::OnMouseMoveInternal(const POINT point)
    {
        const auto dip_point = PiToDip(point);

        //when mouse was previous outside the window
        if (mouse_hover_control_ == nullptr) {
            //invoke TrackMouseEvent to have WM_MOUSELEAVE sent.
            TRACKMOUSEEVENT tme;
            tme.cbSize = sizeof tme;
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd_;

            TrackMouseEvent(&tme);
        }

        //Find the first control that hit test succeed.
        const auto new_control_mouse_hover = HitTest(dip_point);
        const auto old_control_mouse_hover = mouse_hover_control_;
        mouse_hover_control_ = new_control_mouse_hover;

        if (mouse_capture_control_) // if mouse is captured
        {
            DispatchEvent(mouse_capture_control_, &Control::RaiseMouseMoveEvent, nullptr, dip_point);
        }
        else
        {
            DispatchMouseHoverControlChangeEvent(old_control_mouse_hover, new_control_mouse_hover, dip_point);
            DispatchEvent(new_control_mouse_hover, &Control::RaiseMouseMoveEvent, nullptr, dip_point);
        }
    }

    void Window::OnMouseLeaveInternal()
    {
        DispatchEvent(mouse_hover_control_, &Control::RaiseMouseLeaveEvent, nullptr);
        mouse_hover_control_ = nullptr;
    }

    void Window::OnMouseDownInternal(MouseButton button, POINT point)
    {
        const auto dip_point = PiToDip(point);

        Control* control;

        if (mouse_capture_control_)
            control = mouse_capture_control_;
        else
            control = HitTest(dip_point);

        DispatchEvent(control, &Control::RaiseMouseDownEvent, nullptr, dip_point, button);
    }

    void Window::OnMouseUpInternal(MouseButton button, POINT point)
    {
        const auto dip_point = PiToDip(point);

        Control* control;

        if (mouse_capture_control_)
            control = mouse_capture_control_;
        else
            control = HitTest(dip_point);

        DispatchEvent(control, &Control::RaiseMouseUpEvent, nullptr, dip_point, button);
    }

    void Window::OnKeyDownInternal(int virtual_code)
    {
        DispatchEvent(focus_control_, &Control::RaiseKeyDownEvent, nullptr, virtual_code);
    }

    void Window::OnKeyUpInternal(int virtual_code)
    {
        DispatchEvent(focus_control_, &Control::RaiseKeyUpEvent, nullptr, virtual_code);
    }

    void Window::OnCharInternal(wchar_t c)
    {
        DispatchEvent(focus_control_, &Control::RaiseCharEvent, nullptr, c);
    }

    void Window::OnActivatedInternal()
    {
        events::UiEventArgs args(this, this);
        activated_event.Raise(args);
    }

    void Window::OnDeactivatedInternal()
    {
        events::UiEventArgs args(this, this);
        deactivated_event.Raise(args);
    }

    void Window::DispatchMouseHoverControlChangeEvent(Control* old_control, Control* new_control, const Point& point)
    {
        if (new_control != old_control) //if the mouse-hover-on control changed
        {
            const auto lowest_common_ancestor = FindLowestCommonAncestor(old_control, new_control);
            if (old_control != nullptr) // if last mouse-hover-on control exists
                DispatchEvent(old_control, &Control::RaiseMouseLeaveEvent, lowest_common_ancestor); // dispatch mouse leave event.
            if (new_control != nullptr)
            {
                DispatchEvent(new_control, &Control::RaiseMouseEnterEvent, lowest_common_ancestor, point); // dispatch mouse enter event.
                UpdateCursor();
            }
        }
    }
}
//--------------------------------------------------------
//-------end of file: src\ui\window.cpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\animations\animation.cpp
//--------------------------------------------------------

#include <utility>


namespace cru::ui::animations
{
    namespace details
    {
        class AnimationDelegateImpl;
        constexpr double frame_rate = 60;
        constexpr AnimationTimeUnit frame_step_time = AnimationTimeUnit(1) / frame_rate;

        
        class AnimationDelegateImpl : public virtual IAnimationDelegate
        {
        public:
            explicit AnimationDelegateImpl(String tag)
                : tag_(std::move(tag))
            {

            }
            AnimationDelegateImpl(const AnimationDelegateImpl& other) = delete;
            AnimationDelegateImpl(AnimationDelegateImpl&& other) = delete;
            AnimationDelegateImpl& operator=(const AnimationDelegateImpl& other) = delete;
            AnimationDelegateImpl& operator=(AnimationDelegateImpl&& other) = delete;
            ~AnimationDelegateImpl() override = default;

            void Cancel() override
            {
                AnimationManager::GetInstance()->RemoveAnimation(tag_);
            }

        private:
            String tag_;
        };


        class Animation : public Object
        {
        public:
            Animation(AnimationInfo info, AnimationDelegatePtr delegate)
                : info_(std::move(info)), delegate_(std::move(delegate))
            {
                
            }

            Animation(const Animation& other) = delete;
            Animation(Animation&& other) = delete;
            Animation& operator=(const Animation& other) = delete;
            Animation& operator=(Animation&& other) = delete;
            ~Animation() override;


            // If finish or invalid, return false.
            bool Step(AnimationTimeUnit time);

            String GetTag() const
            {
                return info_.tag;
            }

        private:
            const AnimationInfo info_;
            const AnimationDelegatePtr delegate_;

            AnimationTimeUnit current_time_ = AnimationTimeUnit::zero();
        };

        AnimationManager* AnimationManager::GetInstance()
        {
            return Application::GetInstance()->ResolveSingleton<AnimationManager>([](auto)
            {
                return new AnimationManager{};
            });
        }

        AnimationManager::AnimationManager()
        {

        }

        AnimationManager::~AnimationManager()
        {
            KillTimer();
        }

        AnimationDelegatePtr AnimationManager::CreateAnimation(AnimationInfo info)
        {
            if (animations_.empty())
                SetTimer();

            const auto tag = info.tag;
            auto delegate = std::make_shared<AnimationDelegateImpl>(tag);
            animations_[tag] = std::make_unique<Animation>(std::move(info), delegate);

            return delegate;
        }

        void AnimationManager::RemoveAnimation(const String& tag)
        {
            const auto find_result = animations_.find(tag);
            if (find_result != animations_.cend())
                animations_.erase(find_result);

            if (animations_.empty())
                KillTimer();
        }

        void AnimationManager::SetTimer()
        {
            if (!timer_.has_value())
                timer_ = SetInterval(std::chrono::duration_cast<std::chrono::milliseconds>(frame_step_time), [this]()
                {
                    auto i = animations_.cbegin();
                    while (i != animations_.cend())
                    {
                        auto current_i = i++;
                        if (current_i->second->Step(frame_step_time))
                            animations_.erase(current_i);
                    }

                    if (animations_.empty())
                        KillTimer();
                });
        }

        void AnimationManager::KillTimer()
        {
            if (timer_.has_value())
            {
                timer_.value().Cancel();
                timer_ = std::nullopt;
            }
        }

        Animation::~Animation()
        {
            if (current_time_ < info_.duration)
                for (const auto& handler : info_.cancel_handlers)
                    handler();
        }

        bool Animation::Step(const AnimationTimeUnit time)
        {
            current_time_ += time;
            if (current_time_ > info_.duration)
            {
                for (const auto& handler : info_.step_handlers)
                    handler(delegate_, 1);
                for (const auto& handler : info_.finish_handlers)
                    handler();
                return true;
            }
            else
            {
                for (const auto& handler : info_.step_handlers)
                    handler(delegate_, current_time_ / info_.duration);
                return false;
            }
        }

    }

    AnimationDelegatePtr AnimationBuilder::Start()
    {
        CheckValid();
        valid_ = false;
        return details::AnimationManager::GetInstance()->CreateAnimation(std::move(info_));
    }
}
//--------------------------------------------------------
//-------end of file: src\ui\animations\animation.cpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\controls\button.cpp
//--------------------------------------------------------


namespace cru::ui::controls
{
    Button::Button() : Control(true),
        normal_border_{UiManager::GetInstance()->GetPredefineResources()->button_normal_border},
        pressed_border_{UiManager::GetInstance()->GetPredefineResources()->button_press_border}
    {
        SetBordered(true);
        GetBorderProperty() = normal_border_;

        SetCursor(cursors::hand);
    }

    StringView Button::GetControlType() const
    {
        return control_type;
    }

    void Button::OnMouseClickBegin(MouseButton button)
    {
        GetBorderProperty() = pressed_border_;
        InvalidateBorder();
    }

    void Button::OnMouseClickEnd(MouseButton button)
    {
        GetBorderProperty() = normal_border_;
        InvalidateBorder();
    }
}
//--------------------------------------------------------
//-------end of file: src\ui\controls\button.cpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\controls\linear_layout.cpp
//--------------------------------------------------------

#include <algorithm>

namespace cru::ui::controls
{
    LinearLayout::LinearLayout(const Orientation orientation)
        : Control(true), orientation_(orientation)
    {

    }

    inline float AtLeast0(const float value)
    {
        return value < 0 ? 0 : value;
    }

    inline Size AtLeast0(const Size& size)
    {
        return Size(AtLeast0(size.width), AtLeast0(size.height));
    }

    StringView LinearLayout::GetControlType() const
    {
        return control_type;
    }

    Size LinearLayout::OnMeasureContent(const Size& available_size)
    {
        auto actual_size_for_children = Size::Zero();

        float secondary_side_child_max_length = 0;

        std::list<Control*> stretch_control_list;

        // First measure Content and Exactly and count Stretch.
        if (orientation_ == Orientation::Horizontal)
            for(auto control: GetChildren())
            {
                const auto mode = control->GetLayoutParams()->width.mode;
                if (mode == MeasureMode::Content || mode == MeasureMode::Exactly)
                {
                    Size current_available_size(AtLeast0(available_size.width - actual_size_for_children.width), available_size.height);
                    control->Measure(current_available_size);
                    const auto size = control->GetDesiredSize();
                    actual_size_for_children.width += size.width;
                    secondary_side_child_max_length = std::max(size.height, secondary_side_child_max_length);
                }
                else
                    stretch_control_list.push_back(control);
            }
        else
            for(auto control: GetChildren())
            {
                const auto mode = control->GetLayoutParams()->height.mode;
                if (mode == MeasureMode::Content || mode == MeasureMode::Exactly)
                {
                    Size current_available_size(available_size.width, AtLeast0(available_size.height - actual_size_for_children.height));
                    control->Measure(current_available_size);
                    const auto size = control->GetDesiredSize();
                    actual_size_for_children.height += size.height;
                    secondary_side_child_max_length = std::max(size.width, secondary_side_child_max_length);
                }
                else
                    stretch_control_list.push_back(control);
            }

        if (orientation_ == Orientation::Horizontal)
        {
            const auto available_width = AtLeast0(available_size.width - actual_size_for_children.width) / stretch_control_list.size();
            for (const auto control : stretch_control_list)
            {
                control->Measure(Size(available_width, available_size.height));
                const auto size = control->GetDesiredSize();
                actual_size_for_children.width += size.width;
                secondary_side_child_max_length = std::max(size.height, secondary_side_child_max_length);
            }
        }
        else
        {
            const auto available_height = AtLeast0(available_size.height - actual_size_for_children.height) / stretch_control_list.size();
            for (const auto control : stretch_control_list)
            {
                control->Measure(Size(available_size.width, available_height));
                const auto size = control->GetDesiredSize();
                actual_size_for_children.height += size.height;
                secondary_side_child_max_length = std::max(size.width, secondary_side_child_max_length);
            }
        }

        if (orientation_ == Orientation::Horizontal)
        {
            for (auto control : GetChildren())
            {
                if (control->GetLayoutParams()->height.mode == MeasureMode::Stretch)
                {
                    control->SetDesiredSize(Size(control->GetDesiredSize().width, secondary_side_child_max_length));
                }
            }
            actual_size_for_children.height = secondary_side_child_max_length;
        }
        else
        {
            for (auto control : GetChildren())
            {
                if (control->GetLayoutParams()->width.mode == MeasureMode::Stretch)
                {
                    control->SetDesiredSize(Size(secondary_side_child_max_length, control->GetDesiredSize().height));
                }
            }

            actual_size_for_children.width = secondary_side_child_max_length;
        }

        return actual_size_for_children;
    }

    void LinearLayout::OnLayoutContent(const Rect& rect)
    {
        float current_main_side_anchor = 0;
        for(auto control: GetChildren())
        {
            const auto layout_params = control->GetLayoutParams();
            const auto size = control->GetDesiredSize();
            const auto alignment = orientation_ == Orientation::Horizontal ? layout_params->height.alignment : layout_params->width.alignment;

            auto&& calculate_secondary_side_anchor = [alignment](const float layout_length, const float control_length) -> float
            {
                switch (alignment)
                {
                case Alignment::Center:
                    return (layout_length - control_length) / 2;
                case Alignment::Start:
                    return 0;
                case Alignment::End:
                    return layout_length - control_length;
                default:
                    UnreachableCode();
                }
            };

            auto&& calculate_rect = [rect, size](const float anchor_left, const float anchor_top)
            {
                return Rect(Point(rect.left + anchor_left, rect.top + anchor_top), size);
            };

            if (orientation_ == Orientation::Horizontal)
            {
                control->Layout(calculate_rect(current_main_side_anchor, calculate_secondary_side_anchor(rect.height, size.height)));
                current_main_side_anchor += size.width;
            }
            else
            {
                control->Layout(calculate_rect(calculate_secondary_side_anchor(rect.width, size.width), current_main_side_anchor));
                current_main_side_anchor += size.height;
            }
        }
    }
}
//--------------------------------------------------------
//-------end of file: src\ui\controls\linear_layout.cpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\controls\list_item.cpp
//--------------------------------------------------------


namespace cru::ui::controls
{
    ListItem::ListItem() : Control(true)
    {
        const auto predefine_resources = UiManager::GetInstance()->GetPredefineResources();

        brushes_[State::Normal].border_brush = predefine_resources->list_item_normal_border_brush;
        brushes_[State::Normal].fill_brush   = predefine_resources->list_item_normal_fill_brush;
        brushes_[State::Hover] .border_brush = predefine_resources->list_item_hover_border_brush;
        brushes_[State::Hover] .fill_brush   = predefine_resources->list_item_hover_fill_brush;
        brushes_[State::Select].border_brush = predefine_resources->list_item_select_border_brush;
        brushes_[State::Select].fill_brush   = predefine_resources->list_item_select_fill_brush;
    }

    StringView ListItem::GetControlType() const
    {
        return control_type;
    }

    void ListItem::SetState(const State state)
    {
        state_ = state;
        Repaint();
    }

    void ListItem::OnDrawForeground(ID2D1DeviceContext* device_context)
    {
        const auto rect = Rect(Point::Zero(), GetRect(RectRange::Padding).GetSize());
        device_context->FillRectangle(Convert(rect), brushes_[state_].fill_brush.Get());
        device_context->DrawRectangle(Convert(rect.Shrink(Thickness(0.5))), brushes_[state_].border_brush.Get(), 1);
    }

    void ListItem::OnMouseEnterCore(events::MouseEventArgs& args)
    {
        if (GetState() == State::Select)
            return;

        if (IsAnyMouseButtonDown())
            return;

        SetState(State::Hover);
    }

    void ListItem::OnMouseLeaveCore(events::MouseEventArgs& args)
    {
        if (GetState() == State::Select)
            return;

        SetState(State::Normal);
    }

    void ListItem::OnMouseClickCore(events::MouseButtonEventArgs& args)
    {
        if (args.GetMouseButton() == MouseButton::Left)
            SetState(State::Select);
    }
}
//--------------------------------------------------------
//-------end of file: src\ui\controls\list_item.cpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\controls\popup_menu.cpp
//--------------------------------------------------------


namespace cru::ui::controls
{
    Window* CreatePopupMenu(const Point& anchor, const std::vector<MenuItemInfo>& items, Window* parent)
    {
        const auto popup = Window::CreatePopup(parent);

        popup->lose_focus_event.AddHandler([popup](events::FocusChangeEventArgs& args)
        {
            if (args.IsWindow())
                popup->Close();
        });

        const auto create_menu_item = [popup](const String& text, const std::function<void()>& action) -> ListItem*
        {
            auto text_block = TextBlock::Create(text);
            text_block->GetLayoutParams()->width.alignment = Alignment::Start;

            auto list_item = CreateWithLayout<ListItem>(
                LayoutSideParams::Stretch(Alignment::Center),
                LayoutSideParams::Content(Alignment::Start),
                ControlList{ text_block }
            );

            list_item->mouse_click_event.AddHandler([popup, action](events::MouseButtonEventArgs& args)
            {
                if (args.GetMouseButton() == MouseButton::Left)
                {
                    action();
                    popup->Close();
                }
            });

            return list_item;
        };

        const auto menu = LinearLayout::Create(LinearLayout::Orientation::Vertical);

        menu->SetBordered(true);

        for (const auto& item : items)
            menu->AddChild(create_menu_item(item.first, item.second));

        popup->AddChild(menu);

        popup->SetSizeFitContent();
        popup->SetWindowPosition(anchor);

        return popup;
    }
}
//--------------------------------------------------------
//-------end of file: src\ui\controls\popup_menu.cpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\controls\text_block.cpp
//--------------------------------------------------------


namespace cru::ui::controls
{
    TextBlock::TextBlock() : TextControl(
        UiManager::GetInstance()->GetPredefineResources()->text_block_text_format,
        UiManager::GetInstance()->GetPredefineResources()->text_block_text_brush
    )
    {

    }

    StringView TextBlock::GetControlType() const
    {
        return control_type;
    }
}
//--------------------------------------------------------
//-------end of file: src\ui\controls\text_block.cpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\controls\text_box.cpp
//--------------------------------------------------------

#include <cwctype>
#include <cassert>


namespace cru::ui::controls
{
    TextBox::TextBox() : TextControl(
        UiManager::GetInstance()->GetPredefineResources()->text_box_text_format,
        UiManager::GetInstance()->GetPredefineResources()->text_box_text_brush
    )
    {
        SetSelectable(true);

        caret_brush_ = UiManager::GetInstance()->GetPredefineResources()->text_box_caret_brush;

        GetBorderProperty() = UiManager::GetInstance()->GetPredefineResources()->text_box_border;
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
            const auto caret_half_width = UiManager::GetInstance()->GetCaretInfo().half_caret_width;
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
        caret_timer_ = SetInterval(UiManager::GetInstance()->GetCaretInfo().caret_blink_duration, [this]
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
//--------------------------------------------------------
//-------end of file: src\ui\controls\text_box.cpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\controls\text_control.cpp
//--------------------------------------------------------

#include <cassert>


namespace cru::ui::controls
{
    TextControl::TextControl(const Microsoft::WRL::ComPtr<IDWriteTextFormat>& init_text_format,
        const Microsoft::WRL::ComPtr<ID2D1Brush>& init_brush) : Control(false)
    {
        text_format_ = init_text_format;

        RecreateTextLayout();

        brush_ = init_brush;

        selection_brush_ = UiManager::GetInstance()->GetPredefineResources()->text_control_selection_brush;
    }


    void TextControl::SetText(const String& text)
    {
        if (text_ != text)
        {
            const auto old_text = text_;
            text_ = text;
            OnTextChangedCore(old_text, text);
        }
    }

    void TextControl::SetBrush(const Microsoft::WRL::ComPtr<ID2D1Brush>& brush)
    {
        brush_ = brush;
        Repaint();
    }

    void TextControl::SetTextFormat(const Microsoft::WRL::ComPtr<IDWriteTextFormat>& text_format)
    {
        text_format_ = text_format;
        RecreateTextLayout();
        Repaint();
    }

    void TextControl::SetSelectable(const bool is_selectable)
    {
        if (is_selectable_ != is_selectable)
        {
            if (!is_selectable)
            {
                if (is_selecting_)
                {
                    is_selecting_ = false;
                    GetWindow()->ReleaseCurrentMouseCapture();
                }
                selected_range_ = std::nullopt;
                Repaint();
            }
            is_selectable_ = is_selectable;
            UpdateCursor(std::nullopt);
        }
    }

    void TextControl::SetSelectedRange(std::optional<TextRange> text_range)
    {
        if (is_selectable_)
        {
            selected_range_ = text_range;
            Repaint();
        }
    }

    void TextControl::OnSizeChangedCore(events::SizeChangedEventArgs& args)
    {
        Control::OnSizeChangedCore(args);
        const auto content = GetRect(RectRange::Content);
        ThrowIfFailed(text_layout_->SetMaxWidth(content.width));
        ThrowIfFailed(text_layout_->SetMaxHeight(content.height));
        Repaint();
    }

    namespace
    {
        unsigned TextLayoutHitTest(IDWriteTextLayout* text_layout, const Point& point)
        {
            BOOL is_trailing, is_inside;
            DWRITE_HIT_TEST_METRICS metrics{};
            text_layout->HitTestPoint(point.x, point.y, &is_trailing, &is_inside, &metrics);
            return is_trailing == 0 ? metrics.textPosition : metrics.textPosition + 1;
        }

        void DrawSelectionRect(ID2D1DeviceContext* device_context, IDWriteTextLayout* layout, ID2D1Brush* brush, const std::optional<TextRange> range)
        {
            if (range.has_value())
            {
                DWRITE_TEXT_METRICS text_metrics{};
                ThrowIfFailed(layout->GetMetrics(&text_metrics));
                const auto metrics_count = text_metrics.lineCount * text_metrics.maxBidiReorderingDepth;

                std::vector<DWRITE_HIT_TEST_METRICS> hit_test_metrics(metrics_count);
                UINT32 actual_count;
                layout->HitTestTextRange(
                    range.value().position, range.value().count,
                    0, 0,
                    hit_test_metrics.data(), metrics_count, &actual_count
                );

                hit_test_metrics.erase(hit_test_metrics.cbegin() + actual_count, hit_test_metrics.cend());

                for (const auto& metrics : hit_test_metrics)
                    device_context->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(metrics.left, metrics.top, metrics.left + metrics.width, metrics.top + metrics.height), 3, 3), brush);
            }
        }
    }
    void TextControl::OnDrawContent(ID2D1DeviceContext* device_context)
    {
        Control::OnDrawContent(device_context);
        DrawSelectionRect(device_context, text_layout_.Get(), selection_brush_.Get(), selected_range_);
        device_context->DrawTextLayout(D2D1::Point2F(), text_layout_.Get(), brush_.Get());
    }

    void TextControl::OnMouseDownCore(events::MouseButtonEventArgs& args)
    {
        Control::OnMouseDownCore(args);
        if (is_selectable_ && args.GetMouseButton() == MouseButton::Left && GetRect(RectRange::Padding).IsPointInside(args.GetPoint(this, RectRange::Margin)))
        {
            selected_range_ = std::nullopt;
            const auto hit_test_result = TextLayoutHitTest(text_layout_.Get(), args.GetPoint(this));
            RequestChangeCaretPosition(hit_test_result);
            mouse_down_position_ = hit_test_result;
            is_selecting_ = true;
            GetWindow()->CaptureMouseFor(this);
            Repaint();
        }
    }

    void TextControl::OnMouseMoveCore(events::MouseEventArgs& args)
    {
        Control::OnMouseMoveCore(args);
        if (is_selecting_)
        {
            const auto hit_test_result = TextLayoutHitTest(text_layout_.Get(), args.GetPoint(this));
            RequestChangeCaretPosition(hit_test_result);
            selected_range_ = TextRange::FromTwoSides(hit_test_result, mouse_down_position_);
            Repaint();
        }
        UpdateCursor(args.GetPoint(this, RectRange::Margin));
    }

    void TextControl::OnMouseUpCore(events::MouseButtonEventArgs& args)
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

    void TextControl::OnLoseFocusCore(events::FocusChangeEventArgs& args)
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


    Size TextControl::OnMeasureContent(const Size& available_size)
    {
        ThrowIfFailed(text_layout_->SetMaxWidth(available_size.width));
        ThrowIfFailed(text_layout_->SetMaxHeight(available_size.height));

        DWRITE_TEXT_METRICS metrics{};

        ThrowIfFailed(text_layout_->GetMetrics(&metrics));

        const Size measure_result(metrics.width, metrics.height);

        return measure_result;
    }

    void TextControl::RequestChangeCaretPosition(unsigned position)
    {

    }

    void TextControl::OnTextChangedCore(const String& old_text, const String& new_text)
    {
        RecreateTextLayout();
        InvalidateLayout();
        Repaint();
    }

    void TextControl::RecreateTextLayout()
    {
        assert(text_format_ != nullptr);

        text_layout_ = nullptr;

        const auto dwrite_factory = graph::GraphManager::GetInstance()->GetDWriteFactory();

        const auto&& size = GetSize();

        ThrowIfFailed(dwrite_factory->CreateTextLayout(
            text_.c_str(), static_cast<UINT32>(text_.size()),
            text_format_.Get(),
            size.width, size.height,
            &text_layout_
        ));
    }

    void TextControl::UpdateCursor(const std::optional<Point>& point)
    {
        if (!is_selectable_)
        {
            SetCursor(nullptr);
            return;
        }

        const auto window = GetWindow();
        if (window == nullptr)
        {
            SetCursor(nullptr);
            return;
        }

        if (is_selecting_)
        {
            SetCursor(cursors::i_beam);
            return;
        }

        const auto p = point.value_or(WindowToControl(window->GetMousePosition()));
        if (GetRect(RectRange::Padding).IsPointInside(p))
            SetCursor(cursors::i_beam);
        else
            SetCursor(nullptr);
    }
}
//--------------------------------------------------------
//-------end of file: src\ui\controls\text_control.cpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\controls\toggle_button.cpp
//--------------------------------------------------------


namespace cru::ui::controls
{
    using animations::AnimationBuilder;

    // ui length parameters of toggle button.
    constexpr float half_height = 15;
    constexpr float half_width = half_height * 2;
    constexpr float stroke_width = 3;
    constexpr float inner_circle_radius = half_height - stroke_width;
    constexpr float inner_circle_x = half_width - half_height;

    ToggleButton::ToggleButton() : current_circle_position_(-inner_circle_x)
    {
        graph::GraphManager::GetInstance()->GetD2D1Factory()->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(-half_width, -half_height, half_width, half_height), half_height, half_height), &frame_path_);

        on_brush_ = UiManager::GetInstance()->GetPredefineResources()->toggle_button_on_brush;
        off_brush_ = UiManager::GetInstance()->GetPredefineResources()->toggle_button_off_brush;
    }

    inline D2D1_POINT_2F ConvertPoint(const Point& point)
    {
        return D2D1::Point2F(point.x, point.y);
    }

    StringView ToggleButton::GetControlType() const
    {
        return control_type;
    }

    bool ToggleButton::IsPointInside(const Point& point)
    {
        const auto size = GetSize();
        const auto transform = D2D1::Matrix3x2F::Translation(size.width / 2, size.height / 2);
        BOOL contains;
        frame_path_->FillContainsPoint(ConvertPoint(point), transform, &contains);
        if (!contains)
            frame_path_->StrokeContainsPoint(ConvertPoint(point), stroke_width, nullptr, transform, &contains);
        return contains != 0;
    }

    void ToggleButton::SetState(const bool state)
    {
        if (state != state_)
        {
            state_ = state;
            float destination_x;

            if (state)
                destination_x = inner_circle_x;
            else
                destination_x = -inner_circle_x;

            const auto previous_position = current_circle_position_;
            const auto delta = destination_x - current_circle_position_;

            constexpr auto total_time = FloatSecond(0.2);

            const auto time = total_time * (std::abs(delta) / (inner_circle_x * 2));

            // ReSharper disable once CppExpressionWithoutSideEffects
            AnimationBuilder(Format(L"ToggleButton {}", reinterpret_cast<size_t>(this)), time)
                .AddStepHandler([=](auto, const double percentage)
                {
                    current_circle_position_ = static_cast<float>(previous_position + delta * percentage);
                    Repaint();
                })
                .Start();

            RaiseToggleEvent(state);
            Repaint();
        }
    }

    void ToggleButton::Toggle()
    {
        SetState(!GetState());
    }

    void ToggleButton::OnToggle(events::ToggleEventArgs& args)
    {

    }

    void ToggleButton::OnDrawContent(ID2D1DeviceContext* device_context)
    {
        Control::OnDrawContent(device_context);
        const auto size = GetSize();
        graph::WithTransform(device_context, D2D1::Matrix3x2F::Translation(size.width / 2, size.height / 2), [this](ID2D1DeviceContext* device_context)
        {
            if (state_)
            {
                device_context->DrawGeometry(frame_path_.Get(), on_brush_.Get(), stroke_width);
                device_context->FillEllipse(D2D1::Ellipse(D2D1::Point2F(current_circle_position_, 0), inner_circle_radius, inner_circle_radius), on_brush_.Get());
            }
            else
            {
                device_context->DrawGeometry(frame_path_.Get(), off_brush_.Get(), stroke_width);
                device_context->FillEllipse(D2D1::Ellipse(D2D1::Point2F(current_circle_position_, 0), inner_circle_radius, inner_circle_radius), off_brush_.Get());
            }
        });
    }

    void ToggleButton::OnMouseClickCore(events::MouseButtonEventArgs& args)
    {
        Control::OnMouseClickCore(args);
        Toggle();
    }

    Size ToggleButton::OnMeasureContent(const Size& available_size)
    {
        const Size result_size(
            half_width * 2 + stroke_width,
            half_height * 2 + stroke_width
        );

        return result_size;
    }

    void ToggleButton::RaiseToggleEvent(bool new_state)
    {
        events::ToggleEventArgs args(this, this, new_state);
        OnToggle(args);
        toggle_event.Raise(args);
    }
}
//--------------------------------------------------------
//-------end of file: src\ui\controls\toggle_button.cpp
//--------------------------------------------------------
//--------------------------------------------------------
//-------begin of file: src\ui\events\ui_event.cpp
//--------------------------------------------------------


namespace cru::ui::events
{
    Point MouseEventArgs::GetPoint(Control* control, const RectRange range) const
    {
        if (point_.has_value())
            return control->TransformPoint(control->WindowToControl(point_.value()), RectRange::Margin, range);
        return Point();
    }
}
//--------------------------------------------------------
//-------end of file: src\ui\events\ui_event.cpp
//--------------------------------------------------------
