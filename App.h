#pragma once
#include "Window.h"
#include "Tools/Timer.h"
#include "Graphics/Drawable/Box.h"

class App
{
public:
	App();
	~App();
	int Begin();	// handles message pump between windows and the app
private:
	void Update();	// called per frame
private:
	Window wnd;
	Timer timer;
	std::vector<std::unique_ptr<class Box>> boxes;
};