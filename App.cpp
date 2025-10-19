#include "App.h"

float zpos = 0.0f;

App::App():
	wnd (800, 600, "TapiEngine v0.1")
{
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> adist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> ddist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> odist(0.0f, 3.1415f * 0.3f);
	std::uniform_real_distribution<float> rdist(6.0f, 20.0f);
	for (auto i = 0; i < 80; i++)
	{
		boxes.push_back(std::make_unique<Box>(
			wnd.Gfx(), rng, adist,
			ddist, odist, rdist
		));
	}
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
}

App::~App()
{ }

int App::Begin()
{
	// If ecode has value (some event handling)
	while (true) {
		if (const auto ecode = Window::ProcessMessages())
			return *ecode;

		Update();
	}
}

void App::Update()
{
	// Game logic
	auto dt = timer.Mark();
	wnd.Gfx().ClearBuffer(0.4f, 0.2f, 1.0f);
	for (auto& b : boxes)
	{
		b->Update(dt);
		b->Draw(wnd.Gfx());
	}
	
	// Process all pending mouse events
	/*while (!wnd.mouse.isEmpty())
	{
		const auto e = wnd.mouse.Read();
		switch (e.GetType())
		{
		case Mouse::Event::Type::WheelUp:
			zpos += 1.0f;
			break;
		case Mouse::Event::Type::WheelDown:
			zpos -= 1.0f;
			break;
		}
	}

	wnd.Gfx().DrawTest(timer.Peek(), wnd.mouse.GetPosX() / 400.0f - 1.0f, -wnd.mouse.GetPosY() / 300.0f + 1.0f, zpos);*/
	wnd.Gfx().Endframe();
}