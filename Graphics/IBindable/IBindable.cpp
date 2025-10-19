#include "IBindable.h"

ID3D11DeviceContext* IBindable::GetContext(Graphics& gfx) noexcept
{
	return gfx.pContext.Get();
}

ID3D11Device* IBindable::GetDevice(Graphics& gfx) noexcept
{
	return gfx.pDevice.Get();
}
