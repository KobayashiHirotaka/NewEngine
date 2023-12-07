#include "WindowsApp.h"
#pragma comment(lib,"winmm.lib")

WindowsApp* WindowsApp::GetInstance()
{
	static WindowsApp instance;
	return &instance;
}

//ウィンドウプロシージャ
LRESULT WindowsApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
	{
		return true;
	}

	//メッセージに応じてゲーム固有の処理を行う
	switch (msg) 
	{
		//ウィンドウが破棄された
	case WM_DESTROY:
		// OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	// 標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void WindowsApp::Initialize()
{
	//システムタイマーの分解能をあげる
	timeBeginPeriod(1);

	//ウィンドウプロシージャ
	wc_.lpfnWndProc = WindowProc;
	//クラス名
	wc_.lpszClassName = L"CG2WindowClass";
	//インスタンスハンドル
	wc_.hInstance = GetModuleHandle(nullptr);
	//カーソル
	wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);

	//ウィンドウクラス登録
	RegisterClass(&wc_);

	//ウィンドウサイズの構造体にクライアント領域を入れる
	RECT wrc = { 0,0,kClientWidth,kClientHeight };

	//クライアント領域を元に実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//ウィンドウの生成
	hwnd_ = CreateWindow(
		wc_.lpszClassName,//クラス名
		L"CG2",//タイトルバーの名前
		WS_OVERLAPPEDWINDOW,//ウィンドウスタイル
		CW_USEDEFAULT,//表示X座標
		CW_USEDEFAULT,//表示Y座標
		wrc.right - wrc.left,//ウィンドウ横幅
		wrc.bottom - wrc.top,//ウィンドウ縦幅
		nullptr,//親ウィンドウハンドル
		nullptr,//メニューハンドル
		wc_.hInstance,//インスタンスハンドル
		nullptr//オプション
	);

	//ウィンドウ表示
	ShowWindow(hwnd_, SW_SHOW);
}
