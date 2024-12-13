/**
 * @file Sprite.h
 * @brief スプライトの読み込み、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Base/TextureManager/TextureManager.h"
#include "Engine/Utility/Math/MyMath.h"
#include <array>
#include <dxcapi.h>

#pragma comment(lib,"dxcompiler.lib")

class Sprite
{
public:
	//ブレンドモード
	enum BlendMode
	{
		//ブレンドなし
		kBlendModeNone,

		//通常ブレンド
		kBlendModeNormal,

		//加算ブレンド
		kBlendModeAdd,

		//減算ブレンド
		kBlendModeSubtract,

		//乗算ブレンド
		kBlendModeMultiply,

		//スクリーンブレンド
		kBlendModeScreen,

		//ブレンドモードの数
		kCountOfBlendMode,
	};

	//頂点データ
	struct VertexData
	{
		//頂点位置
		Vector4 position{};

		//テクスチャ座標
		Vector2 texcoord{};
	};

	//マテリアルデータ
	struct MaterialData
	{
		//色
		Vector4 color{};

		//UV変換行列
		Matrix4x4 uvTransform{};
	};

	/// <summary>静的初期化</summary>
	static void StaticInitialize();

	/// <summary>初期化</summary>
	void Initialize(uint32_t textureHandle, Vector2 position);

	/// <summary>更新</summary>
	void Update();

	/// <summary>描画</summary>
	void Draw();

	/// <summary>解放</summary>
	static void Release();

	/// <summary>スプライトの生成</summary>
	static Sprite* Create(uint32_t textureHandle, Vector2 position);

	/// <summary>描画前の処理</summary>
	static void PreDraw(BlendMode blendMode);

	/// <summary>描画後の処理</summary>
	static void PostDraw();

	/// <summary>ImGui</summary>
	void ImGui();

	//Getter・Setter
	//Position
	const Vector2& GetPosition() const { return position_; };
	void SetPosition(const Vector2& position) { position_ = position; };

	//Rotation
	const float& GetRotation() const { return rotation_; };
	void SetRotation(const float& rotation) { rotation_ = rotation; };

	//Size
	const Vector2& GetSize() const { return size_; };
	void SetSize(const Vector2& size) { size_ = size; };

	//Color
	const Vector4& GetColor() const { return color_; };
	void SetColor(const Vector4& color) { color_ = color; };

	//TextureHandle
	const uint32_t& GetTexture() const { return textureHandle_; };
	void SetTexture(const uint32_t& textureHandle) { textureHandle_ = textureHandle; };

	//拡張機能
	//AnchorPoint
	const Vector2& GetAnchorPoint() const { return anchorPoint_; };
	void SetAnchorPoint(const Vector2& anchorPoint) { anchorPoint_ = anchorPoint; };

	//Flip
	bool GetIsFlipX() { return isFlipX_; };
	void SetIsFlipX(bool isFlipX) { isFlipX_ = isFlipX; };

	bool GetIsFlipY() { return isFlipY_; };
	void SetIsFlipY(bool isFlipY) { isFlipY_ = isFlipY; };

	//TextureLeftTop
	const Vector2& GetTextureLeftTop() const { return textureLeftTop_; };
	void SetTextureLeftTop(const Vector2& textureLeftTop) { textureLeftTop_ = textureLeftTop; };

	//TextureSize
	const Vector2& GetTextureSize() const { return textureSize_; };
	void SetTextureSize(const Vector2& textureSize) { textureSize_ = textureSize; };

private:
	/// <summary>DXCの初期化</summary>
	static void InitializeDXC();

	/// <summary>シェーダーコンパイル</summary>
	static Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile);

	/// <summary>PSOの作成</summary>
	static void CreatePSO();

	/// <summary>頂点バッファの作成</summary>
	void CreateVertexBuffer();

	/// <summary>マテリアルリソースの作成</summary>
	void CreateMaterialResource();

	/// <summary>WVPリソースの作成</summary>
	void CreateWVPResource();

	/// <summary>頂点の更新</summary>
	void UpdateVertex();

	/// <summary>マテリアルの更新</summary>
	void UpdateMaterial();

	/// <summary>行列の更新</summary>
	void UpdateMatrix();

	/// <summary>テクスチャサイズの調整</summary>
	void AdjustTextureSize();

private:
	//DirectXCoreのポインタ
	static DirectXCore* sDxCore_;

	//TextureManagerのポインタ
	static TextureManager* sTextureManager_;

	//DirectX関連のリソース
	static ID3D12Device* sDevice_;
	static ID3D12GraphicsCommandList* sCommandList_;
	static Microsoft::WRL::ComPtr<IDxcUtils> sDxcUtils_;
	static Microsoft::WRL::ComPtr<IDxcCompiler3> sDxcCompiler_;
	static Microsoft::WRL::ComPtr<IDxcIncludeHandler> sIncludeHandler_;

	//パイプライン関連
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> sRootSignature_;
	static std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, kCountOfBlendMode> sGraphicsPipelineState_;

	//プロジェクション行列
	static Matrix4x4 matProjection_;

	//頂点
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	static const uint32_t kNumVertices_ = 6;

	//位置
	Vector2 position_ = { 0.0f,0.0f };

	//回転
	float rotation_ = 0.0f;

	//サイズ
	Vector2 size_ = { 1.0f,1.0f };

	//テクスチャ座標の基準位置
	Vector2 texBase_ = { 0.0f,0.0f };

	//テクスチャのサイズ
	Vector2 texSize_ = { 1.0f,1.0f };

	//テクスチャハンドル
	uint32_t textureHandle_{};

	//リソース情報
	D3D12_RESOURCE_DESC resourceDesc_{};

	//マテリアルリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;

	//WVPリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_ = nullptr;

	//色
	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };

	//UV
	Vector2 uvTranslation_ = { 0.0f,0.0f };
	float uvRotation_ = 0.0f;
	Vector2 uvScale_ = { 1.0f,1.0f };

	//アンカーポイント
	Vector2 anchorPoint_ = { 0.0f,0.0f };
	const float kDefaultAnchorX = 0.0f;
	const float kDefaultAnchorY = 0.0f;

	//反転するか
	bool isFlipX_ = false;
	bool isFlipY_ = false;

	//テクスチャの左上位置
	Vector2 textureLeftTop_ = { 0.0f,0.0f };
	const float kDefaultTextureLeftTopX = 0.0f;
	const float kDefaultTextureLeftTopY = 0.0f;

	//テクスチャのサイズ
	Vector2 textureSize_ = { 100.0f,100.0f };
	const float kDefaultTextureWidth = 1.0f;
	const float kDefaultTextureHeight = 1.0f;
};
