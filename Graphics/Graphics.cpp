#include "Graphics.h"
#include <iostream>
#pragma comment(lib,"d3d11.lib")		// tell linker about d3d library

Graphics::Graphics(HWND hWnd)
{
	// Descriptor object to configure init function
	DXGI_SWAP_CHAIN_DESC sd = {};
	// Figure out dimension based on window size
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;

	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	
	// No scaling required (fits window)
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	// Anti-alising (disabled for now)
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;

	// Use this buffer for rendering
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;					// 1 back buffer (Double buffering)
	sd.OutputWindow = hWnd;				// handle to our window
	sd.Windowed = true;

	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	// init rendering stuff (device, context, swap chain)
	HRESULT debug;

	debug = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,		// use hardware driver
		nullptr,						// handle for software driver (we don't have any)
		0,								// flags
		nullptr,						// input feature level
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,
		&pDevice,
		nullptr,						// output feature level
		&pContext
	);
	std::cout << "[DEBUG] " << debug;

	// get access to texture (back buffer) in swap chain
	ID3D11Resource* pBackBuffer = nullptr;
	pSwap->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer));
	pDevice->CreateRenderTargetView(
		pBackBuffer,
		nullptr,
		&pTarget
	);
	pBackBuffer->Release();		// discard handle
}


Graphics::~Graphics()
{
	if (pTarget != nullptr)
		pTarget->Release();

	if (pContext != nullptr)
		pContext->Release();

	if (pSwap != nullptr)
		pSwap->Release();

	if (pDevice != nullptr)
		pDevice->Release();
}

void Graphics::Endframe()
{
	pSwap->Present(1u, 0u);		// Sync to device refresh rate, no flag
}

void Graphics::ClearBuffer(float r, float g, float b) noexcept
{
	const float color[] = { r, g, b, 1.0f };
	pContext->ClearRenderTargetView(pTarget, color);
}