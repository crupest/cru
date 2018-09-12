#include "application.h"

#include <fmt/format.h>

#include "timer.h"
#include "ui/window.h"
#include "graph/graph.h"

namespace cru {
    constexpr auto god_window_class_name = L"GodWindowClass";
    constexpr int invoke_later_message_id = WM_USER + 2000;

    Application* Application::instance_ = nullptr;

    Application * Application::GetInstance() {
        return instance_;
    }

    Application::Application(HINSTANCE h_instance)
        : h_instance_(h_instance) {

        if (instance_)
            throw std::runtime_error("A application instance already exists.");

        instance_ = this;

        god_window_class_ = std::make_unique<ui::WindowClass>(god_window_class_name, GodWndProc, h_instance);

        const auto hwnd = CreateWindowEx(0,
            god_window_class_name,
            L"", 0,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            nullptr, nullptr, h_instance, nullptr
        );

        if (hwnd == nullptr)
            throw std::runtime_error("Failed to create window.");

        god_hwnd_ = hwnd;

        window_manager_ = std::make_unique<ui::WindowManager>();
        graph_manager_ = std::make_unique<graph::GraphManager>();
        timer_manager_ = std::make_unique<TimerManager>();
    }

    Application::~Application()
    {
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

    LRESULT Application::GodWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
    {
        const auto app = Application::GetInstance();

        if (app)
        {
            const auto result = app->HandleGodWindowMessage(hWnd, Msg, wParam, lParam);
            if (result.has_value())
                return result.value();
            else
                return DefWindowProc(hWnd, Msg, wParam, lParam);
        }
        else
            return DefWindowProc(hWnd, Msg, wParam, lParam);
    }

    std::optional<LRESULT> Application::HandleGodWindowMessage(HWND hwnd, int msg, WPARAM w_param, LPARAM l_param)
    {
        switch (msg)
        {
        case invoke_later_message_id:
        {
            const auto p_action = reinterpret_cast<InvokeLaterAction*>(w_param);
            (*p_action)();
            delete p_action;
            return 0;
        }
        case WM_TIMER:
        {
            const auto action = GetTimerManager()->GetAction(static_cast<UINT_PTR>(w_param));
            if (action.has_value())
            {
                action.value()();
                return 0;
            }
            break;
        }
        default:
            return std::nullopt;
        }
        return std::nullopt;
    }

    void InvokeLater(InvokeLaterAction&& action) {
        //copy the action to a safe place
        auto p_action_copy = new InvokeLaterAction(std::move(action));

        if (PostMessageW(Application::GetInstance()->GetGodWindowHandle(), invoke_later_message_id, reinterpret_cast<WPARAM>(p_action_copy), 0) == 0)
            throw std::runtime_error(fmt::format("Last error: {}.", ::GetLastError()));
    }
}
