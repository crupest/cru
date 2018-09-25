#pragma once

#include "system_headers.h"
#include <memory>
#include <optional>

#include "base.h"

namespace cru 
{
    class Application;

    namespace ui
    {
        class WindowClass;
        class WindowManager;

        namespace animations::details
        {
            class AnimationManager;
        }
    }

    namespace graph
    {
        class GraphManager;
    }

    class TimerManager;

    struct CaretInfo
    {
        std::chrono::milliseconds caret_blink_duration;
        float half_caret_width;
    };

    class GodWindow : public Object
    {
    public:
        explicit GodWindow(Application* application);
        GodWindow(const GodWindow& other) = delete;
        GodWindow(GodWindow&& other) = delete;
        GodWindow& operator=(const GodWindow& other) = delete;
        GodWindow& operator=(GodWindow&& other) = delete;
        ~GodWindow() override;

        HWND GetHandle() const
        {
            return hwnd_;
        }

        std::optional<LRESULT> HandleGodWindowMessage(HWND hwnd, int msg, WPARAM w_param, LPARAM l_param);

    private:
        Application* application_;
        std::unique_ptr<ui::WindowClass> god_window_class_;
        HWND hwnd_;
    };

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

        ui::animations::details::AnimationManager* GetAnimationManager() const
        {
            return animation_manager_.get();
        }

        HINSTANCE GetInstanceHandle() const
        {
            return h_instance_;
        }

        GodWindow* GetGodWindow() const
        {
            return god_window_.get();
        }

        CaretInfo GetCaretInfo() const
        {
            return caret_info_;
        }

#ifdef CRU_DEBUG_DRAW_CONTROL_BORDER
        Microsoft::WRL::ComPtr<ID2D1Brush> GetDebugBorderBrush() const
        {
            return debug_border_brush_;
        }
#endif

    private:
        HINSTANCE h_instance_;
        
        std::unique_ptr<ui::WindowManager> window_manager_;
        std::unique_ptr<graph::GraphManager> graph_manager_;
        std::unique_ptr<TimerManager> timer_manager_;
        std::unique_ptr<ui::animations::details::AnimationManager> animation_manager_;

        std::unique_ptr<GodWindow> god_window_;

#ifdef CRU_DEBUG_DRAW_CONTROL_BORDER
        Microsoft::WRL::ComPtr<ID2D1Brush> debug_border_brush_;
#endif

        CaretInfo caret_info_;
    };


    using InvokeLaterAction = Function<void()>;
    void InvokeLater(InvokeLaterAction&& action);
}