#include "Sprite.h"

void Sprite::Initialize(const Vector4& LeftTop, const Vector4& LeftBottom, const Vector4& RightTop, const Vector4& RightBottom, const uint32_t textureHandle)
{
	dxCore_ = DirectXCore::GetInstance();
	textureManager_ = TextureManager::GetInstance();

	textureHandle_ = textureHandle;

	vertexResourceSprite_ = dxCore_->CreateBufferResource(sizeof(VertexData) * 4);
	materialResourceSprite_ = dxCore_->CreateBufferResource(sizeof(Material));
	indexResourceSprite_ = dxCore_->CreateBufferResource(sizeof(uint32_t) * 6);

	CreateVertexBufferViewSprite();
	CreateIndexBufferViewSprite();

	vertexResourceSprite_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite_));

	D3D12_RESOURCE_DESC resDesc = textureManager_->GetResourceDesc(textureHandle_);

	float texLeft = textureLeftTop_.x / resDesc.Width;
	float texRight = (textureLeftTop_.x + textureSize_.x) / resDesc.Width;
	float texTop = textureLeftTop_.y / resDesc.Height;
	float texBottom = (textureLeftTop_.y + textureSize_.y) / resDesc.Height;

	//左下
	vertexDataSprite_[0].position = LeftBottom;
	vertexDataSprite_[0].texcoord = { texLeft,texBottom };

	//左上
	vertexDataSprite_[1].position = LeftTop;
	vertexDataSprite_[1].texcoord = { texLeft,texTop };

	//右下
	vertexDataSprite_[2].position = RightBottom;
	vertexDataSprite_[2].texcoord = { texRight,texBottom };

	//右上
	vertexDataSprite_[3].position = RightTop;
	vertexDataSprite_[3].texcoord = { texRight,texTop };

	//インデックスリソースにデータを書き込む
	indexResourceSprite_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite_));

	//三角形1枚目
	indexDataSprite_[0] = 0;
	indexDataSprite_[1] = 1;
	indexDataSprite_[2] = 2;

	//三角形2枚目
	indexDataSprite_[3] = 1;
	indexDataSprite_[4] = 3;
	indexDataSprite_[5] = 2;

	camera_.Initialize();
	camera_.constMap->view = MakeIdentity4x4();
	camera_.constMap->projection = MakeOrthographicMatrix(0.0f, 0.0f, float(WindowsApp::kClientWidth), float(WindowsApp::kClientHeight), 0.0f, 100.0f);
}

void Sprite::Draw(const WorldTransform& transform)
{
	//色の書き込み
	materialResourceSprite_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite_));
	materialDataSprite_->color = color_;

	//ライティングをしない
	materialDataSprite_->enableLighting = false;
	materialDataSprite_->uvTransform = MakeIdentity4x4();

	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransformSprite_.scale);
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransformSprite_.rotate.z));
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransformSprite_.translate));
	materialDataSprite_->uvTransform = uvTransformMatrix;

	dxCore_->GetcommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//頂点
	dxCore_->GetcommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSprite_);
	dxCore_->GetcommandList()->IASetIndexBuffer(&indexBufferViewSprite_);

	//WorldTransform
	dxCore_->GetcommandList()->SetGraphicsRootConstantBufferView(1, transform.constBuff.Get()->GetGPUVirtualAddress());

	//ViewProjection
	dxCore_->GetcommandList()->SetGraphicsRootConstantBufferView(4, camera_.constBuff->GetGPUVirtualAddress());

	//色用のCBufferの場所を特定
	dxCore_->GetcommandList()->SetGraphicsRootConstantBufferView(0, materialResourceSprite_.Get()->GetGPUVirtualAddress());

	//テクスチャ
	dxCore_->GetcommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetGPUHandle(textureHandle_));

	dxCore_->GetcommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Sprite::ImGui(const char* Title)
{
	ImGui::Begin(Title);
	ImGui::SliderFloat3("ScaleSprite", &transformSprite_.scale.x, 1, 30, "%.3f");
	ImGui::SliderFloat3("RotateSprite", &transformSprite_.rotate.x, -7, 7, "%.3f");
	ImGui::SliderFloat3("TranslateSprite", &transformSprite_.translate.x, -640, 640, "%.3f");
	ImGui::DragFloat2("UVTranslate", &uvTransformSprite_.translate.x, 0.01f, -10.0f, 10.0f);
	ImGui::DragFloat2("UVScale", &uvTransformSprite_.scale.x, 0.01f, -10.0f, 10.0f);
	ImGui::SliderAngle("UVRotate", &uvTransformSprite_.rotate.z);
	ImGui::ColorEdit4("Color", &color_.x);
	ImGui::End();
}

void Sprite::CreateVertexBufferViewSprite()
{
	//リソースの先頭のアドレス
	vertexBufferViewSprite_.BufferLocation = vertexResourceSprite_.Get()->GetGPUVirtualAddress();
	//使用する頂点サイズ
	vertexBufferViewSprite_.SizeInBytes = sizeof(VertexData) * 4;
	//1頂点あたりのアドレス
	vertexBufferViewSprite_.StrideInBytes = sizeof(VertexData);
}

void Sprite::CreateIndexBufferViewSprite()
{
	//リソース先頭アドレス
	indexBufferViewSprite_.BufferLocation = indexResourceSprite_.Get()->GetGPUVirtualAddress();
	//使用するインデックスサイズ
	indexBufferViewSprite_.SizeInBytes = sizeof(uint32_t) * 6;
	//インデックスはuint32_t
	indexBufferViewSprite_.Format = DXGI_FORMAT_R32_UINT;
}
