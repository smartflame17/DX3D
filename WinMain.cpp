#include <Windows.h>

int CALLBACK WinMain(			// CALLBACK is stdcall convention used by windows
	HINSTANCE	hInstance,		// handle to current instance
	HINSTANCE	hPrevInstance,	// nullptr
	LPSTR		lpCmdLine,		// string pointer to cmd line arguments (non-parsed)
	int			nCmdShow)		// window property on startup
{
	const auto pClassName = "My 3D Engine";

	// register windows config class
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;				// allow multiple windows for app
	wc.lpfnWndProc = DefWindowProc;		// default behaviour
	wc.cbClsExtra = 0;					// extra bytes for class structure (not needed now)
	wc.cbWndExtra = 0;					// extra windows for app (not needed now)
	wc.hInstance = hInstance;			// handle to instance
	wc.hIcon = nullptr;					// icon for app (not needed)
	wc.hIconSm = nullptr;				// icon
	wc.hCursor = nullptr;				// cursor style (not needed)
	wc.hbrBackground = nullptr;			// background style (not needed)
	wc.lpszMenuName = nullptr;			// menu (not needed)
	wc.lpszClassName = pClassName;		// name of class
	RegisterClassEx( &wc );

	// create window instance
	HWND hWnd = CreateWindowEx(
		0, pClassName,
		"First Window",
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,	// window shows window name, has minimize button, has system menu
		200, 200, 640, 480,
		nullptr,									// no parent
		nullptr,									// no menu
		hInstance,									// handle to instance
		nullptr										// ptr to custom structure (none for now)
	);

	ShowWindow(hWnd, SW_SHOW);		// show window
	return 0;
}