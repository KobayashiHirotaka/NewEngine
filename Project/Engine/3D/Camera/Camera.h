/**
 * @file Camera.h
 * @brief カメラの更新、行列計算などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Utility/Math/MyMath.h"
#include <d3d12.h>
#include <wrl.h>

struct ConstBuffDataCamera
{
	//ワールド座標
	Vector3 worldPosition;

	//パディング
	float padding;

	//ビュー行列
	Matrix4x4 view;

	//投影行列
	Matrix4x4 projection;
};

class Camera
{
public:
	/// <summary>コンストラクタ</summary>
	Camera();

	/// <summary>デストラクタ</summary>
	~Camera();

	/// <summary>ビュー行列の更新</summary>
	void UpdateViewMatrix();

	/// <summary>プロジェクション行列の更新</summary>
	void UpdateProjectionMatrix();

	/// <summary>行列の更新</summary>
	void UpdateMatrix();

	/// <summary>行列の転送</summary>
	void TransferMatrix();

	/// <summary>ImGui</summary>
	void ImGui();

	//定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;

	//ローカルスケール
	Vector3 scale_ = { 1.0f,1.0f,1.0f };

	//X,Y,Z軸回りのローカル回転角
	Vector3 rotation_ = { 0.0f,0.0f,0.0f };

	//ローカル座標
	Vector3 translation_ = { 0.0f,0.0f,0.0f };

	//視野角
	float fov_ = 0.45f;

	//アスペクト比
	float aspectRatio_ = (float)16 / 9;

	//近平面への距離（手前側）
	float nearClip_ = 0.1f;

	//遠平面への距離（奥側）
	float farClip_ = 300.0f;

	//ビュー行列
	Matrix4x4 matView_{};

	//投影行列
	Matrix4x4 matProjection_{};
};