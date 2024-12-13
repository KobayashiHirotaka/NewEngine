/**
 * @file WindowsApp.h
 * @brief アプリケーションのウィンドウ生成などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include <Windows.h>
#include <cstdint>
#include "Engine/externals/imgui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

class WindowsApp
{
public:
	//ウィンドウサイズ
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

	/// <summary>インスタンスの取得</summary>
	static WindowsApp* GetInstance();

	/// <summary>インスタンスの削除</summary>
	static void DeleteInstance();

	/// <summary>ウィンドウプロシージャ</summary>
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	/// <summary>ゲームウィンドウの生成</summary>
	void CreateGameWindow(const wchar_t* title, int32_t clientWidth, int32_t clientHeight);

	/// <summary>ゲームウィンドウを閉じる</summary>
	void CloseGameWindow();

	/// <summary>メッセージ処理</summary>
	bool ProcessMessage();

	//Getter
	//ウィンドウハンドル
	HWND GetHwnd() const { return hwnd_; };

	//アプリケーションインスタンス
	HINSTANCE GetHinstance() { return wc_.hInstance; };

private:
	//シングルトン
	WindowsApp() = default;
	~WindowsApp() = default;
	WindowsApp(const WindowsApp&) = delete;
	const WindowsApp& operator=(const WindowsApp&) = delete;

private:
	//WindowsAppのインスタンス
	static WindowsApp* sInstance_;

	//ウィンドウクラス
	WNDCLASS wc_{};

	//ウィンドウサイズ
	RECT wrc_{};

	//ウィンドウハンドル
	HWND hwnd_ = nullptr;
};

