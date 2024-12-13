/**
 * @file WorldTransform.cpp
 * @brief オブジェクトのワールド座標の更新やペアレントなどを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "WorldTransform.h"

void WorldTransform::Initialize()
{
	//単位行列
	matWorld = MakeIdentity4x4();

	//定数バッファの作成
	CreateConstBuffer();

	//定数バッファのマッピング
	Map();

	//行列転送
	TransferMatrix();
}

void WorldTransform::CreateConstBuffer()
{
	//定数バッファを作成
	constBuff = DirectXCore::GetInstance()->CreateBufferResource(sizeof(ConstBufferDataWorldTransform));
}

void WorldTransform::Map()
{
	//定数バッファをマップ
	constBuff.Get()->Map(0, nullptr, reinterpret_cast<void**>(&constMap));
}

void WorldTransform::TransferMatrix()
{
	//データを転送
	constMap->matWorld = matWorld;
	constMap->worldInverseTranspose = Transpose(Inverse(matWorld));
}

void WorldTransform::UpdateMatrixEuler()
{
	//スケール、回転、平行移動で行列を更新
	Matrix4x4 AffineMatrix = MakeAffineMatrix(scale, rotation, translation);
	matWorld = AffineMatrix;

	//親がいる場合
	if (parent)
	{
		matWorld = Multiply(matWorld, parent->matWorld);
	}

	//行列の転送
	TransferMatrix();
}

void WorldTransform::UpdateMatrixQuaternion()
{
	//スケール、クォータニオン、平行移動で行列を更新
	Matrix4x4 AffineMatrix = MakeAffineMatrix(scale, quaternion, translation);
	matWorld = AffineMatrix;

	//親がいる場合
	if (parent)
	{
		matWorld = Multiply(matWorld, parent->matWorld);
	}

	//行列の転送
	TransferMatrix();
}

void WorldTransform::SetParent(const WorldTransform* newParent)
{
	parent = newParent;

	//親が設定された場合
	if (parent)
	{
		//親の位置を引く
		translation = Subtract(translation, parent->translation);
	}
}

void WorldTransform::DeleteParent()
{
	//親がいる場合
	if (parent)
	{
		//親の位置に基づいて設定
		translation = { Vector3(matWorld.m[3][0], matWorld.m[3][1], matWorld.m[3][2]) };
	}

	parent = nullptr;
}
