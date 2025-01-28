/**
 * @file ParticleModel.cpp
 * @brief パーティクルモデルの読み込み、生成、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "ParticleModel.h"

DirectXCore* ParticleModel::sDxCore_ = nullptr;
Engine::TextureManager* ParticleModel::sTextureManager_ = nullptr;
ID3D12Device* ParticleModel::sDevice_ = nullptr;
ID3D12GraphicsCommandList* ParticleModel::sCommandList_ = nullptr;
Microsoft::WRL::ComPtr<IDxcUtils> ParticleModel::sDxcUtils_ = nullptr;
Microsoft::WRL::ComPtr<IDxcCompiler3> ParticleModel::sDxcCompiler_ = nullptr;
Microsoft::WRL::ComPtr<IDxcIncludeHandler> ParticleModel::sIncludeHandler_ = nullptr;
Microsoft::WRL::ComPtr<ID3D12RootSignature> ParticleModel::sRootSignature_ = nullptr;
Microsoft::WRL::ComPtr<ID3D12PipelineState> ParticleModel::sGraphicsPipelineState_ = nullptr;
std::list<ParticleModel::ModelData> ParticleModel::sModelDatas_{};

void ParticleModel::StaticInitialize()
{
	//DirectXCoreのインスタンスの取得
	sDxCore_ = DirectXCore::GetInstance();

	//TextureManagerのインスタンスの取得
	sTextureManager_ = Engine::TextureManager::GetInstance();

	//デバイスの取得
	sDevice_ = sDxCore_->GetDevice();

	//コマンドリストの取得
	sCommandList_ = sDxCore_->GetCommandList();

	//DXCの初期化
	InitializeDXC();

	//PSOの作成
	CreatePSO();
}

void ParticleModel::Draw(const ParticleSystem* particleSystem, const Camera& camera)
{
	//DescriptorHeapを設定
	sTextureManager_->SetGraphicsDescriptorHeap();

	//VBVを設定
	sCommandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);

	//形状を設定
	sCommandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//マテリアルCBufferの場所を設定
	sCommandList_->SetGraphicsRootConstantBufferView(UINT(RootParameterIndex::Material), materialResource_->GetGPUVirtualAddress());

	//WorldTransform用のCBufferの場所を設定
	sTextureManager_->SetGraphicsRootDescriptorTable(UINT(RootParameterIndex::WorldTransform), particleSystem->GetSrvIndex());

	//ViewProjection用のCBufferの場所を設定
	sCommandList_->SetGraphicsRootConstantBufferView(UINT(RootParameterIndex::ViewProjection), camera.constBuff_->GetGPUVirtualAddress());

	//DescriptorTableを設定
	sTextureManager_->SetGraphicsRootDescriptorTable(UINT(RootParameterIndex::Texture), textureHandle_);

	//描画
	sCommandList_->DrawInstanced(UINT(vertices_.size()), particleSystem->GetNumInstance(), 0, 0);
}

void ParticleModel::Release()
{
	//解放
	sDxcUtils_.Reset();
	sDxcCompiler_.Reset();
	sIncludeHandler_.Reset();
	sRootSignature_.Reset();
	sGraphicsPipelineState_.Reset();
}

ParticleModel* ParticleModel::CreateFromOBJ(const std::string& directoryPath, const std::string& filename)
{
	//モデルを生成
	ParticleModel* particleModel = new ParticleModel();

	//モデルが既に存在するかチェック
	bool modelExists = false;

	//モデルデータを読み込む
	ModelData modelData;

	for (ModelData existingModelData : sModelDatas_)
	{
		if (existingModelData.name == filename)
		{
			//既に存在する場合はそのモデルを使う
			modelData = existingModelData;
			modelExists = true;
			break;
		}
	}

	if (!modelExists)
	{
		//モデルデータを読み込む
		modelData = particleModel->LoadObjFile(directoryPath, filename);
		modelData.name = filename;
		sModelDatas_.push_back(modelData);
	}

	particleModel->Initialize(modelData);
	return particleModel;
}

void ParticleModel::PreDraw()
{
	//ルートシグネチャの設定
	sCommandList_->SetGraphicsRootSignature(sRootSignature_.Get());

	//パイプライン状態の設定
	sCommandList_->SetPipelineState(sGraphicsPipelineState_.Get());
}

void ParticleModel::PostDraw()
{

}

void ParticleModel::InitializeDXC()
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

Microsoft::WRL::ComPtr<IDxcBlob> ParticleModel::CompileShader(const std::wstring& filePath, const wchar_t* profile)
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


void ParticleModel::CreatePSO()
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

	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
	descriptorRangeForInstancing[0].BaseShaderRegister = 0;//0から始まる
	descriptorRangeForInstancing[0].NumDescriptors = 1;//数は一つ
	descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//RootParameter作成
	D3D12_ROOT_PARAMETER rootParameters[4] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVで使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing;//Tableの中身の配列を指定
	rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing);//Tableで利用する数
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	rootParameters[2].Descriptor.ShaderRegister = 1;//レジスタ番号1を使う
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[3].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
	rootParameters[3].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数
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
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);


	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//共通設定
	blendDesc.RenderTarget[0].BlendEnable = true;//ブレンドを有効にする
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う

	////半透明合成
	//blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;//加算
	//blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースのアルファ値
	//blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f-ソースのアルファ値

	//加算合成
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;//加算
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースの値を100%使う
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;//デストの値を100%使う

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


	//Shaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = CompileShader(L"Resource/Shaders/Particle.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = CompileShader(L"Resource/Shaders/Particle.PS.hlsl", L"ps_6_0");
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
	graphicsPipelineStateDesc.pRootSignature = sRootSignature_.Get();//RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;//InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };//VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };//PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;//BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 2;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	graphicsPipelineStateDesc.RTVFormats[1] = DXGI_FORMAT_R32_FLOAT;
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
	hr = sDevice_->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&sGraphicsPipelineState_));
	assert(SUCCEEDED(hr));
}


void ParticleModel::Initialize(const ModelData& modelData)
{
	//頂点データの取得
	vertices_ = modelData.vertices;

	//頂点リソースの作成
	CreateVertexResource();

	//マテリアル用のリソースの作成
	CreateMaterialResource();

	//テクスチャの読み込み
	textureHandle_ = sTextureManager_->LoadTexture(modelData.material.textureFilePath);
}


void ParticleModel::CreateVertexResource()
{
	//頂点バッファリソースを作成
	vertexResource_ = sDxCore_->CreateBufferResource(sizeof(VertexData) * vertices_.size());

	//頂点バッファビューの設定
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * vertices_.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//データを転送
	VertexData* vertexData = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, vertices_.data(), sizeof(VertexData) * vertices_.size());
	vertexResource_->Unmap(0, nullptr);
}


void ParticleModel::CreateMaterialResource()
{
	//マテリアル用のバッファリソースを作成
	materialResource_ = sDxCore_->CreateBufferResource(sizeof(ConstBuffDataMaterial));

	//データを転送
	ConstBuffDataMaterial* materialData = nullptr;
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	materialData->color = Vector4{ 1.0f,1.0f,1.0f,1.0f };
	materialData->uvTransform = MakeIdentity4x4();
	materialResource_->Unmap(0, nullptr);
}


ParticleModel::ModelData ParticleModel::LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
	ModelData modelData;
	std::vector<Vector4> positions;
	std::vector<Vector3> normals;
	std::vector<Vector2> texcoords;
	std::string line;

	//OBJファイルを開く
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());


	//ファイルの各行を読み込む
	while (std::getline(file, line))
	{
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		//頂点位置の読み込み
		if (identifier == "v")
		{
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.z *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);
		}
		//テクスチャ座標の読み込み
		else if (identifier == "vt")
		{
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		}
		//法線ベクトルの読み込み
		else if (identifier == "vn")
		{
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.z *= -1.0f;
			normals.push_back(normal);
		}
		//面情報(フェイス)の読み込み
		else if (identifier == "f")
		{
			VertexData triangle[3];
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex)
			{
				std::string vertexDefinition;
				s >> vertexDefinition;
				std::istringstream v(vertexDefinition);

				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element)
				{
					std::string index;
					std::getline(v, index, '/');
					elementIndices[element] = std::stoi(index);
				}

				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				triangle[faceVertex] = { position,texcoord,normal };
			}


			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		}
		//マテリアルテンプレートファイルの読み込み
		else if (identifier == "mtllib")
		{
			std::string materialFilename;
			s >> materialFilename;
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}
	return modelData;
}


ParticleModel::MaterialData ParticleModel::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
{
	//マテリアルデータの読み込み
	MaterialData materialData;
	std::string line;
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	while (std::getline(file, line))
	{
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		if (identifier == "map_Kd")
		{
			std::string textureFilename;
			s >> textureFilename;
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}
	return materialData;
}

