/**
 * @file Input.h
 * @brief 入力の受付(キーボード、マウス、パッド)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#define DIRECTINPUT_VERSION 0x0800 //DirectInputのバージョン指定
#include <dinput.h>
#include <Xinput.h>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"xinput.lib")
#include <array>
#include <wrl.h>
#include "Engine/Base/WindowsApp/WindowsApp.h"

class Input
{
public:
	/// <summary>インスタンスの取得</summary>
	static Input* GetInstance();

	/// <summary>インスタンスの削除</summary>
	static void DeleteInstance();

	/// <summary>初期化</summary>
	void Initialize(WindowsApp* win);

	/// <summary>更新</summary>
	void Update();

	//Key
	/// <summary>Keyを押したとき</summary>
	bool PushKey(uint8_t keyNumber)const;

	/// <summary>Keyを押しているとき</summary>
	bool PressKey(uint8_t keyNumber)const;

	/// <summary>Keyを離したとき</summary>
	bool IsReleseKey(uint8_t keyNumber)const;

	//Mouse
	/// <summary>Mouseを押しているとき</summary>
	bool IsPressMouse(int32_t mouseNum);

	/// <summary>Mouseを離しているとき</summary>
	bool IsReleaseMouse(int32_t mouseNum);

	/// <summary>Mouseを押したとき</summary>
	bool IsPressMouseEnter(int32_t mouseNum);

	/// <summary>Mouseを離したとき</summary>
	bool IsPressMouseExit(int32_t mouseNum);

	/// <summary>ホイールの状態の取得</summary>
	int32_t GetWheel();

	//Joystick
	/// <summary>Joystickの状態の取得</summary>
	bool GetJoystickState();

	/// <summary>Joystickのボタンを押しているとき</summary>
	bool IsPressButton(WORD button);

	/// <summary>Joystickのボタンを押したとき</summary>
	bool IsPressButtonEnter(WORD button);

	/// <summary>Joystickの左スティック(X)の状態の取得</summary>
	float GetLeftStickX();

	/// <summary>Joystickの左スティック(Y)の状態の取得</summary>
	float GetLeftStickY();

	/// <summary>Joystickの右スティック(X)の状態の取得</summary>
	float GetRightStickX();

	/// <summary>Joystickの右スティック(Y)の状態の取得</summary>
	float GetRightStickY();

	/// <summary>Joystickのデッドゾーンの状態の取得</summary>
	float GetDeadZone() { return kDeadZone_; };

private:
	//シングルトン
	Input() = default;
	~Input() = default;
	Input(const Input&) = delete;
	const Input& operator = (const Input&) = delete;

private:
	//Inputのインスタンス
	static Input* sInstance_;

	//DirectInputのインターフェイス
	Microsoft::WRL::ComPtr<IDirectInput8>directInput_ = nullptr;

	//キーボードのインターフェイス
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard_ = nullptr;

	//マウスのインターフェイス
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mouseDevice_ = nullptr;

	//現在のキー入力状態
	std::array<BYTE, 256> keys_;

	//前フレームのキー入力状態
	std::array<BYTE, 256> preKeys_;

	//現在のマウス入力状態
	DIMOUSESTATE mouse_;

	//前フレームのマウス入力状態
	DIMOUSESTATE mousePre_;

	//現在のゲームパッド入力状態
	XINPUT_STATE state_{};

	//前フレームのゲームパッド入力状態
	XINPUT_STATE preState_{};

	//デッドゾーン
	const float kDeadZone_ = 0.7f;
};

