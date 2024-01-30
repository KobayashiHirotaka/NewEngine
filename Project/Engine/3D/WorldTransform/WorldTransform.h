#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Utility/Math/MyMath.h"

struct ConstBufferDataWorldTransform
{
	Matrix4x4 matWorld;
	Matrix4x4 worldInverseTranspose;
};

class WorldTransform
{
public:
	void Initialize();

	void CreateConstBuffer();

	void Map();

	void TransferMatrix();

	void UpdateMatrix();

	void SetParent(const WorldTransform* parent);

	void DeleteParent();

	//定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff;

	//マッピング済みアドレス
	ConstBufferDataWorldTransform* constMap = nullptr;

	//ローカルスケール
	Vector3 scale = { 1, 1, 1 };

	//X,Y,Z軸回りのローカル回転角
	Vector3 rotation = { 0, 0, 0 };

	//ローカル座標
	Vector3 translation = { 0, 0, 0 };

	//ローカル → ワールド変換行列
	Matrix4x4 matWorld;

	//親となるワールド変換へのポインタ
	const WorldTransform* parent_ = nullptr;
};
