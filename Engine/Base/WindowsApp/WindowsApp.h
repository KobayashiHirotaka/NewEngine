#pragma once
#include <Windows.h>
#include <cstdint>
#include "Engine/externals/imgui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

class WindowsApp 
{
public:
	static WindowsApp* GetInstance();

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	void CreateGameWindow(const wchar_t* title, int32_t clientWidth, int32_t clientHeight);

	void CloseGameWindow();

	bool ProcessMessage();

	HWND GetHwnd() const { return hwnd_; };

	HINSTANCE GetHInstance() { return wc_.hInstance; };

	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

private:
	WindowsApp() = default;
	~WindowsApp() = default;
	WindowsApp(const WindowsApp&) = delete;
	const WindowsApp& operator=(const WindowsApp&) = delete;

private:
	WNDCLASS wc_{};
	RECT wrc_{};
	HWND hwnd_{};
};

