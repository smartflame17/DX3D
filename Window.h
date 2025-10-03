#pragma once
#include "SmflmWin.h"	// always put wrapper headers first to override windows macros
#include <optional>
#include <string>
#include <memory>

class Window
{
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
	Window(int width, int height, const char* name) noexcept;
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;		// static function so that winapi can register as callback procedure without class pointer
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;							// little hack needed to pass member function to static function

private:
	int width;
	int height;
	HWND hWnd;
};