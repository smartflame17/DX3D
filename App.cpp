#include "App.h"

float zpos = 0.0f;

App::App():
	wnd (800, 600, "TapiEngine v0.1")
{}

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

	wnd.Gfx().ClearBuffer(0.4f, 0.2f, 1.0f);

	
	// Process all pending mouse events
	while (!wnd.mouse.isEmpty())
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

	wnd.Gfx().DrawTest(timer.Peek(), wnd.mouse.GetPosX() / 400.0f - 1.0f, -wnd.mouse.GetPosY() / 300.0f + 1.0f, zpos);
	wnd.Gfx().Endframe();
}