#pragma once
#include "SmflmWin.h"							// always put wrapper headers first to override windows macros
#include "ErrorHandling/SmflmException.h"
#include "Input/Keyboard.h"
#include "Input/Mouse.h"
#include "Graphics/Graphics.h"
#include <optional>
#include <string>
#include <memory>
#include <sstream>

class Window
{
public:
	class HrException : public SmflmException
	{
		using SmflmException::SmflmException;
	public:
		HrException(int line, const char* file, HRESULT hr) noexcept;		// includes windows error code HRESULT
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept;
		static std::string TranslateErrorCode(HRESULT hr) noexcept;		// Translates HRESULT into error string
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
	private:
		HRESULT hr;
	};

	class NoGfxException : public HrException
	{
	public:
		using HrException::HrException;
		const char* GetType() const noexcept override;
	};

private:
	// singleton for registering/cleanup of window class
	class WindowClass
	{
	public:	// noexcept keywords insures no exceptions are thrown, so compiler doesn't add exception handling
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;	// enforce singleton by destroying new instances
		static constexpr const char* wndClassName = "SmartFlame's Dx3D Engine";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};
public:
	Window(int width, int height, const char* name);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	void SetTitle(const std::string& title);
	static std::optional<int> ProcessMessages();	// c++17 feature allows returning int or nullopt if no message
	Graphics& Gfx();
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;		// static function so that winapi can register as callback procedure without class pointer
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;							// little hack needed to pass member function to static function

public:
	Keyboard kbd;	// accessible keyboard object for key input handling
	Mouse mouse;	// mouse object as well
private:
	int width;
	int height;
	HWND hWnd;
	std::unique_ptr<Graphics> pGfx;
};

// error handling exception macro
#define SFWND_EXCEPT(hr) Window::HrException(__LINE__, __FILE__, hr)
#define SFWND_LAST_EXCEPT() Window::HrException(__LINE__, __FILE__, GetLastError())	// for windows function without error code
#define SFWND_NOGFX_EXCEPT() Window::NoGfxException( __LINE__,__FILE__ )