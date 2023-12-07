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
}

void Input::Update()
{
	preKeys_ = keys_;
	//キーボード情報の取得開始
	keyboard_->Acquire();
	keys_ = {};
	//全てのキーの入力状態を取得する
	keyboard_->GetDeviceState(sizeof(keys_), &keys_);
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

bool Input::GetJoystickState(XINPUT_STATE& state) {
	DWORD dwResult = XInputGetState(0, &state);
	if (dwResult == ERROR_SUCCESS) {
		return true;
	}
	return false;
}