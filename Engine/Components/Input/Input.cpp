#include "Input.h"
Input* Input::GetInstance()
{
	static Input instance;
	return &instance;
}

void Input::Initialize(WindowsApp* win)
{
	//DirectInputのオブジェクトを作成
	HRESULT hr = DirectInput8Create(win->GetHinstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput_, nullptr);
	assert(SUCCEEDED(hr));
	//キーボードデバイスを生成
	hr = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(hr));
	//入力データ形式のセット
	hr = keyboard_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));
	//排他制御レベルのセット
	hr = keyboard_->SetCooperativeLevel(win->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
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
	//キーボード情報の取得開始
	keyboard_->Acquire();
	keys_ = {};
	//全てのキーの入力状態を取得する
	keyboard_->GetDeviceState(sizeof(keys_), &keys_);

	preState_ = state_;

	DWORD dwResult = XInputGetState(0, &state_);
	if (dwResult == ERROR_SUCCESS)
	{
		ZeroMemory(&state_, sizeof(XINPUT_STATE));
	}
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

bool Input::GetJoystickState() 
{
	DWORD dwResult = XInputGetState(0, &state_);
	if (dwResult == ERROR_SUCCESS)
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
