/**
 * @file Skybox.cpp
 * @brief Skyboxの生成、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "Skybox.h"

DirectXCore* Skybox::dxCore_ = nullptr;
TextureManager* Skybox::textureManager_ = nullptr;
ID3D12Device* Skybox::device_ = nullptr;
ID3D12GraphicsCommandList* Skybox::commandList_ = nullptr;
Microsoft::WRL::ComPtr<IDxcUtils> Skybox::dxcUtils_ = nullptr;
Microsoft::WRL::ComPtr<IDxcCompiler3> Skybox::dxcCompiler_ = nullptr;
Microsoft::WRL::ComPtr<IDxcIncludeHandler> Skybox::includeHandler_ = nullptr;
Microsoft::WRL::ComPtr<ID3D12RootSignature> Skybox::rootSignature_ = nullptr;
Microsoft::WRL::ComPtr<ID3D12PipelineState> Skybox::graphicsPipelineState_ = nullptr;

void Skybox::StaticInitialize()
{
	dxCore_ = DirectXCore::GetInstance();

	textureManager_ = TextureManager::GetInstance();

	device_ = dxCore_->GetDevice();

	commandList_ = dxCore_->GetCommandList();

	InitializeDXC();

	CreatePSO();
}

void Skybox::Initialize()
{
	//テクスチャの読み込み
	textureHandle_ = textureManager_->LoadTexture("resource/images/skybox.dds");

	//頂点リソースの作成
	CreateVertexResource();

	//インデックスリソースの作成
	CreateIndexResource();

	//マテリアル用のリソースの作成
	CreateMaterialResource();
}

void Skybox::Draw(WorldTransform& worldTransform, const Camera& camera)
{
	//Materialの更新
	UpdateMaterialResource();
	
	//DescriptorHeapを設定
	textureManager_->SetGraphicsDescriptorHeap();

	//VBVを設定
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);

	//形状を設定
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//IndexBufferを設定
	commandList_->IASetIndexBuffer(&indexBufferView_);

	//マテリアルCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(UINT(RootParameterIndex::Material), materialResource_->GetGPUVirtualAddress());

	//WorldTransform用のCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(UINT(RootParameterIndex::WorldTransform), worldTransform.constBuff->GetGPUVirtualAddress());

	//ViewProjection用のCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(UINT(RootParameterIndex::ViewProjection), camera.constBuff_->GetGPUVirtualAddress());

	//DescriptorTableを設定
	textureManager_->SetGraphicsRootDescriptorTable(UINT(RootParameterIndex::Texture), textureHandle_);

	//描画
	commandList_->DrawIndexedInstanced(kMaxIndices, 1, 0, 0, 0);
}

void Skybox::Release()
{
	dxcUtils_.Reset();
	dxcCompiler_.Reset();
	includeHandler_.Reset();
	rootSignature_.Reset();
	graphicsPipelineState_.Reset();
}

Skybox* Skybox::Create()
{
	Skybox* skybox = new Skybox();
	skybox->Initialize();

	return skybox;
}

void Skybox::PreDraw()
{
	commandList_->SetGraphicsRootSignature(rootSignature_.Get());
	commandList_->SetPipelineState(graphicsPipelineState_.Get());
}

void Skybox::PostDraw()
{

}

void Skybox::InitializeDXC()
{
	//dxccompilerを初期化
	HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr));

	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr));

	//現時点ではincludeはしないが、includeに対応するための設定を行っておく
	hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr));
}

Microsoft::WRL::ComPtr<IDxcBlob> Skybox::CompileShader(const std::wstring& filePath, const wchar_t* profile)
{
	//これからシェーダーをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}\n", filePath, profile)));
	//hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);
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
	hr = dxcCompiler_->Compile(
		&shaderSourceBuffer,//読み込んだファイル
		arguments,//コンパイルオプション
		_countof(arguments),//コンパイルオプションの数
		includeHandler_.Get(),//includeが含まれた諸々
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


void Skybox::CreatePSO()
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

	//RootParameter作成
	D3D12_ROOT_PARAMETER rootParameters[4] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVで使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVで使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVで使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	rootParameters[2].Descriptor.ShaderRegister = 1;//レジスタ番号1とバインド
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[3].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
	rootParameters[3].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数

	descriptionRootSignature.pParameters = rootParameters;
	descriptionRootSignature.NumParameters = _countof(rootParameters);

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
	hr = device_->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));


	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[1] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);


	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//共通設定
	blendDesc.RenderTarget[0].BlendEnable = false;//ブレンドを有効にする
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う

	////半透明合成
	//blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;//加算
	//blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースのアルファ値
	//blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f-ソースのアルファ値

	////加算合成
	//blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;//加算
	//blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースの値を100%使う
	//blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;//デストの値を100%使う

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


	//Shaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = CompileShader(L"resource/shaders/Skybox.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = CompileShader(L"resource/shaders/Skybox.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);


	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	//depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthEnable = false;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;


	//PSOを作成する
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();//RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;//InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize() };//VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize() };//PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;//BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ(形状)のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むかの設定(気にしなくて良い)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	
	//実際に生成
	hr = device_->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(hr));
}

void Skybox::CreateVertexResource()
{
	//右面。描画インデックスは[0,1,2][2,1,3]で内側を向く
	vertices_[0] = { 1.0f,1.0f,1.0f,1.0f };
	vertices_[1] = { 1.0f,1.0f,-1.0f,1.0f };
	vertices_[2] = { 1.0f,-1.0f,1.0f,1.0f };
	vertices_[3] = { 1.0f,-1.0f,-1.0f,1.0f };

	//左面。描画インデックスは[4,5,6][6,5,7]
	vertices_[4] = { -1.0f,1.0f,-1.0f,1.0f };
	vertices_[5] = { -1.0f,1.0f,1.0f,1.0f };
	vertices_[6] = { -1.0f,-1.0f,-1.0f,1.0f };
	vertices_[7] = { -1.0f,-1.0f,1.0f,1.0f };

	//前面。描画インデックスは[8,9,10][10,9,11]
	vertices_[8] = { -1.0f,1.0f,1.0f,1.0f };
	vertices_[9] = { 1.0f,1.0f,1.0f,1.0f };
	vertices_[10] = { -1.0f,-1.0f,1.0f,1.0f };
	vertices_[11] = { 1.0f,-1.0f,1.0f,1.0f };

	//後面。描画インデックスは[12,13,14][14,13,15]
	vertices_[12] = { 1.0f,1.0f,-1.0f,1.0f };
	vertices_[13] = { -1.0f,1.0f,-1.0f,1.0f };
	vertices_[14] = { 1.0f,-1.0f,-1.0f,1.0f };
	vertices_[15] = { -1.0f,-1.0f,-1.0f,1.0f };

	//上面。描画インデックスは[16,17,18][18,17,19]
	vertices_[16] = { -1.0f,1.0f,-1.0f,1.0f };
	vertices_[17] = { 1.0f,1.0f,-1.0f,1.0f };
	vertices_[18] = { -1.0f,1.0f,1.0f,1.0f };
	vertices_[19] = { 1.0f,1.0f,1.0f,1.0f };

	//下面。描画インデックスは[20,21,22][22,21,23]
	vertices_[20] = { -1.0f,-1.0f,1.0f,1.0f };
	vertices_[21] = { 1.0f,-1.0f,1.0f,1.0f };
	vertices_[22] = { -1.0f,-1.0f,-1.0f,1.0f };
	vertices_[23] = { 1.0f,-1.0f,-1.0f,1.0f };

	vertexResource_ = dxCore_->CreateBufferResource(sizeof(Vector4) * kMaxVertices);

	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(Vector4) * kMaxVertices);
	vertexBufferView_.StrideInBytes = sizeof(Vector4);

	Vector4* vertexData = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, vertices_.data(), sizeof(Vector4) * vertices_.size());
	vertexResource_->Unmap(0, nullptr);
}

void Skybox::CreateIndexResource()
{
	//右面。描画インデックスは[0,1,2][2,1,3]で内側を向く
	indices_[0] = 0;
	indices_[1] = 1;
	indices_[2] = 2;
	indices_[3] = 2;
	indices_[4] = 1;
	indices_[5] = 3;

	//左面。描画インデックスは[4,5,6][6,5,7]
	indices_[6] = 4;
	indices_[7] = 5;
	indices_[8] = 6;
	indices_[9] = 6;
	indices_[10] = 5;
	indices_[11] = 7;

	//前面。描画インデックスは[8,9,10][10,9,11]
	indices_[12] = 8;
	indices_[13] = 9;
	indices_[14] = 10;
	indices_[15] = 10;
	indices_[16] = 9;
	indices_[17] = 11;

	//後面。描画インデックスは[12,13,14][14,13,15]
	indices_[18] = 12;
	indices_[19] = 13;
	indices_[20] = 14;
	indices_[21] = 14;
	indices_[22] = 13;
	indices_[23] = 15;

	//上面。描画インデックスは[16,17,18][18,17,19]
	indices_[24] = 16;
	indices_[25] = 17;
	indices_[26] = 18;
	indices_[27] = 18;
	indices_[28] = 17;
	indices_[29] = 19;

	//下面。描画インデックスは[20,21,22][22,21,23]
	indices_[30] = 20;
	indices_[31] = 21;
	indices_[32] = 22;
	indices_[33] = 22;
	indices_[34] = 21;
	indices_[35] = 23;

	indexResource_ = dxCore_->CreateBufferResource(sizeof(uint32_t) * kMaxIndices);

	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * kMaxIndices);
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	uint32_t* indexData = nullptr;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	std::memcpy(indexData, indices_.data(), sizeof(uint32_t) * indices_.size());
	indexResource_->Unmap(0, nullptr);
}

void Skybox::CreateMaterialResource()
{
	materialResource_ = dxCore_->CreateBufferResource(sizeof(Vector4));

	UpdateMaterialResource();
}

void Skybox::UpdateMaterialResource()
{
	Vector4* materialData = nullptr;
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	*materialData = color_;
	materialResource_->Unmap(0, nullptr);
}
