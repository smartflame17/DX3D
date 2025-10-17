#pragma once
#include "../SmflmWin.h"
#include "../ErrorHandling/SmflmException.h"
#include <wrl.h>
#include <DirectXMath.h>
#include <d3d11.h>

// Wrapper class for d3d 11 stuff
class Graphics
{
public:
	class HrException : public SmflmException
	{
	public:
		HrException(int line, const char* file, HRESULT hr) noexcept;		// includes windows error code HRESULT
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept;
		static std::string TranslateErrorCode(HRESULT hr) noexcept;		// Translates HRESULT into error string
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT hr;
	};
	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	};

	Graphics(HWND hWnd);

	// Singleton stuff (no copy or move)
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;

	~Graphics() = default;

	void Endframe();	// flips front-back buffer
	void ClearBuffer(float r, float g, float b) noexcept;
	void DrawTest(float angle, float x, float y, float z);

private:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV = nullptr;
};