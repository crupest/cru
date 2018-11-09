#pragma once

#include "system_headers.hpp"
#include <memory>
#include <optional>
#include <functional>
#include <typeindex>
#include <type_traits>

#include "base.hpp"

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

#ifdef CRU_DEBUG_LAYOUT
    struct DebugLayoutResource
    {
        Microsoft::WRL::ComPtr<ID2D1Brush> out_border_brush;
        Microsoft::WRL::ComPtr<ID2D1Brush> margin_brush;
        Microsoft::WRL::ComPtr<ID2D1Brush> padding_brush;
    };
#endif

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


        HINSTANCE GetInstanceHandle() const
        {
            return h_instance_;
        }

        GodWindow* GetGodWindow() const
        {
            return god_window_.get();
        }

        // Resolve a singleton.
        // All singletons will be delete in reverse order of resolve.
        template<typename T, typename = std::enable_if_t<std::is_base_of_v<Object, T>>>
        T* ResolveSingleton(const std::function<T*(Application*)>& creator)
        {
            const auto& index = std::type_index{typeid(T)};
            const auto find_result = singleton_map_.find(index);
            if (find_result != singleton_map_.cend())
                return static_cast<T*>(find_result->second);

            auto singleton = creator(this);
            singleton_map_.emplace(index, static_cast<Object*>(singleton));
            singleton_list_.push_back(singleton);
            return singleton;
        }


        CaretInfo GetCaretInfo() const
        {
            return caret_info_;
        }

#ifdef CRU_DEBUG_LAYOUT
        const DebugLayoutResource* GetDebugLayoutResource() const
        {
            return &debug_layout_resource_;
        }
#endif

    private:
        HINSTANCE h_instance_;

        std::unique_ptr<GodWindow> god_window_;

        std::unordered_map<std::type_index, Object*> singleton_map_;
        std::list<Object*> singleton_list_; // used for reverse destroy.

#ifdef CRU_DEBUG_LAYOUT
        DebugLayoutResource debug_layout_resource_;
#endif

        CaretInfo caret_info_;
    };


    void InvokeLater(const std::function<void()>& action);
}
