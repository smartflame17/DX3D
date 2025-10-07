#include "Window.h"
#include <sstream>
#include "resource.h"

// Window Class Stuff
Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept
	:
	hInst(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = static_cast<HICON>(LoadImage(GetInstance(), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));		// load resource image icon with 32x32 size, in bitmap
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = static_cast<HICON>(LoadImage(GetInstance(), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));	// 16x16 small icon
	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(wndClassName, GetInstance());
}

const char* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}

// Window Stuff
Window::Window(int width, int height, const char* name):
	width (width),
	height (height)
{
	// calculate window size based on desired client region size
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)		// specify size of WINDOW from the CLIENT window size (wr)
		throw SFWND_LAST_EXCEPT();
	// create window & get hWnd
	hWnd = CreateWindow(
		WindowClass::GetName(), name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,	// let windows decide initial window position
		nullptr, nullptr, WindowClass::GetInstance(), this						// pass pointer to class instance (to save the custom window class in message callback setup)
	);
	if (hWnd == nullptr)
		throw SFWND_LAST_EXCEPT();
	// show window
	ShowWindow(hWnd, SW_SHOWDEFAULT);
}

Window::~Window() {
	DestroyWindow(hWnd);
}

void Window::SetTitle(const std::string& title)
{
	if (SetWindowText(hWnd, title.c_str()) == 0)
		throw SFWND_LAST_EXCEPT();
}

LRESULT WINAPI Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept		// inital static window procedure handler (defined in CreateWindow), sets up the pointer of each instance
{
	if (msg == WM_NCCREATE) {	// when window is first created
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*> (lParam);		// lParam contains CREATESTRUCTW, which has the pointer to custom window class we set on CreatWindow of constructor
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);					// cast void pointer into window class pointer

		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));			// set winapi-side custom user data to pointer of window class
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));	// set message procedure (callback) to non-setup function since setup is done

		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept		// default static window procedure handler, retrieves info of class instance and let their handler do stuff (basically invokes the member function)
{
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));		// retrieve the instance pointer stored earlier as custom user data
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (msg) {
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;		// only post quit message, as window destruction is handled by destructor

		// ----------- Keyboard Message Handling -----------//
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:		//SYSKEY handling to hande the ALT(VK_MENU) key, thanks bill gates for the intuitive interface
		if (!(lParam & 0x40000000) || kbd.IsAutoRepeatEnabled()) {	// bit 30 of lParam is 1 if previously held down (autorepeat), 
			kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
		}
		break;

	case WM_KEYUP:
	case WM_SYSKEYUP:
		kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;

	case WM_CHAR:		// both standard messages and WM_CHAR is sent when a ASCII-correspondant key is pressed
		kbd.OnChar(static_cast<unsigned char>(wParam));
		break;

	case WM_KILLFOCUS:	// if user un-focuses window, clear the key states
		kbd.ClearState();
		break;

		// ---------- Mouse message Handling ---------- //
	case WM_MOUSEMOVE: {
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnMouseMove(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONDOWN: {
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftPressed(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONDOWN: {
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightPressed(pt.x, pt.y);
		break;
	}
	case WM_MBUTTONDOWN: {
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnMiddlePressed(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONUP: {
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftReleased(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONUP: {
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightReleased(pt.x, pt.y);
		break;
	}
	case WM_MBUTTONUP: {
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnMiddleReleased(pt.x, pt.y);
		break;
	}
	case WM_MOUSEWHEEL: {
		const POINTS pt = MAKEPOINTS(lParam);
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
			mouse.OnWheelUp(pt.x, pt.y);
		else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
			mouse.OnWheelDown(pt.x, pt.y);
		break;
	}
	}
	
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// Exception handling
Window::Exception::Exception(int line, const char* file, HRESULT hr) noexcept:
	SmflmException(line, file),
	hr (hr)
{}

const char* Window::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code]" << GetErrorCode() << std::endl
		<< "[Descripton]" << GetErrorString() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Window::Exception::GetType() const noexcept
{
	return "Smflm Window Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept		// uses windows-provided macro to format error code into readable message
{
	char* pMsgBuf = nullptr;
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr);
	// Windows macro formats error code HRESULT hr into string
	if (nMsgLen == 0) return "Unidentified Error Code";
	std::string errorString = pMsgBuf;
	LocalFree(pMsgBuf);			// save to string object and deallocate pMsgBuf
	return errorString;
}

HRESULT Window::Exception::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::Exception::GetErrorString() const noexcept
{
	return TranslateErrorCode(hr);
}