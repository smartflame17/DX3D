#include "Box.h"
#include "../IBindable/IBindableBase.h"
#include "../../ErrorHandling/GraphicsExceptionMacros.h"
#include "Cube.h"
#include "Sphere.h"

Box::Box(Graphics& gfx,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_real_distribution<float>& bdist)
	:
	r(rdist(rng)),
	droll(ddist(rng)),
	dpitch(ddist(rng)),
	dyaw(ddist(rng)),
	dphi(odist(rng)),
	dtheta(odist(rng)),
	dchi(odist(rng)),
	chi(adist(rng)),
	theta(adist(rng)),
	phi(adist(rng))
{
	namespace dx = DirectX;
	if (!IsStaticInitialized())	// we initialize static binds for the same type of object only once
	{
		struct Vertex
		{
			dx::XMFLOAT3 pos;
		};
		auto model = Cube::Make<Vertex>();
		model.Transform(dx::XMMatrixScaling(1.0f, 1.0f, 3.0f));
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices));		// add vertex data

		// add shader data
		auto pvs = std::make_unique<VertexShader>(gfx, L"VertexShader.cso");
		auto pvsbc = pvs->GetBytecode();	// get vertex shader bytecode for input layout later
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PixelShader.cso"));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));	// add index buffer

		struct PixelShaderConstants
		{
			struct
			{
				float r;
				float g;
				float b;
				float a;
			} face_colors[8];
		};
		const PixelShaderConstants cb2 =
		{
			{
				{ 1.0f,1.0f,1.0f,1.0f },
				{ 1.0f,0.0f,0.0f,1.0f },
				{ 0.0f,1.0f,0.0f,1.0f },
				{ 1.0f,1.0f,0.0f,1.0f },
				{ 0.0f,0.0f,1.0f,1.0f },
				{ 1.0f,0.0f,1.0f,1.0f },
				{ 0.0f,1.0f,1.0f,1.0f },
				{ 0.0f,0.0f,0.0f,1.0f },
			}
		};
		AddStaticBind(std::make_unique<PixelConstantBuffer<PixelShaderConstants>>(gfx, cb2));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));	// bind input layout with vertex shader info

		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else {
		SetIndexFromStatic();	// If this object doesn't have to do static binding (not the first new object), get Index Buffer pointer
	}

	AddBind(std::make_unique<TransformCbuf>(gfx, *this));	// the transformation constant buffer is non-static (different per object)
}

void Box::Update(float dt) noexcept	// update angle per deltatime
{
	roll += droll * dt;
	pitch += dpitch * dt;
	yaw += dyaw * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}

DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *				
		DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi) *	// rotate around world origin
		DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f);			// move it away from camera (set at origin)
}
