#include "window.h"

#include <fmt/format.h>

#include "application.h"
#include "graph/graph.h"
#include "exception.h"

namespace cru
{
    namespace ui
    {
        WindowClass::WindowClass(const String& name, WNDPROC window_proc, HINSTANCE h_instance)
            : name_(name)
        {
            WNDCLASSEX window_class;
            window_class.cbSize = sizeof(WNDCLASSEX);

            window_class.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
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
            auto window = Application::GetInstance()->GetWindowManager()->FromHandle(hWnd);

            LRESULT result;
            if (window != nullptr && window->HandleWindowMessage(hWnd, Msg, wParam, lParam, result))
                return result;

            return DefWindowProc(hWnd, Msg, wParam, lParam);
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

        inline Point PiToDip(const POINT& pi_point)
        {
            return Point(
                graph::PixelToDipX(pi_point.x),
                graph::PixelToDipY(pi_point.y)
            );
        }

        Window::Window() : Control(WindowConstructorTag{}, this), control_list_({ this }) {
            const auto app = Application::GetInstance();
            hwnd_ = CreateWindowEx(0,
                app->GetWindowManager()->GetGeneralWindowClass()->GetName(),
                L"", WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                nullptr, nullptr, app->GetInstanceHandle(), nullptr
            );

            if (hwnd_ == nullptr)
                throw std::runtime_error("Failed to create window.");

            app->GetWindowManager()->RegisterWindow(hwnd_, this);

            render_target_ = app->GetGraphManager()->CreateWindowRenderTarget(hwnd_);
        }

        Window::~Window() {
            Close();
            TraverseDescendants([this](Control* control) {
                control->OnDetachToWindow(this);
            });
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

        bool Window::HandleWindowMessage(HWND hwnd, int msg, WPARAM w_param, LPARAM l_param, LRESULT & result) {
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
                if (control->IsPointInside(control->AbsoluteToLocal(point))) {
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

        void Window::OnDestroyInternal() {
            Application::GetInstance()->GetWindowManager()->UnregisterWindow(hwnd_);
            hwnd_ = nullptr;
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

        void Window::OnResizeInternal(int new_width, int new_height) {
            render_target_->ResizeBuffer(new_width, new_height);
            Relayout();
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
                    DispatchEvent(new_control, &Control::RaiseMouseEnterEvent, lowest_common_ancestor, point); // dispatch mouse enter event.
            }
        }
    }
}
