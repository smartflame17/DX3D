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

	wnd.Gfx().ClearBuffer(1.0f, 0.0f, 1.0f);
	wnd.Gfx().Endframe();
}