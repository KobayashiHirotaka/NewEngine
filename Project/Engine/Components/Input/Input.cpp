/**
 * @file Input.cpp
 * @brief 入力の受付(キーボード、マウス、パッド)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "Input.h"

Input* Input::sInstance_ = nullptr;

Input* Input::GetInstance()
{
	//インスタンスを生成
	if (sInstance_ == nullptr)
	{
		sInstance_ = new Input();
	}
	return sInstance_;
}

void Input::DeleteInstance()
{
	//インスタンスを削除
	if (sInstance_ != nullptr)
	{
		delete sInstance_;
		sInstance_ = nullptr;
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

	//キーボードの初期化
	keys_ = {};
	preKeys_ = {};

	//コントローラーデバイスの初期化
	ZeroMemory(&state_, sizeof(XINPUT_STATE));
	ZeroMemory(&preState_, sizeof(XINPUT_STATE));
}

void Input::Update()
{
	//前フレームの入力状態を保存
	preKeys_ = keys_;
	mousePre_ = mouse_;	
	preState_ = state_;

	//キーボード情報の取得開始
	keyboard_->Acquire();

	//マウス情報の取得開始
	mouseDevice_->Acquire();

	//全てのキーの入力状態を取得する
	keys_ = {};
	keyboard_->GetDeviceState(sizeof(keys_), &keys_);

	//マウスの入力状態を取得する
	mouseDevice_->GetDeviceState(sizeof(DIMOUSESTATE), &mouse_);
}

bool Input::PushKey(uint8_t keyNumber)const
{
	//前フレームで押されて、現在は離されている
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
	//現在押されている
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
	//現在押された瞬間
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
	//マウスボタンが押されている
	if (mouse_.rgbButtons[mouseNum] == 0x80)
	{
		return true;
	}
	return false;
}

bool Input::IsReleaseMouse(int32_t mouseNum)
{
	//マウスボタンが離されている
	if (mouse_.rgbButtons[mouseNum] == 0x00)
	{
		return true;
	}
	return false;
}

bool Input::IsPressMouseEnter(int32_t mouseNum)
{
	//マウスボタンが押された瞬間
	if (mouse_.rgbButtons[mouseNum] == 0x80 && mousePre_.rgbButtons[mouseNum] == 0x00)
	{
		return true;
	}
	return false;
}


bool Input::IsPressMouseExit(int32_t mouseNum)
{
	//マウスボタンが離された瞬間
	if (mouse_.rgbButtons[mouseNum] == 0x00 && mousePre_.rgbButtons[mouseNum] == 0x80)
	{
		return true;
	}
	return false;
}

int32_t Input::GetWheel()
{
	//マウスホイールの回転値を返す
	return mouse_.lZ;
}

bool Input::GetJoystickState()
{
	//ゲームパッドの状態を取得
	DWORD dwResult = XInputGetState(0, &state_);
	if (dwResult == ERROR_SUCCESS)
	{
		return true;
	}
	return false;
}

bool Input::IsPressButton(WORD button)
{
	//指定されたボタンが押されている
	if (state_.Gamepad.wButtons & button)
	{
		return true;
	}
	return false;
}

bool Input::IsPressButtonEnter(WORD button)
{
	//指定されたボタンが押された瞬間
	if ((state_.Gamepad.wButtons & button) && !(preState_.Gamepad.wButtons & button))
	{
		return true;
	}
	return false;
}

float Input::GetLeftStickX()
{
	//左スティックのX軸値を返す
	float leftStickXValue = static_cast<float>(state_.Gamepad.sThumbLX) / SHRT_MAX;
	return leftStickXValue;
}

float Input::GetLeftStickY()
{
	//左スティックのY軸値を返す
	float leftStickYValue = static_cast<float>(state_.Gamepad.sThumbLY) / SHRT_MAX;
	return leftStickYValue;
}

float Input::GetRightStickX()
{
	//右スティックのX軸値を返す
	float rightStickXValue = static_cast<float>(state_.Gamepad.sThumbRX) / SHRT_MAX;
	return rightStickXValue;
}

float Input::GetRightStickY()
{
	//右スティックのY軸値を返す
	float rightStickYValue = static_cast<float>(state_.Gamepad.sThumbRY) / SHRT_MAX;
	return rightStickYValue;
}
