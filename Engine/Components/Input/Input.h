#pragma once
#include "Engine/Base/WindowsApp/WindowsApp.h"
#include <memory>
#include <wrl.h>
#define DIRECTINPUT_VERSION 0x0800//DirectInputのバージョン指定
#include <dinput.h>
#include <Xinput.h>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"xinput.lib")

class Input
{
public:
	static Input* GetInstance();

	void Initialize();

	void Update();

	bool IsPushKey(uint8_t keyNum);

	bool IsReleaseKey(uint8_t keyNum);

	bool IsPushKeyEnter(uint8_t keyNum);

	bool IsPushKeyExit(uint8_t keyNum);

	bool IsPressMouse(int32_t mouseNum);

	bool IsReleaseMouse(int32_t mouseNum);

	bool IsPressMouseEnter(int32_t mouseNum);

	bool IsPressMouseExit(int32_t mouseNum);

	int32_t GetWheel();

	bool GetJoystickState(XINPUT_STATE& state);

	bool IsPressButtonEnter(WORD button);

private:
	WindowsApp* win_ = nullptr;

	//DirectInput
	Microsoft::WRL::ComPtr<IDirectInput8> directInput_ = nullptr;

	//Keyboard
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboardDevice_ = nullptr;

	//Mouse
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mouseDevice_ = nullptr;

	BYTE key_[256] = {};
	BYTE preKey_[256] = {};

	DIMOUSESTATE mouse_ = {};

	DIMOUSESTATE mousePre_ = {};

	XINPUT_STATE state_{};
	
	XINPUT_STATE preState_{};
};

