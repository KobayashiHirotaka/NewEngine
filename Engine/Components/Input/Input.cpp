#include "Input.h"
#include <cassert>

Input* Input::GetInstance()
{
	static Input instance;
	return &instance;
}

void Input::Initialize() 
{
	win_ = WindowsApp::GetInstance();

	HRESULT result;

	//DirectInputの初期化
	result = DirectInput8Create(win_->GetHinstance(), DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput_, nullptr);
	assert(SUCCEEDED(result));

	//キーボードデバイスの生成
	result = directInput_->CreateDevice(GUID_SysKeyboard, &keyboardDevice_, NULL);
	assert(SUCCEEDED(result));

	//マウスデバイスの生成
	result = directInput_->CreateDevice(GUID_SysMouse, &mouseDevice_, NULL);
	assert(SUCCEEDED(result));

	//キーボード入力データ形式のセット
	result = keyboardDevice_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));

	//マウス入力データ形式のセット
	result = mouseDevice_->SetDataFormat(&c_dfDIMouse);
	assert(SUCCEEDED(result));

	//排他制御レベルのセット
	result = keyboardDevice_->SetCooperativeLevel(
		win_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	result = mouseDevice_->SetCooperativeLevel(
		win_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	assert(SUCCEEDED(result));

	//コントローラーデバイスの初期化
	ZeroMemory(&state_, sizeof(XINPUT_STATE));
	ZeroMemory(&preState_, sizeof(XINPUT_STATE));
}

void Input::Update()
{
	std::memcpy(preKey_, key_, 256);
	mousePre_ = mouse_;

	preState_ = state_;

	keyboardDevice_->Acquire();

	mouseDevice_->Acquire();

	keyboardDevice_->GetDeviceState(sizeof(key_), key_);

	mouseDevice_->GetDeviceState(sizeof(DIMOUSESTATE), &mouse_);

	//コントローラーの状態を取得
	DWORD dwResult = XInputGetState(0, &state_);
	if (dwResult == ERROR_SUCCESS) {
		ZeroMemory(&state_, sizeof(XINPUT_STATE));
	}
}

bool Input::IsPushKey(uint8_t keyNum)
{
	if (key_[keyNum] == 0x80) 
	{
		return true;
	}
	return false;
}

bool Input::IsReleaseKey(uint8_t keyNum) 
{
	if (key_[keyNum] == 0x00) 
	{
		return true;
	}
	return false;
}

bool Input::IsPushKeyEnter(uint8_t keyNum)
{
	if (key_[keyNum] == 0x80 && preKey_[keyNum] == 0x00) 
	{
		return true;
	}
	return false;
}

bool Input::IsPushKeyExit(uint8_t keyNum)
{
	if (key_[keyNum] == 0x00 && preKey_[keyNum] == 0x80)
	{
		return true;
	}
	return false;
}

bool Input::IsPressMouse(int32_t mouseNum)
{
	if (mouse_.rgbButtons[mouseNum] == 0x80) 
	{
		return true;
	}
	return false;
}

bool Input::IsReleaseMouse(int32_t mouseNum) 
{
	if (mouse_.rgbButtons[mouseNum] == 0x00)
	{
		return true;
	}
	return false;
}

bool Input::IsPressMouseEnter(int32_t mouseNum) 
{
	if (mouse_.rgbButtons[mouseNum] == 0x80 && mousePre_.rgbButtons[mouseNum] == 0x00)
	{
		return true;
	}
	return false;
}

bool Input::IsPressMouseExit(int32_t mouseNum)
{
	if (mouse_.rgbButtons[mouseNum] == 0x00 && mousePre_.rgbButtons[mouseNum] == 0x80)
	{
		return true;
	}
	return false;
}

int32_t Input::GetWheel() 
{
	return mouse_.lZ;
}

bool Input::GetJoystickState(XINPUT_STATE& state)
{
	DWORD dwResult = XInputGetState(0, &state);
	if (dwResult == ERROR_SUCCESS) {
		return true;
	}
	return false;
}

bool Input::IsPressButtonEnter(WORD button)
{
	if ((state_.Gamepad.wButtons & button) && !(preState_.Gamepad.wButtons & button)) {
		return true;
	}
	return false;
}
