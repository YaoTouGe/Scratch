#include <tchar.h>
#include <windows.h>
#include <windowsx.h>
#include <wingdi.h>
#include <stdint.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>

using namespace DirectX;
using namespace DirectX::PackedVector;

const uint32_t SCREEN_WIDTH = 960;
const uint32_t SCREEN_HEIGHT = 480;

IDXGISwapChain *g_pSwapchain = nullptr;
ID3D11Device *g_pDev = nullptr;
ID3D11DeviceContext *g_pDevcon = nullptr;

ID3D11RenderTargetView *g_pRTView = nullptr;

ID3D11InputLayout *g_pLayout = nullptr;
ID3D11VertexShader *g_pVS = nullptr;
ID3D11PixelShader *g_pPS = nullptr;
ID3D11Buffer *g_pVBuffer = nullptr;

struct VERTEX
{
    XMFLOAT3 Position;
    XMFLOAT4 Color;
};

template <class T>
inline void SafeRelease(T **ppInterfaceToRelease)
{
    if (*ppInterfaceToRelease != nullptr)
    {
        (*ppInterfaceToRelease)->Release();
        (*ppInterfaceToRelease) = nullptr;
    }
}

void CreateRenderTarget()
{
    HRESULT hr;
    ID3D11Texture2D *pBackBuffer;

    g_pSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pBackBuffer);
    g_pDev->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRTView);
    pBackBuffer->Release();

    g_pDevcon->OMSetRenderTargets(1, &g_pRTView, nullptr);
}

void SetViewPort()
{
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = SCREEN_WIDTH;
    viewport.Height = SCREEN_HEIGHT;

    g_pDevcon->RSSetViewports(1, &viewport);
}

void InitPipeline()
{
    ID3DBlob *VS, *PS;
    D3DReadFileToBlob(L"copy.vso", &VS);
    D3DReadFileToBlob(L"copy.pso", &PS);

    g_pDev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), nullptr, &g_pVS);
    g_pDev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), nullptr, &g_pPS);

    g_pDevcon->VSSetShader(g_pVS, 0, 0);
    g_pDevcon->PSSetShader(g_pPS, 0, 0);

    D3D11_INPUT_ELEMENT_DESC ied[] =
        {{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
         {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}};

    g_pDev->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &g_pLayout);
    g_pDevcon->IASetInputLayout(g_pLayout);

    VS->Release();
    PS->Release();
}

