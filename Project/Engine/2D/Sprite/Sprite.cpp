/**
 * @file Sprite.cpp
 * @brief スプライトの読み込み、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "Sprite.h"

DirectXCore* Sprite::sDxCore_ = nullptr;
TextureManager* Sprite::sTextureManager_ = nullptr;
ID3D12Device* Sprite::sDevice_ = nullptr;
ID3D12GraphicsCommandList* Sprite::sCommandList_ = nullptr;
Microsoft::WRL::ComPtr<IDxcUtils> Sprite::sDxcUtils_ = nullptr;
Microsoft::WRL::ComPtr<IDxcCompiler3> Sprite::sDxcCompiler_ = nullptr;
Microsoft::WRL::ComPtr<IDxcIncludeHandler> Sprite::sIncludeHandler_ = nullptr;
Microsoft::WRL::ComPtr<ID3D12RootSignature> Sprite::sRootSignature_ = nullptr;
std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, Sprite::kCountOfBlendMode> Sprite::sGraphicsPipelineState_{};
Matrix4x4 Sprite::matProjection_{};

void Sprite::StaticInitialize()
{
	//DirectXCoreのインスタンスの取得
	sDxCore_ = DirectXCore::GetInstance();

	//TextureManagerのインスタンスの取得
	sTextureManager_ = TextureManager::GetInstance();

	//デバイスの取得
	sDevice_ = sDxCore_->GetDevice();

	//コマンドリストの取得
	sCommandList_ = sDxCore_->GetCommandList();

	//DXCの初期化
	InitializeDXC();

	//PSOの作成
	CreatePSO();

	//投影の設定
	const float kNearClip = 0.0f;
	const float kFarClip = 100.0f;
	const float kLeft = 0.0f;
	const float kBottom = 0.0f;
	const float kRight = 1280.0f;
	const float kTop = 720.0f;

	//投影行列の作成
	matProjection_ = MakeOrthographicMatrix(kLeft, kBottom, kRight, kTop, kNearClip, kFarClip);
}

void Sprite::Initialize(uint32_t textureHandle, Vector2 position)
{
	//テクスチャハンドルの設定
	textureHandle_ = textureHandle;

	//スプライトの位置の設定
	position_ = position;

	//テクスチャのリソース情報を取得
	resourceDesc_ = sTextureManager_->GetResourceDesc(textureHandle_);

	//テクスチャのサイズを設定
	texSize_ = { float(resourceDesc_.Width),float(resourceDesc_.Height) };
	size_ = { float(resourceDesc_.Width),float(resourceDesc_.Height) };

	//頂点バッファの作成
	CreateVertexBuffer();

	//マテリアル用のリソースの作成
	CreateMaterialResource();

	//wvp用のリソースの作成
	CreateWVPResource();
}

void Sprite::Update()
{
	//テクスチャサイズの調整
	AdjustTextureSize();

	//頂点情報の更新
	UpdateVertex();
}

void Sprite::Draw()
{
	//更新
	Update();

	//マテリアルの更新
	UpdateMaterial();

	//行列の更新
	UpdateMatrix();

	//VBVを設定
	sCommandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);

	//形状を設定。PSOに設定しているものとは別。同じものを設定すると考えておけば良い
	sCommandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//マテリアルCBufferの場所を設定
	sCommandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

	//wvp用のCBufferの場所を設定
	sCommandList_->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());

	//DescriptorHeapを設定
	sTextureManager_->SetGraphicsDescriptorHeap();

	//DescriptorTableを設定
	sTextureManager_->SetGraphicsRootDescriptorTable(2, textureHandle_);

	//描画
	sCommandList_->DrawInstanced(kNumVertices_, 1, 0, 0);
}

void Sprite::Release()
{
	//解放
	sDxcUtils_.Reset();
	sDxcCompiler_.Reset();
	sIncludeHandler_.Reset();
	sRootSignature_.Reset();

	for (int i = 0; i < kCountOfBlendMode; i++)
	{
		sGraphicsPipelineState_[i].Reset();
	}
}

Sprite* Sprite::Create(uint32_t textureHandle, Vector2 position)
{
	//スプライトを作成
	Sprite* sprite = new Sprite();
	sprite->Initialize(textureHandle, position);

	return sprite;
}

void Sprite::PreDraw(BlendMode blendMode)
{
	//ルートシグネチャの設定
	sCommandList_->SetGraphicsRootSignature(sRootSignature_.Get());

	//パイプライン状態の設定
	sCommandList_->SetPipelineState(sGraphicsPipelineState_[blendMode].Get());
}

void Sprite::PostDraw()
{

}

void Sprite::ImGui()
{

}

void Sprite::InitializeDXC()
{
	//dxccompilerを初期化
	HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&sDxcUtils_));
	assert(SUCCEEDED(hr));

	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&sDxcCompiler_));
	assert(SUCCEEDED(hr));

	//現時点ではincludeはしないが、includeに対応するための設定を行っておく
	hr = sDxcUtils_->CreateDefaultIncludeHandler(&sIncludeHandler_);
	assert(SUCCEEDED(hr));
}

Microsoft::WRL::ComPtr<IDxcBlob> Sprite::CompileShader(const std::wstring& filePath, const wchar_t* profile)
{
	//これからシェーダーをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}\n", filePath, profile)));
	//hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = sDxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	//読めなかったら止める
	assert(SUCCEEDED(hr));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer{};
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;//UTF8の文字コードであることを通知


	LPCWSTR arguments[] = {
		filePath.c_str(),//コンパイル対象のhlslファイル名
		L"-E",L"main",//エントリーポイントの指定。基本的にmain以外にはしない
		L"-T",profile,//ShaderProfileの設定
		L"-Zi",L"-Qembed_debug",//デバッグ用の情報を埋め込む
		L"-Od",//最適化を外しておく
		L"-Zpr",//メモリレイアウトは行優先
	};
	//実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = sDxcCompiler_->Compile(
		&shaderSourceBuffer,//読み込んだファイル
		arguments,//コンパイルオプション
		_countof(arguments),//コンパイルオプションの数
		sIncludeHandler_.Get(),//includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)//コンパイル結果
	);
	//コンパイルエラーではなくdxcが起動できないほど致命的な状況
	assert(SUCCEEDED(hr));


	//警告・エラーが出てたらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), _In_opt_ nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(shaderError->GetStringPointer());
		//警告・エラーダメゼッタイ
		assert(false);
	}


	//コンパイル結果から実行用のバイナリ部分を取得
	Microsoft::WRL::ComPtr<IDxcBlob> shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	//成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));
	//もう使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();

	//実行用のバイナリを返却
	return shaderBlob;
}

void Sprite::CreatePSO()
{
	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//DescriptorRange作成
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;//0から始まる
	descriptorRange[0].NumDescriptors = 1;//数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	//RootParameter作成。複数設定できるので配列。今回は結果一つだけなので長さ1の配列
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVで使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0;//レジスタ番号0を使う
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数
	descriptionRootSignature.pParameters = rootParameters;//ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);//配列の長さ

	//Sampler作成
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;//レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	//シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成
	hr = sDevice_->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&sRootSignature_));
	assert(SUCCEEDED(hr));


	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);


	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//カリングしない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


	//Shaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = CompileShader(L"Resource/Shaders/Sprite.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = CompileShader(L"Resource/Shaders/Sprite.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);


	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;


	//PSOを作成する
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = sRootSignature_.Get();//RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;//InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };//VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };//PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;//BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ(形状)のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むかの設定(気にしなくて良い)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//実際に生成
	hr = sDevice_->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&sGraphicsPipelineState_[kBlendModeNone]));
	assert(SUCCEEDED(hr));


	//共通設定
	blendDesc.RenderTarget[0].BlendEnable = true;//ブレンドを有効にする
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う
	//半透明合成
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;//加算
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースのアルファ値
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f-ソースのアルファ値
	//BlendModeを再設定
	graphicsPipelineStateDesc.BlendState = blendDesc;

	//実際に生成
	hr = sDevice_->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&sGraphicsPipelineState_[kBlendModeNormal]));
	assert(SUCCEEDED(hr));


	//加算合成
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;//加算
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースの値を100%使う
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;//デストの値を100%使う
	//BlendModeを再設定
	graphicsPipelineStateDesc.BlendState = blendDesc;

	//実際に生成
	hr = sDevice_->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&sGraphicsPipelineState_[kBlendModeAdd]));
	assert(SUCCEEDED(hr));


	//減算合成
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;//減算
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースの値を100%使う
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;//デストの値を100%使う
	//BlendModeを再設定
	graphicsPipelineStateDesc.BlendState = blendDesc;

	//実際に生成
	hr = sDevice_->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&sGraphicsPipelineState_[kBlendModeSubtract]));
	assert(SUCCEEDED(hr));


	//乗算合成
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;//加算
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;//ソースの値を0%使う
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;//デストの色を使う
	//BlendModeを再設定
	graphicsPipelineStateDesc.BlendState = blendDesc;

	//実際に生成
	hr = sDevice_->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&sGraphicsPipelineState_[kBlendModeMultiply]));
	assert(SUCCEEDED(hr));


	//スクリーン合成
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;//加算
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;//1-ソースの色を使う
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;//デストの色を100%使う
	//BlendModeを再設定
	graphicsPipelineStateDesc.BlendState = blendDesc;

	//実際に生成
	hr = sDevice_->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&sGraphicsPipelineState_[kBlendModeScreen]));
	assert(SUCCEEDED(hr));
}

void Sprite::CreateVertexBuffer()
{
	//頂点バッファのリソースの作成
	vertexResource_ = sDxCore_->CreateBufferResource(sizeof(VertexData) * kNumVertices_);

	//頂点バッファビューの設定
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * kNumVertices_;
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//アンカー点に基づいて四隅の位置を計算
	float left = kDefaultAnchorX - anchorPoint_.x;
	float right = kDefaultTextureWidth - anchorPoint_.x;
	float top = kDefaultAnchorY - anchorPoint_.y;
	float bottom = kDefaultTextureHeight - anchorPoint_.y;

	//テクスチャのUV座標を計算
	float texLeft = textureLeftTop_.x / resourceDesc_.Width;
	float texRight = (textureLeftTop_.x + textureSize_.x) / resourceDesc_.Width;
	float texTop = textureLeftTop_.y / resourceDesc_.Height;
	float texBottom = (textureLeftTop_.y + textureSize_.y) / resourceDesc_.Height;

	//反転処理(X軸)
	if (isFlipX_)
	{
		left = -left;
		right = -right;
	}

	//反転処理(Y軸)
	if (isFlipY_)
	{
		top = -top;
		bottom = -bottom;
	}

	//頂点データを設定
	VertexData* vertexData = nullptr;

	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	vertexData[0].position = { left,bottom,0.0f,1.0f };//左下
	vertexData[0].texcoord = { texLeft,texBottom };
	vertexData[1].position = { left,top,0.0f,1.0f };//左上
	vertexData[1].texcoord = { texLeft,texTop };
	vertexData[2].position = { right,bottom,0.0f,1.0f };//右下
	vertexData[2].texcoord = { texRight,texBottom };
	vertexData[3].position = { left,top,0.0f,1.0f };//左上
	vertexData[3].texcoord = { texLeft,texTop };
	vertexData[4].position = { right,top,0.0f,1.0f };//右上
	vertexData[4].texcoord = { texRight,texTop };
	vertexData[5].position = { right,bottom,0.0f,1.0f };//右上
	vertexData[5].texcoord = { texRight,texBottom };

	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
}

void Sprite::CreateMaterialResource()
{
	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	materialResource_ = sDxCore_->CreateBufferResource(sizeof(MaterialData));
	//マテリアルにデータを書き込む
	MaterialData* materialData = nullptr;
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	//今回は赤を書き込んでみる
	materialData->color = color_;
	materialData->uvTransform = MakeIdentity4x4();
}

void Sprite::CreateWVPResource()
{
	//WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	wvpResource_ = sDxCore_->CreateBufferResource(sizeof(Matrix4x4));
	//データを書き込む
	Matrix4x4* wvpData = nullptr;
	//書き込むためのアドレスを取得
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//単位行列を書き込んでおく
	*wvpData = MakeIdentity4x4();
}

void Sprite::UpdateVertex()
{
	//アンカー点に基づいて四隅の位置を計算
	float left = kDefaultAnchorX - anchorPoint_.x;
	float right = kDefaultTextureWidth - anchorPoint_.x;
	float top = kDefaultAnchorY - anchorPoint_.y;
	float bottom = kDefaultTextureHeight - anchorPoint_.y;

	//テクスチャのUV座標を計算
	float texLeft = textureLeftTop_.x / resourceDesc_.Width;
	float texRight = (textureLeftTop_.x + textureSize_.x) / resourceDesc_.Width;
	float texTop = textureLeftTop_.y / resourceDesc_.Height;
	float texBottom = (textureLeftTop_.y + textureSize_.y) / resourceDesc_.Height;

	//反転処理(X軸)
	if (isFlipX_)
	{
		left = -left;
		right = -right;
	}

	//反転処理(Y軸)
	if (isFlipY_)
	{
		top = -top;
		bottom = -bottom;
	}

	//頂点データを設定
	VertexData* vertexData = nullptr;

	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	vertexData[0].position = { left,bottom,0.0f,1.0f };//左下
	vertexData[0].texcoord = { texLeft,texBottom };
	vertexData[1].position = { left,top,0.0f,1.0f };//左上
	vertexData[1].texcoord = { texLeft,texTop };
	vertexData[2].position = { right,bottom,0.0f,1.0f };//右下
	vertexData[2].texcoord = { texRight,texBottom };
	vertexData[3].position = { left,top,0.0f,1.0f };//左上
	vertexData[3].texcoord = { texLeft,texTop };
	vertexData[4].position = { right,top,0.0f,1.0f };//右上
	vertexData[4].texcoord = { texRight,texTop };
	vertexData[5].position = { right,bottom,0.0f,1.0f };//右上
	vertexData[5].texcoord = { texRight,texBottom };

	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
}

void Sprite::UpdateMaterial()
{
	//マテリアルにデータを書き込む
	MaterialData* materialData = nullptr;
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	//今回は赤を書き込んでみる
	materialData->color = color_;
	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(Vector3{ uvScale_.x,uvScale_.y,1.0f });
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(uvRotation_));
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(Vector3{ uvTranslation_.x,uvTranslation_.y,0.0f }));
	materialData->uvTransform = uvTransformMatrix;
}

void Sprite::UpdateMatrix()
{
	//ワールド行列の作成
	Matrix4x4 worldMatrix = MakeAffineMatrix(Vector3(size_.x, size_.y, 1.0f), Vector3(0.0f, 0.0f, rotation_), Vector3(position_.x, position_.y, 0.0f));
	//ビュー行列の作成
	Matrix4x4 viewMatrix = MakeIdentity4x4();
	//WVPMatrixの作成
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, matProjection_));

	//データを書き込む
	Matrix4x4* wvpData = nullptr;
	//書き込むためのアドレスを取得
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//単位行列を書き込んでおく
	*wvpData = worldViewProjectionMatrix;
}

void Sprite::AdjustTextureSize()
{
	//テクスチャリソースの取得
	resourceDesc_ = sTextureManager_->GetResourceDesc(textureHandle_);

	//テクスチャのサイズを設定
	textureSize_ = { float(resourceDesc_.Width),float(resourceDesc_.Height) };
}
