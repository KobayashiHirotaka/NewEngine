#include "Input.h"

Input* Input::instance_ = nullptr;

Input* Input::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new Input();
	}
	return instance_;
}

void Input::DeleteInstance()
{
	if (instance_ != nullptr)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

void Input::Initialize(WindowsApp* win)
{
	//DirectInputのオブジェクトを作成
	HRESULT hr = DirectInput8Create(win->GetHinstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput_, nullptr);
	assert(SUCCEEDED(hr));
	//キーボードデバイスを生成
	hr = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(hr));

	//マウスデバイスを生成
	hr = directInput_->CreateDevice(GUID_SysMouse, &mouseDevice_, NULL);
	assert(SUCCEEDED(hr));

	//入力データ形式のセット
	hr = keyboard_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));

	hr = mouseDevice_->SetDataFormat(&c_dfDIMouse);
	assert(SUCCEEDED(hr));

	//排他制御レベルのセット
	hr = keyboard_->SetCooperativeLevel(win->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));

	hr = mouseDevice_->SetCooperativeLevel(win->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	assert(SUCCEEDED(hr));

	keys_ = {};
	preKeys_ = {};

	//コントローラーデバイスの初期化
	ZeroMemory(&state_, sizeof(XINPUT_STATE));
	ZeroMemory(&preState_, sizeof(XINPUT_STATE));
}

void Input::Update()
{
	preKeys_ = keys_;

	mousePre_ = mouse_;

	//キーボード情報の取得開始
	keyboard_->Acquire();

	//マウス情報の取得開始
	mouseDevice_->Acquire();

	keys_ = {};
	//全てのキーの入力状態を取得する
	keyboard_->GetDeviceState(sizeof(keys_), &keys_);

	//マウスの入力状態を取得する
	mouseDevice_->GetDeviceState(sizeof(DIMOUSESTATE), &mouse_);

	preState_ = state_;
}

bool Input::PushKey(uint8_t keyNumber)const
{
	if (!keys_[keyNumber] && preKeys_[keyNumber])
	{
		return true;

	}
	else {
		return false;
	}
}

bool Input::PressKey(uint8_t keyNumber)const
{
	if (keys_[keyNumber])
	{
		return true;

	}
	else {
		return false;
	}
}

bool Input::IsReleseKey(uint8_t keyNumber)const
{
	if (keys_[keyNumber] && !preKeys_[keyNumber])
	{
		return true;

	}
	else {
		return false;
	}
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

bool Input::GetJoystickState()
{
	DWORD dwResult = XInputGetState(0, &state_);
	if (dwResult == ERROR_SUCCESS)
	{
		return true;
	}
	return false;
}

bool Input::IsPressButton(WORD button)
{
	if (state_.Gamepad.wButtons & button)
	{
		return true;
	}
	return false;
}

bool Input::IsPressButtonEnter(WORD button)
{
	if ((state_.Gamepad.wButtons & button) && !(preState_.Gamepad.wButtons & button))
	{
		return true;
	}
	return false;
}

float Input::GetLeftStickX()
{
	float leftStickXValue = static_cast<float>(state_.Gamepad.sThumbLX) / SHRT_MAX;
	return leftStickXValue;
}

float Input::GetLeftStickY()
{
	float leftStickYValue = static_cast<float>(state_.Gamepad.sThumbLY) / SHRT_MAX;
	return leftStickYValue;
}

float Input::GetRightStickX()
{
	float rightStickXValue = static_cast<float>(state_.Gamepad.sThumbRX) / SHRT_MAX;
	return rightStickXValue;
}

float Input::GetRightStickY()
{
	float rightStickYValue = static_cast<float>(state_.Gamepad.sThumbRY) / SHRT_MAX;
	return rightStickYValue;
}
