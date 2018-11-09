#pragma once

#include "system_headers.hpp"
#include <memory>
#include <functional>

#include "base.hpp"


namespace cru::graph
{
    class GraphManager;

    //Represents a window render target.
    class WindowRenderTarget : public Object
    {
    public:
        WindowRenderTarget(GraphManager* graph_manager, HWND hwnd);
        WindowRenderTarget(const WindowRenderTarget& other) = delete;
        WindowRenderTarget(WindowRenderTarget&& other) = delete;
        WindowRenderTarget& operator=(const WindowRenderTarget& other) = delete;
        WindowRenderTarget& operator=(WindowRenderTarget&& other) = delete;
        ~WindowRenderTarget() override;

    public:
        //Get the graph manager that created the render target.
        GraphManager* GetGraphManager() const
        {
            return graph_manager_;
        }

        //Get the d2d device context.
        inline Microsoft::WRL::ComPtr<ID2D1DeviceContext> GetD2DDeviceContext() const;

        //Get the target bitmap which can be set as the ID2D1DeviceContext's target.
        Microsoft::WRL::ComPtr<ID2D1Bitmap1> GetTargetBitmap() const
        {
            return target_bitmap_;
        }

        //Resize the underlying buffer.
        void ResizeBuffer(int width, int height);

        //Set this render target as the d2d device context's target.
        void SetAsTarget();

        //Present the data of the underlying buffer to the window.
        void Present();

    private:
        void CreateTargetBitmap();

    private:
        GraphManager* graph_manager_;
        Microsoft::WRL::ComPtr<IDXGISwapChain1> dxgi_swap_chain_;
        Microsoft::WRL::ComPtr<ID2D1Bitmap1> target_bitmap_;
    };

    struct Dpi
    {
        float x;
        float y;
    };

    class GraphManager final : public Object
    {
    public:
        static GraphManager* GetInstance();

    private:
        GraphManager();
    public:
        GraphManager(const GraphManager& other) = delete;
        GraphManager(GraphManager&& other) = delete;
        GraphManager& operator=(const GraphManager& other) = delete;
        GraphManager& operator=(GraphManager&& other) = delete;
        ~GraphManager() override;

    public:
        Microsoft::WRL::ComPtr<ID2D1Factory1> GetD2D1Factory() const
        {
            return d2d1_factory_;
        }

        Microsoft::WRL::ComPtr<ID2D1DeviceContext> GetD2D1DeviceContext() const
        {
            return d2d1_device_context_;
        }

        Microsoft::WRL::ComPtr<ID3D11Device> GetD3D11Device() const
        {
            return d3d11_device_;
        }

        Microsoft::WRL::ComPtr<IDXGIFactory2> GetDxgiFactory() const
        {
            return dxgi_factory_;
        }

        Microsoft::WRL::ComPtr<IDWriteFactory> GetDWriteFactory() const
        {
            return dwrite_factory_;
        }


        //Create a window render target with the HWND.
        std::shared_ptr<WindowRenderTarget> CreateWindowRenderTarget(HWND hwnd);

        //Get the desktop dpi.
        Dpi GetDpi() const;

        //Reload system metrics including desktop dpi.
        void ReloadSystemMetrics();

        Microsoft::WRL::ComPtr<IDWriteFontCollection> GetSystemFontCollection() const
        {
            return dwrite_system_font_collection_.Get();
        }

    private:
        Microsoft::WRL::ComPtr<ID3D11Device> d3d11_device_;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3d11_device_context_;
        Microsoft::WRL::ComPtr<ID2D1Factory1> d2d1_factory_;
        Microsoft::WRL::ComPtr<ID2D1Device> d2d1_device_;
        Microsoft::WRL::ComPtr<ID2D1DeviceContext> d2d1_device_context_;
        Microsoft::WRL::ComPtr<IDXGIFactory2> dxgi_factory_;

        Microsoft::WRL::ComPtr<IDWriteFactory> dwrite_factory_;
        Microsoft::WRL::ComPtr<IDWriteFontCollection> dwrite_system_font_collection_;
    };

    inline int DipToPixelInternal(const float dip, const float dpi)
    {
        return static_cast<int>(dip * dpi / 96.0f);
    }

    inline int DipToPixelX(const float dip_x)
    {
        return DipToPixelInternal(dip_x, GraphManager::GetInstance()->GetDpi().x);
    }

    inline int DipToPixelY(const float dip_y)
    {
        return DipToPixelInternal(dip_y, GraphManager::GetInstance()->GetDpi().y);
    }

    inline float DipToPixelInternal(const int pixel, const float dpi)
    {
        return static_cast<float>(pixel) * 96.0f / dpi;
    }

    inline float PixelToDipX(const int pixel_x)
    {
        return DipToPixelInternal(pixel_x, GraphManager::GetInstance()->GetDpi().x);
    }

    inline float PixelToDipY(const int pixel_y)
    {
        return DipToPixelInternal(pixel_y, GraphManager::GetInstance()->GetDpi().y);
    }

    Microsoft::WRL::ComPtr<ID2D1DeviceContext> WindowRenderTarget::GetD2DDeviceContext() const
    {
        return graph_manager_->GetD2D1DeviceContext();
    }

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> CreateSolidBrush(const D2D1_COLOR_F& color);
    Microsoft::WRL::ComPtr<IDWriteTextFormat> CreateDefaultTextFormat();

    inline void WithTransform(ID2D1DeviceContext* device_context, const D2D1_MATRIX_3X2_F matrix, const std::function<void(ID2D1DeviceContext*)>& action)
    {
        D2D1_MATRIX_3X2_F old_transform;
        device_context->GetTransform(&old_transform);
        device_context->SetTransform(old_transform * matrix);
        action(device_context);
        device_context->SetTransform(old_transform);
    }
}
