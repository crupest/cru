#include "application.h"

#include "timer.h"
#include "ui/window.h"
#include "graph/graph.h"

namespace cru {
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
            if (!HandleThreadMessage(msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);

            }
        }

        return static_cast<int>(msg.wParam);
    }

    void Application::Quit(const int quit_code) {
        ::PostQuitMessage(quit_code);
    }

    bool Application::HandleThreadMessage(const MSG& message)
    {
        if (message.hwnd != nullptr)
            return false;

        switch (message.message)
        {
        case invoke_later_message_id:
        {
            const auto p_action = reinterpret_cast<InvokeLaterAction*>(message.wParam);
            (*p_action)();
            delete p_action;
            return true;
        }
        case WM_TIMER:
        {
            const auto action = timer_manager_->GetAction(static_cast<UINT_PTR>(message.wParam));
            if (action.has_value())
            {
                action.value()();
                return true;
            }
            break;
        }
        default:
            return false;
        }
        return false;
    }

    void InvokeLater(const InvokeLaterAction& action) {
        //copy the action to a safe place
        auto p_action_copy = new InvokeLaterAction(action);

        PostMessage(
            nullptr,
            invoke_later_message_id,
            reinterpret_cast<WPARAM>(p_action_copy),
            0
        );
    }
}
