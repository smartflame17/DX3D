#pragma once
#include "../SmflmWin.h"
#include <d3d11.h>

// Wrapper class for d3d 11 stuff
class Graphics
{
public:
	Graphics(HWND hWnd);

	// Singleton stuff (no copy or move)
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;

	~Graphics();

	void Endframe();	// flips front-back buffer
	void ClearBuffer(float r, float g, float b) noexcept;

private:
	ID3D11Device* pDevice = nullptr;
	IDXGISwapChain* pSwap = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* pTarget = nullptr;
};