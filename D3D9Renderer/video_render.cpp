#include "video_render.h"

#pragma comment(lib, "../common/libyuv_internal.lib")

WebrtcVideoRendererD3D9Impl::WebrtcVideoRendererD3D9Impl(HWND wnd)
{
    wnd_ = wnd;
    memset(&locked_rect_, 0, sizeof(locked_rect_));
}

bool WebrtcVideoRendererD3D9Impl::InitRender(int w, int h)
{
    d3d_handle_ = Direct3DCreate9(D3D_SDK_VERSION);
    if (d3d_handle_ == nullptr)
    {
        return false;
    }

    image_width_ = w;
    image_height_ = h;
    window_aspect_ = (double)w / (double)h;
    if (movie_src_fmt_ == D3DFMT_UNKNOWN)
    {
        //movie_src_fmt_ = (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2');
        movie_src_fmt_ = D3DFMT_A8R8G8B8;
    }

    HRESULT hr;
    D3DDISPLAYMODE disp_mode;
    hr = d3d_handle_->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &disp_mode);
    if (FAILED(hr))
    {
        DestoryRender();
        return false;
    }

    desktop_fmt_ = disp_mode.Format;
    cur_back_buf_width_ = disp_mode.Width;
    cur_backbuf_height_ = disp_mode.Height;

    D3DCAPS9 disp_caps;
    hr = d3d_handle_->GetDeviceCaps(D3DADAPTER_DEFAULT,
                                    D3DDEVTYPE_HAL,
                                    &disp_caps);
    if (FAILED(hr))
    {
        DestoryRender();
        return false;
    }
    if (!ConfigureD3D())
    {
        DestoryRender();
        return false;
    }

    UpdateWindowRect();

    return true;
}

void WebrtcVideoRendererD3D9Impl::DestoryRender()
{
    DestroyD3DSurfaces();
    if (d3d_back_buf_)
    {
        d3d_back_buf_->Release();
        d3d_back_buf_ = NULL;
    }
    memset(&locked_rect_, 0, sizeof(locked_rect_));
}

void WebrtcVideoRendererD3D9Impl::FillD3DPresentparams(D3DPRESENT_PARAMETERS* present_params)
{
    memset(present_params, 0, sizeof(D3DPRESENT_PARAMETERS));
    present_params->Windowed = TRUE;
    present_params->SwapEffect = D3DSWAPEFFECT_COPY;
    present_params->Flags = D3DPRESENTFLAG_VIDEO;
    present_params->hDeviceWindow = wnd_;
    present_params->MultiSampleType = D3DMULTISAMPLE_NONE;
    present_params->PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    present_params->BackBufferFormat = desktop_fmt_;
    present_params->BackBufferCount = 1;
    present_params->EnableAutoDepthStencil = FALSE;
}

bool WebrtcVideoRendererD3D9Impl::ConfigureD3D()
{
    D3DDISPLAYMODE disp_mode;
    D3DVIEWPORT9 vp = { 0, 0, image_width_, image_height_, 0, 1 };
    HRESULT hr;

    DestroyD3DSurfaces();
    hr = d3d_handle_->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &disp_mode);
    if (FAILED(hr))
    {
        DestoryRender();
        return false;
    }
    desktop_fmt_ = disp_mode.Format;
    if (!ChangeD3DBackBuffer(D3DBackBuffer_Create))
    {
        return false;
    }
    if (!CreateD3DSurfaces())
    {
        return false;
    }

    hr = d3d_device_->SetViewport(&vp);
    if (FAILED(hr))
    {
        return false;
    }
    return true;
}

bool WebrtcVideoRendererD3D9Impl::ChangeD3DBackBuffer(D3DBackBufferAction action)
{
    HRESULT hr;
    DestroyD3DSurfaces();
    if (image_width_ > cur_back_buf_width_)
        cur_back_buf_width_ = image_width_;

    if (image_height_ > cur_backbuf_height_)
        cur_backbuf_height_ = image_height_;

    RECT rect_win = { 0 };
    GetClientRect(wnd_, &rect_win);

    present_params_.BackBufferWidth = rect_win.right - rect_win.left;
    present_params_.BackBufferHeight = rect_win.bottom - rect_win.top;
    present_params_.BackBufferWidth = (present_params_.BackBufferWidth >> 1) << 1;
    present_params_.BackBufferHeight = (present_params_.BackBufferHeight >> 1) << 1;

    FillD3DPresentparams(&present_params_);
    if (d3d_handle_)
    {
        if (d3d_device_)
        {
            d3d_device_->Release();
            d3d_device_ = nullptr;
        }
        hr = d3d_handle_->CreateDevice(D3DADAPTER_DEFAULT,
                                       D3DDEVTYPE_HAL, wnd_,
                                       D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE | D3DCREATE_MULTITHREADED,
                                       &present_params_, &d3d_device_);
        if (FAILED(hr))
        {
            DestoryRender();
            return false;
        }
    }

    if (action == D3DBackBuffer_Reset)
    {
        hr = d3d_device_->Reset(&present_params_);
        if (FAILED(hr))
        {
            return false;
        }
    }
    return true;
}

void WebrtcVideoRendererD3D9Impl::DestroyD3DSurfaces()
{
    if (locked_rect_.pBits && d3d_surface_)
    {
        d3d_surface_->UnlockRect();
    }
    if (d3d_surface_)
    {
        d3d_surface_->Release();
        d3d_surface_ = NULL;
    }
    locked_rect_.pBits = NULL;

    if (d3d_back_buf_)
        d3d_back_buf_->Release();
    d3d_back_buf_ = NULL;
}

