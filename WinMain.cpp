#include "Window.h"		// wrapper for winapi

int CALLBACK WinMain(			// CALLBACK is stdcall convention used by windows
	HINSTANCE	hInstance,		// handle to current instance
	HINSTANCE	hPrevInstance,	// nullptr
	LPSTR		lpCmdLine,		// string pointer to cmd line arguments (non-parsed)
	int			nCmdShow)		// window property on startup
{
	Window wnd(800, 300, "I like milk tea");	// instantiating by calling wrapper class

	// message pump
	MSG msg;
	BOOL gResult;
	while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0) {	// receive all message in queue, for all windows, and save info on msg
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (gResult == -1) {	// if final result for message was error
		return -1;
	}
	return msg.wParam;	// returns wParam when exitting
}