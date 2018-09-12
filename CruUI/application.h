#pragma once

#include "system_headers.h"
#include <memory>
#include <optional>

#include "base.h"

namespace cru 
{
    namespace ui
    {
        class WindowClass;
        class WindowManager;
    }

    namespace graph
    {
        class GraphManager;
    }

    class TimerManager;

    class Application : public Object
    {
    public:
        static Application* GetInstance();
    private:
        static Application* instance_;

    public:
        explicit Application(HINSTANCE h_instance);
        Application(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator = (const Application&) = delete;
        Application& operator = (Application&&) = delete;
        ~Application() override;

    public:
        int Run();
        void Quit(int quit_code);

        ui::WindowManager* GetWindowManager() const
        {
            return window_manager_.get();
        }

        graph::GraphManager* GetGraphManager() const
        {
            return graph_manager_.get();
        }

        TimerManager* GetTimerManager() const
        {
            return timer_manager_.get();
        }

        HINSTANCE GetInstanceHandle() const
        {
            return h_instance_;
        }

        HWND GetGodWindowHandle() const
        {
            return god_hwnd_;
        }

    private:
        static LRESULT __stdcall GodWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
        std::optional<LRESULT> HandleGodWindowMessage(HWND hwnd, int msg, WPARAM w_param, LPARAM l_param);

    private:
        HINSTANCE h_instance_;
        
        std::unique_ptr<ui::WindowClass> god_window_class_;
        HWND god_hwnd_;

        std::unique_ptr<ui::WindowManager> window_manager_;
        std::unique_ptr<graph::GraphManager> graph_manager_;
        std::unique_ptr<TimerManager> timer_manager_;
    };


    using InvokeLaterAction = Action<>;
    void InvokeLater(InvokeLaterAction&& action);
}
