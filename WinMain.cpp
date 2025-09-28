#include <Windows.h>
#include <string>
#include <sstream>

LRESULT CALLBACK MyWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {		// custom windows procedure	(handle, message code, parameters (depends per message type))

	switch (msg) {
	case WM_CLOSE:
		PostQuitMessage(17);	// posts quit message with return value to queue
		break;
	case WM_KEYDOWN:
		if (wParam == 'F') {
			SetWindowText(hWnd, "Press F");
		}
		break;

	case WM_KEYUP:
		if (wParam == 'F') {
			SetWindowText(hWnd, "F up");
		}
		break;

	case WM_CHAR: {			// when keys corresponding to text are pressed
		static std::string title;
		title.push_back((char)wParam);
		SetWindowText(hWnd, title.c_str());
	}
		break;

	case WM_LBUTTONDOWN: 
		const POINTS pt = MAKEPOINTS(lParam);		// bit manipulate lparam to extract xy-coords
		std::ostringstream coords;
		coords << "(" << pt.x << ", " << pt.y << ")";
		SetWindowText(hWnd, coords.str().c_str());
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);	// return default behaviour otherwise
}

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
	wc.lpfnWndProc = MyWndProc;			// custom behaviour procedure
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
	else return msg.wParam;	// returns wParam when exitting
}