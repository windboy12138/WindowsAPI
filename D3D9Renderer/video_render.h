#pragma once
#include <d3d9.h>
#include <d3d9types.h>
#include <atlbase.h>
#include <codecapi.h>
#include <combaseapi.h>
#include <dxva2api.h>
#include <memory.h>
#include "../common/convert_from_argb.h"

class WebrtcVideoRendererD3D9Impl
{
public:
    WebrtcVideoRendererD3D9Impl(HWND wnd);

    enum D3DBackBufferAction
    {
        D3DBackBuffer_Create,
        D3DBackBuffer_Reset,
    };

    bool InitRender(int w, int h);

    void DestoryRender();

    void FillD3DPresentparams(D3DPRESENT_PARAMETERS* present_params);

    bool ConfigureD3D();

    bool ChangeD3DBackBuffer(D3DBackBufferAction action);

    void DestroyD3DSurfaces();

    bool CreateD3DSurfaces();

    bool ReconfigureD3D();

    void DoRender(uint8_t* img_data, int width, int height);

    void RenderOneTime();

    void UpdateWindowRect();

private:

    LPDIRECT3D9 d3d_handle_ = nullptr;
    LPDIRECT3DDEVICE9 d3d_device_ = nullptr;
    LPDIRECT3DSURFACE9 d3d_surface_ = nullptr;
    LPDIRECT3DSURFACE9 d3d_back_buf_ = nullptr;
    D3DLOCKED_RECT locked_rect_;
    D3DFORMAT desktop_fmt_ = D3DFMT_UNKNOWN;
    D3DFORMAT movie_src_fmt_ = D3DFMT_UNKNOWN;
    D3DPRESENT_PARAMETERS present_params_;
    RECT last_client_rect_;

    int cur_back_buf_width_ = 0;
    int cur_backbuf_height_ = 0;
    int image_width_ = 0;
    int image_height_ = 0;
    bool keep_aspect_ = true;
    double window_aspect_ = 0.0;
    HWND wnd_ = nullptr;
};