void InitGraphics()
{
    VERTEX OurVertices[] =
        {{XMFLOAT3(0.f, 0.5f, 0.f), XMFLOAT4(1.f, 0.f, 0.f, 1.f)},
         {XMFLOAT3(0.45f, -0.5f, 0.f), XMFLOAT4(0.f, 1.f, 0.f, 1.f)},
         {XMFLOAT3(-0.45f, -0.5f, 0.f), XMFLOAT4(0.f, 0.f, 1.f, 1.f)}};

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(VERTEX) * 3;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    g_pDev->CreateBuffer(&bd, nullptr, &g_pVBuffer);

    D3D11_MAPPED_SUBRESOURCE ms;
    g_pDevcon->Map(g_pVBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
    memcpy(ms.pData, OurVertices, sizeof(VERTEX) * 3);
    g_pDevcon->Unmap(g_pVBuffer, NULL);
}

HRESULT CreateGraphicsResources(HWND hWnd)
{
    HRESULT hr = S_OK;
    if (g_pSwapchain == nullptr)
    {
        DXGI_SWAP_CHAIN_DESC scd;

        ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

        scd.BufferCount = 1;
        scd.BufferDesc.Width = SCREEN_WIDTH;
        scd.BufferDesc.Height = SCREEN_HEIGHT;
        scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        scd.BufferDesc.RefreshRate.Numerator = 60;
        scd.BufferDesc.RefreshRate.Denominator = 1;
        scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scd.OutputWindow = hWnd;
        scd.SampleDesc.Count = 4;
        scd.Windowed = TRUE;
        scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        const D3D_FEATURE_LEVEL featureLevels[] =
            {
                D3D_FEATURE_LEVEL_11_1,
                D3D_FEATURE_LEVEL_11_0,
                D3D_FEATURE_LEVEL_10_1,
                D3D_FEATURE_LEVEL_10_0,
                D3D_FEATURE_LEVEL_9_3,
                D3D_FEATURE_LEVEL_9_2,
                D3D_FEATURE_LEVEL_9_1,
            };

        D3D_FEATURE_LEVEL featureLevelsSupported;

        hr = D3D11CreateDeviceAndSwapChain(NULL,
                                           D3D_DRIVER_TYPE_HARDWARE,
                                           NULL,
                                           0,
                                           featureLevels,
                                           _countof(featureLevels),
                                           D3D11_SDK_VERSION,
                                           &scd,
                                           &g_pSwapchain,
                                           &g_pDev,
                                           &featureLevelsSupported,
                                           &g_pDevcon);
        if (hr == E_INVALIDARG)
        {
            hr = D3D11CreateDeviceAndSwapChain(NULL,
                                               D3D_DRIVER_TYPE_HARDWARE,
                                               NULL,
                                               0,
                                               &featureLevelsSupported,
                                               1,
                                               D3D11_SDK_VERSION,
                                               &scd,
                                               &g_pSwapchain,
                                               &g_pDev,
                                               NULL,
                                               &g_pDevcon);
        }

        if (hr == S_OK)
        {
            CreateRenderTarget();
            SetViewPort();
            InitPipeline();
            InitGraphics();
        }
    }
    return hr;
}

void DiscardGraphicsResources()
{
    SafeRelease(&g_pLayout);
    SafeRelease(&g_pVS);
    SafeRelease(&g_pPS);
    SafeRelease(&g_pVBuffer);
    SafeRelease(&g_pSwapchain);
    SafeRelease(&g_pRTView);
    SafeRelease(&g_pDev);
    SafeRelease(&g_pDevcon);
}

void RenderFrame()
{
    const FLOAT clearColor[] = {0.f, 0.2f, 0.4f, 1.f};
    g_pDevcon->ClearRenderTargetView(g_pRTView, clearColor);

    {
        UINT stride = sizeof(VERTEX);
        UINT offset = 0;
        g_pDevcon->IASetVertexBuffers(0, 1, &g_pVBuffer, &stride, &offset);

        g_pDevcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        g_pDevcon->Draw(3, 0);
    }

    g_pSwapchain->Present(0, 0);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam,
                            LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPTSTR lpCmdLine, int nCmdShow)
{
    HWND hWnd;
    WNDCLASSEX wc;

    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = _T("WindowClass1");

    RegisterClassEx(&wc);

    hWnd = CreateWindowEx(0, _T("WindowClass1"), _T("Hello, Engine![Direct 3D]"),
                          WS_OVERLAPPEDWINDOW, 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL,
                          hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);

    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam,
                            LPARAM lParam)
{
    LRESULT result = 0;
    bool wasHandled = false;
    switch (message)
    {
    case WM_CREATE:
        wasHandled = true;
        break;

    case WM_PAINT:
    {
        result = CreateGraphicsResources(hWnd);
        RenderFrame();
        wasHandled = true;
    }
    break;
    case WM_SIZE:
        if (g_pSwapchain != nullptr)
        {
            DiscardGraphicsResources();
        }
        wasHandled = true;
        break;

    case WM_DESTROY:
        DiscardGraphicsResources();
        PostQuitMessage(0);
        wasHandled = true;
        break;

    case WM_DISPLAYCHANGE:
        InvalidateRect(hWnd, nullptr, false);
        wasHandled = true;
        break;
    default:
        break;
    }

    if (!wasHandled)
    {
        result = DefWindowProc(hWnd, message, wParam, lParam);
    }

    return result;
}