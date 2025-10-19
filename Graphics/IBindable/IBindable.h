#pragma once
#include "../Graphics.h"


// Parent abstract class that encapsulates all direct3d-side things. Later, 'game objects' will have a container of IBindables to access any required dx3d operations
class IBindable
{
public:
	virtual void Bind(Graphics& gfx) noexcept = 0;
	virtual ~IBindable() = default;
protected:
	static ID3D11DeviceContext* GetContext(Graphics& gfx) noexcept;
	static ID3D11Device* GetDevice(Graphics& gfx) noexcept;
};