#include "Triangle.h"

void Triangle::Initialize()
{
	dxCore_ = DirectXCore::GetInstance();
	textureManager_ = TextureManager::GetInstance();

	vertexResource_ = dxCore_->CreateBufferResource(sizeof(VertexData) * 3);
	materialResource_ = dxCore_->CreateBufferResource(sizeof(Material));

	CreateVertexBufferView();

	vertexResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	//左下
	vertexData_[0].position = { -0.5f,0.0f,0.0f,1.0f };
	vertexData_[0].texcoord = { 0.0f,1.0f };

	//上
	vertexData_[1].position = { 0.0f,1.0f,0.0f,1.0f };
	vertexData_[1].texcoord = { 0.5f,0.0f };

	//右下
	vertexData_[2].position = { 0.5f,0.0f,0.0f,1.0f };
	vertexData_[2].texcoord = { 1.0f,1.0f };

	//色を書き込むアドレスを取得
	materialResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

	//色情報を書き込む
	materialData_->color = color_;
	materialData_->enableLighting = false;
	materialData_->uvTransform = MakeIdentity4x4();
}

void Triangle::Draw(const WorldTransform& transform, const Camera& camera, const uint32_t textureHandle)
{
	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTranformTriAngle_.scale);
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTranformTriAngle_.rotate.z));
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTranformTriAngle_.translate));
	materialData_->uvTransform = uvTransformMatrix;

	dxCore_->GetcommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);

	dxCore_->GetcommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//WorldTransform
	dxCore_->GetcommandList()->SetGraphicsRootConstantBufferView(1, transform.constBuff->GetGPUVirtualAddress());

	//ViewProjection
	dxCore_->GetcommandList()->SetGraphicsRootConstantBufferView(4, camera.constBuff->GetGPUVirtualAddress());

	//色用のCBufferの場所を特定
	dxCore_->GetcommandList()->SetGraphicsRootConstantBufferView(0, materialResource_.Get()->GetGPUVirtualAddress());

	//SRVのDescriptorTableの先頭を設定　2はrootParameter[2]の2
	dxCore_->GetcommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetGPUHandle(textureHandle));

	dxCore_->GetcommandList()->DrawInstanced(3, 1, 0, 0);
}

void Triangle::ImGui(const char* Title)
{
	ImGui::Begin(Title);
	ImGui::SliderFloat3("Scale", &tranformTriAngle_.scale.x, 1, 30, "%.3f");
	ImGui::SliderFloat3("Rotate", &tranformTriAngle_.rotate.x, -7, 7, "%.3f");
	ImGui::SliderFloat3("Translate", &tranformTriAngle_.translate.x, -2, 2, "%.3f");
	ImGui::DragFloat2("UVScale", &uvTranformTriAngle_.scale.x, 0.01f, -10.0f, 10.0f);
	ImGui::SliderAngle("UVRotate", &uvTranformTriAngle_.rotate.z);
	ImGui::DragFloat2("UVTranslate", &uvTranformTriAngle_.translate.x, 0.01f, -10.0f, 10.0f);
	ImGui::ColorPicker4("Color", &color_.x);
	ImGui::End();
}

void Triangle::CreateVertexBufferView()
{
	vertexBufferView_.BufferLocation = vertexResource_.Get()->GetGPUVirtualAddress();

	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 3;

	vertexBufferView_.StrideInBytes = sizeof(VertexData);
}

