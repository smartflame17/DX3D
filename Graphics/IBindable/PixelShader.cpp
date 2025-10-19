#include "PixelShader.h"
#include "../../ErrorHandling/GraphicsExceptionMacros.h"

PixelShader::PixelShader(Graphics& gfx, const std::wstring& path)
{
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_FAILED(D3DReadFileToBlob(path.c_str(), &pBlob));
	GFX_THROW_FAILED(GetDevice(gfx)->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));
}

void PixelShader::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->PSSetShader(pPixelShader.Get(), nullptr, 0u);
}
