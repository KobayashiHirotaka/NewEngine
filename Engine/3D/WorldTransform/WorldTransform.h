#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Utility/Math/MyMath.h"

struct WorldTransformData
{
	// ローカル → ワールド変換行列
	Matrix4x4 matWorld;
};

class WorldTransform
{
public:
	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff;

	// マッピング済みアドレス
	WorldTransformData* constMap = nullptr;

	// ローカルスケール
	Vector3 scale = { 1, 1, 1 };

	// X,Y,Z軸回りのローカル回転角
	Vector3 rotation = { 0, 0, 0 };

	// ローカル座標
	Vector3 translation = { 0, 0, 0 };

	// ローカル → ワールド変換行列
	Matrix4x4 matWorld;

	// 親となるワールド変換へのポインタ
	const WorldTransform* parent_ = nullptr;

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
	/// 行列を転送する
	/// </summary>
	void TransferMatrix();

	void UpdateMatrix();

	void SetParent(const WorldTransform* parent);

	void DeleteParent();
};
