#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Utility/Math/MyMath.h"
#include <d3d12.h>
#include <wrl.h>

struct ConstBufferDataViewProjection
{
	// ワールド → ビュー変換行列
	Matrix4x4 view;

	// ビュー → プロジェクション変換行列
	Matrix4x4 projection;

	// カメラ座標（ワールド座標）
	Vector3 cameraPos;
};

class Camera
{
public:
	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff;

	// マッピング済みアドレス
	ConstBufferDataViewProjection* constMap = nullptr;

	// X,Y,Z軸回りのローカル回転角
	Vector3 rotation = { 0.0f, 0.0f, 0.0f };

	// ローカル座標
	Vector3 translation = { 0.0f, 2.0f, -35.0f };

	// 垂直方向視野角
	float fovAngleY = 0.45f;

	// ビューポートのアスペクト比
	float aspectRatio = (float)16 / 9;

	// 深度限界（手前側）
	float nearZ = 0.1f;

	// 深度限界（奥側）
	float farZ = 1000.0f;

	// ビュー行列
	Matrix4x4 matView;

	// 射影行列
	Matrix4x4 matProjection;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 定数バッファ生成
	/// </summary>
	void CreateConstBuffer();

	/// <summary>
	/// マッピングする
	/// </summary>
	void Map();

	/// <summary>
	/// 行列を更新する
	/// </summary>
	void UpdateMatrix();

	/// <summary>
	/// 行列を転送する
	/// </summary>
	void TransferMatrix();

	/// <summary>
	/// ビュー行列を更新する
	/// </summary>
	void UpdateViewMatrix();

	/// <summary>
	/// 射影行列を更新する
	/// </summary>
	void UpdateProjectionMatrix();
};
