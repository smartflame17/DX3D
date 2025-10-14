#pragma once
#include "Window.h"
#include "Tools/Timer.h"

class App
{
public:
	App();

	int Begin();	// handles message pump between windows and the app
private:
	void Update();	// called per frame
private:
	Window wnd;
	Timer timer;
};