bool WebrtcVideoRendererD3D9Impl::CreateD3DSurfaces()
{
    if (!d3d_device_)
    {
        return false;
    }

    HRESULT hr = E_FAIL;
    if (!d3d_surface_)
    {
        if (movie_src_fmt_ == D3DFMT_A8R8G8B8)
        {
            hr = d3d_device_->CreateOffscreenPlainSurface(image_width_, image_height_,
                                                          movie_src_fmt_, D3DPOOL_DEFAULT,
                                                          &d3d_surface_, NULL);
            if (hr != S_OK)
            {
                movie_src_fmt_ = D3DFMT_YUY2;
                OutputDebugString(L"CreateOffscreenPlainSurface A8R8G8B8 failed!");
            }
            else
            {
                OutputDebugString(L"CreateOffscreenPlainSurface A8R8G8B8 success.");
            }
        }

        if (FAILED(hr))
        {
            return false;
        }
    }

    if (!d3d_back_buf_)
    {
        hr = d3d_device_->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &d3d_back_buf_);
        if (FAILED(hr))
        {
            return false;
        }
    }
    IDirect3DDevice9_SetRenderState(d3d_device_, D3DRS_SRCBLEND, D3DBLEND_ONE);
    IDirect3DDevice9_SetRenderState(d3d_device_, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    IDirect3DDevice9_SetRenderState(d3d_device_, D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    IDirect3DDevice9_SetRenderState(d3d_device_, D3DRS_ALPHAREF, (DWORD)0x0);
    IDirect3DDevice9_SetRenderState(d3d_device_, D3DRS_LIGHTING, FALSE);
    IDirect3DDevice9_SetSamplerState(d3d_device_, 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    IDirect3DDevice9_SetSamplerState(d3d_device_, 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    return true;
}

bool WebrtcVideoRendererD3D9Impl::ReconfigureD3D()
{
    DestroyD3DSurfaces();

    if (!ConfigureD3D())
    {
        return false;
    }
    return true;
}

void WebrtcVideoRendererD3D9Impl::DoRender(uint8_t* img_data, int width, int height)
{
    HRESULT  hr = E_FAIL;

    if (width != image_width_ ||
        height != image_height_)
    {
        if (!InitRender(width, height))
        {
            OutputDebugString(L"ReInitRender failed\n");
            return;
        }
    }


    if (d3d_device_)
    {
        hr = d3d_device_->TestCooperativeLevel();
        if (hr == D3DERR_DEVICELOST)
        {
            OutputDebugString(L"D3DERR_DEVICELOST");
            return;
        }
        else if (hr == D3DERR_DEVICENOTRESET)
        {
            OutputDebugString(L"D3DERR_DEVICENOTRESET");
            DestoryRender();
            if (d3d_device_)
            {
                d3d_device_->Release();
                d3d_device_ = NULL;
            }
            if (d3d_handle_)
            {
                d3d_handle_->Release();
                d3d_handle_ = NULL;
            }
        }
    }

    if (!locked_rect_.pBits && d3d_surface_)
    {
        hr = d3d_surface_->LockRect(&locked_rect_, NULL, 0);
        if (FAILED(hr))
        {
            if (!ReconfigureD3D())
            {
                OutputDebugString(L"ReconfigureD3D failed.");
                return;
            }

            hr = d3d_surface_->LockRect(&locked_rect_, NULL, 0);
            if (FAILED(hr))
            {
                OutputDebugString(L"d3d_surface LockRect failed.");
                return;
            }
        }
    }
    else
    {
        if (d3d_surface_ == nullptr)
        {
            OutputDebugString(L"d3d_surface_ == nullptr failed.");
        }
        return;
    }

    libyuv::ARGBCopy(img_data, width * 4, (uint8_t*)locked_rect_.pBits, locked_rect_.Pitch, image_width_, image_height_);

    hr = d3d_surface_->UnlockRect();
    if (FAILED(hr))
    {
        OutputDebugString(L"Surface unlock failed.");
        return;
    }

    locked_rect_.pBits = NULL;

    return;
}

void WebrtcVideoRendererD3D9Impl::RenderOneTime()
{
    HRESULT  hr = E_FAIL;
    hr = d3d_device_->BeginScene();
    if (FAILED(hr))
    {
        //RTC_LOG(LS_INFO) << "d3d_device BeginScene failed." << hr;
        return;
    }

    hr = d3d_device_->StretchRect(d3d_surface_,
                                  nullptr,
                                  d3d_back_buf_,
                                  nullptr,
                                  D3DTEXF_LINEAR);
    if (FAILED(hr))
    {
        //RTC_LOG(LS_INFO) << "d3d_device StretchRect failed." << hr;
        hr = d3d_device_->EndScene();
        return;
    }

    hr = d3d_device_->EndScene();
    if (FAILED(hr))
    {
        //RTC_LOG(LS_INFO) << "d3d_device EndScene failed." << hr;
        return;
    }

    RECT rect = { 0 };
    GetClientRect(wnd_, &rect);

    hr = d3d_device_->Present(NULL, &rect, NULL, NULL);
    if (FAILED(hr))
    {
        //RTC_LOG(LS_INFO) << "d3d_device Present failed." << hr;
        return;
    }
}

void WebrtcVideoRendererD3D9Impl::UpdateWindowRect()
{
    if (!::IsWindowVisible(wnd_))
    {
        return;
    }
    RECT rect = { 0 };
    ::GetWindowRect(wnd_, &rect);
    if (!::ScreenToClient(::GetParent(wnd_), (LPPOINT)&rect))
    {
        return;
    }
    if (::ScreenToClient(::GetParent(wnd_), ((LPPOINT)&rect + 1)))
    {
        ::InvalidateRect(::GetParent(wnd_), &rect, TRUE);
        ::UpdateWindow(::GetParent(wnd_));
    }
}
