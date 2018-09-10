#include "graph.h"

#include "application.h"
#include "exception.h"

namespace cru {
    namespace graph {
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
            return Application::GetInstance()->GetGraphManager();
        }

        GraphManager::GraphManager()
        {
            UINT creation_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef _DEBUG
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
}
