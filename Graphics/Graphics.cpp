#include "Graphics.h"
#include "../ErrorHandling/dxerr.h"
#include <sstream>
#include <d3dcompiler.h>

#pragma comment(lib,"d3d11.lib")		// tell linker about d3d library
#pragma comment(lib, "D3DCompiler.lib")	// for compiling hlsl shader at runtime

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

#define GFX_THROW_FAILED(hrcall) if (FAILED(hr = (hrcall))) throw Graphics::HrException(__LINE__, __FILE__, hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr))

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

	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;		// double buffering  (To use DXGI_SWAP_EFFECT_FLIP_DISCARD, increment Buffercount to 2 or above)
	sd.Flags = 0;

	UINT isDebugModeFlag = 0u;
#ifndef NDEBUG
	isDebugModeFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// init rendering stuff (device, context, swap chain)
	HRESULT hr;

	GFX_THROW_FAILED( D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,		// use hardware driver
		nullptr,						// handle for software driver (we don't have any)
		isDebugModeFlag,				// flags
		nullptr,						// input feature level
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,
		&pDevice,
		nullptr,						// output feature level
		&pContext
	));

	// get access to texture (back buffer) in swap chain
	wrl::ComPtr<ID3D11Resource> pBackBuffer = nullptr;
	GFX_THROW_FAILED(pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer));
	GFX_THROW_FAILED(pDevice->CreateRenderTargetView(
		pBackBuffer.Get(),
		nullptr,
		&pTarget
	));
}

void Graphics::Endframe()
{
	HRESULT hr;
	if (FAILED(hr = pSwap->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		else GFX_THROW_FAILED(hr);
	}
}

void Graphics::ClearBuffer(float r, float g, float b) noexcept
{
	const float color[] = { r, g, b, 1.0f };
	pContext->ClearRenderTargetView(pTarget.Get(), color);	// clear back buffer with specified color
}

void Graphics::DrawTestTriangle(float angle, float x, float y)
{
	HRESULT hr;

	struct Vertex
	{
		struct {
			float x;
			float y;
		} pos;
		struct {
			unsigned char r;
			unsigned char g;
			unsigned char b;
			unsigned char a;
		} color;
	};
	const Vertex vertices[] =
	{
		{0.0f, 0.5f, 255, 0, 0, 1},
		{0.5f, -0.5f, 0, 255, 0, 1},
		{-0.5f, -0.5f, 0, 0, 255, 1}
	};
	const unsigned short indices[] =
	{
		0,1,2
	};
	/////////////// Vertex Buffer (Actual data) ///////////////

	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeof(vertices);
	bd.StructureByteStride = sizeof(Vertex);

	// create vertex buffer (basically the actual data of the vertices)
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices;
	GFX_THROW_FAILED(pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));

	// bind vertex buffer to pipeline
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);

	/////////////// Index Buffer (Index data) ///////////////
	wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = sizeof(indices);
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices;
	GFX_THROW_FAILED(pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer));

	// bind index buffer to pipeline
	pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);


	/////////////// Constant Buffer ///////////////

	// create constant buffer for transformation matrix to pass to shader (uniform in openGL)
	struct ConstantBuffer
	{
		dx::XMMATRIX transform;		// optimized for SIMD, don't access directly!!
	};

	const ConstantBuffer cb =
	{
		{
			dx::XMMatrixTranspose(
			dx::XMMatrixRotationZ(angle) *
			dx::XMMatrixScaling(0.5f, 0.5f, 1.0f) *
			dx::XMMatrixTranslation(x, y, 0.0f))
		}
	};
	wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
	D3D11_BUFFER_DESC cbd = {};
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;				// Dynamic so we can update it each frame
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPU needs to write (update) the value
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(cb);
	cbd.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = &cb;
	GFX_THROW_FAILED(pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer));

	// bind constant buffer to pipeline (shader)
	pContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());

	/////////////// Pixel Shader ///////////////
	
	// read in compiled pixel shader and create shader object
	wrl::ComPtr<ID3D11PixelShader> pPixelShader;
	wrl::ComPtr<ID3DBlob> pBlob;					// just a binary file (bytecode by hlsl compiler)
	GFX_THROW_FAILED(D3DReadFileToBlob(L"PixelShader.cso", &pBlob));		// read compiled shader binary and save to Blob
	GFX_THROW_FAILED(pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));

	// bind pixel shader
	pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);


	
	/////////////// Vertex Shader ///////////////

	// do the same with vertex shader
	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	GFX_THROW_FAILED(D3DReadFileToBlob(L"VertexShader.cso", &pBlob));		// read compiled shader binary and save to Blob
	GFX_THROW_FAILED(pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));

	// bind vertex shader
	pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);



	/////////////// Input Layout (Layout info) ///////////////

	// input (vertex) layout (basically the layout of each vertex in the buffer we sent earlier)
	wrl::ComPtr<ID3D11InputLayout> pInputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[] =				// Name of each element has to match name on the argument of shader (except SV_* values that are pre-defined)
	{
		{ "Position", 0, DXGI_FORMAT_R32G32_FLOAT,		// 2 32-bit floats (we are using 2d vertex (x, y)
		0,
		D3D11_APPEND_ALIGNED_ELEMENT,			// offset from beginning of vertex object
		D3D11_INPUT_PER_VERTEX_DATA,
		0 },

		// color mapping
		{
		"Color", 0, DXGI_FORMAT_R8G8B8A8_UNORM,		// RGBA color normalized to float
		0,
		D3D11_APPEND_ALIGNED_ELEMENT,			// offset from beginning of vertex object
		D3D11_INPUT_PER_VERTEX_DATA,
		0 }
	};
	pDevice->CreateInputLayout(ied, (UINT)std::size(ied), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout);	// the function requires the pointer to the binary shader and the size of it

	// bind vertex layout
	pContext->IASetInputLayout(pInputLayout.Get());

	///////////////////////////////////////////////////////////

	// bind render target (back buffer)
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);		// no z-buffer yet so nullptr

	// set primitive topology (how to intepret vertices) -> we set it to list of triangles (group by 3)
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// configure viewport
	D3D11_VIEWPORT vp;
	// viewport dimensions (full screen in this case)
	vp.Width = 800;
	vp.Height = 600;
	// for mapping depth value
	vp.MinDepth = 0;	
	vp.MaxDepth = 1;
	// coordinate of viewport inside window
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports(1u, &vp);

	pContext->DrawIndexed(UINT(std::size(indices)), 0u, 0u);
}

// Exception handling
Graphics::HrException::HrException(int line, const char* file, HRESULT hr) noexcept :
	SmflmException(line, file),
	hr(hr)
{
}

const char* Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code]" << GetErrorCode() << std::endl
		<< "[Descripton]" << GetErrorString() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept
{
	return "Smflm Graphics Exception";
}

std::string Graphics::HrException::TranslateErrorCode(HRESULT hr) noexcept		// uses windows-provided macro to format error code into readable message
{
	char* pMsgBuf = nullptr;
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr);
	// Windows macro formats error code HRESULT hr into string
	if (nMsgLen == 0) return "Unidentified Error Code";
	std::string errorString = pMsgBuf;
	LocalFree(pMsgBuf);			// save to string object and deallocate pMsgBuf
	return errorString;
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
	return TranslateErrorCode(hr);
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	char buf[512];
	DXGetErrorDescription(hr, buf, sizeof(buf));
	return buf;
}

const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}