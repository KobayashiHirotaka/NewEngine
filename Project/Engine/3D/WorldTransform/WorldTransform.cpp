#include "WorldTransform.h"

void WorldTransform::Initialize()
{
	matWorld = MakeIdentity4x4();
	CreateConstBuffer();
	Map();
	TransferMatrix();
}

void WorldTransform::CreateConstBuffer()
{
	constBuff = DirectXCore::GetInstance()->CreateBufferResource(sizeof(ConstBufferDataWorldTransform));
}

void WorldTransform::Map()
{
	constBuff.Get()->Map(0, nullptr, reinterpret_cast<void**>(&constMap));
}

void WorldTransform::TransferMatrix()
{
	constMap->matWorld = matWorld;
}

void WorldTransform::UpdateMatrix(RotationType rotationType)
{
	switch (rotationType)
	{
	case RotationType::Euler:
		matWorld = MakeAffineMatrix(scale, rotation, translation);
		break;

	case RotationType::Quaternion:
		matWorld = MakeAffineMatrix(scale, quaternion, translation);
		break;
	}
	
	if (parent_)
	{
		matWorld = Multiply(matWorld, parent_->matWorld);
	}

	TransferMatrix();
}

void WorldTransform::SetParent(const WorldTransform* parent)
{
	parent_ = parent;

	if (parent_)
	{
		translation = Subtract(translation, parent->translation);
	}
}

void WorldTransform::DeleteParent()
{
	if (parent_)
	{
		translation = { Vector3(matWorld.m[3][0], matWorld.m[3][1], matWorld.m[3][2]) };
	}

	parent_ = nullptr;
}
