/**
 * @file IState.h
 * @brief Stateの基底クラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once

class IState
{
public:
	/// <summary>初期化</summary>
	virtual void Initialize() = 0;

	/// <summary>更新</summary>
	virtual void Update() = 0;
};
