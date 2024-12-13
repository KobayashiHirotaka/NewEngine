/**
 * @file WorldTransform.h
 * @brief オブジェクトのワールド座標の更新やペアレントなどを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Utility/Math/MyMath.h"

struct ConstBufferDataWorldTransform
{
	//ワールド行列
	Matrix4x4 matWorld;

	//ワールド行列の逆行列転置
	Matrix4x4 worldInverseTranspose;
};

class WorldTransform
{
public:
	/// <summary>初期化</summary>
	void Initialize();

	/// <summary>定数バッファの作成</summary>
	void CreateConstBuffer();

	/// <summary>定数バッファをマップ</summary>
	void Map();

	/// <summary>行列転送の準備</summary>
	void TransferMatrix();

	/// <summary>オイラー角を使って行列を更新</summary>
	void UpdateMatrixEuler();

	/// <summary>クォータニオンを使って行列を更新</summary>
	void UpdateMatrixQuaternion();

	/// <summary>親を設定</summary>
	void SetParent(const WorldTransform* parent);

	/// <summary>親を削除</summary>
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

	Quaternion quaternion = { 0, 0, 0, 1 };

	//ローカル → ワールド変換行列
	Matrix4x4 matWorld = {};

	//親となるワールド変換へのポインタ
	const WorldTransform* parent = nullptr;
};
