#include "App.h"

App::App():
	wnd (800, 600, "I like milk tea")
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
	wnd.Gfx().DrawTestTriangle(timer.Peek(), wnd.mouse.GetPosX() / 400.0f - 1.0f, -wnd.mouse.GetPosY() / 300.0f + 1.0f);
	wnd.Gfx().Endframe();
}