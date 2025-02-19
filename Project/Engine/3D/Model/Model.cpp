/**
 * @file Model.cpp
 * @brief モデルの読み込み、生成、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "Model.h"

DirectXCore* Model::sDxCore_ = nullptr;
TextureManager* Model::sTextureManager_ = nullptr;
ID3D12Device* Model::sDevice_ = nullptr;
ID3D12GraphicsCommandList* Model::sCommandList_ = nullptr;
Microsoft::WRL::ComPtr<IDxcUtils> Model::sDxcUtils_ = nullptr;
Microsoft::WRL::ComPtr<IDxcCompiler3> Model::sDxcCompiler_ = nullptr;
Microsoft::WRL::ComPtr<IDxcIncludeHandler> Model::sIncludeHandler_ = nullptr;
Microsoft::WRL::ComPtr<ID3D12RootSignature> Model::sRootSignature_ = nullptr;
Microsoft::WRL::ComPtr<ID3D12PipelineState> Model::sGraphicsPipelineState_ = nullptr;
Microsoft::WRL::ComPtr<ID3D12RootSignature> Model::sBoneRootSignature_ = nullptr;
Microsoft::WRL::ComPtr<ID3D12PipelineState> Model::sBoneGraphicsPipelineState_ = nullptr;
std::list<ModelData> Model::sModelDatas_{};
uint32_t Model::sEnvironmentTextureHandle_;

void Model::StaticInitialize()
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

	//骨のPSOの作成
	CreateBonePSO();

	//環境テクスチャハンドルの設定
	sEnvironmentTextureHandle_ = sTextureManager_->LoadTexture("Resource/Images/Skybox.dds");
}

void Model::Update()
{
	//各ジョイントのローカル変換行列とスケルトン空間行列を計算
	for (Joint& joint : skeleton_.joints)
	{
		joint.localMatrix = MakeAffineMatrix(joint.scale, joint.rotate, joint.translate);

		if (joint.parent)
		{
			joint.skeletonSpaceMatrix = joint.localMatrix * skeleton_.joints[*joint.parent].skeletonSpaceMatrix;
		}
		else
		{
			joint.skeletonSpaceMatrix = joint.localMatrix;
		}
	}

	//各ジョイントのワールド変換行列を設定（ワールド行列はSkeletonSpaceの情報を基に作成）
	for (size_t i = 0; i < skeleton_.joints.size(); ++i)
	{
		if (i >= jointWorldTransform_.size())
		{
			jointWorldTransform_.resize(i + 1);
		}

		jointWorldTransform_[i].matWorld = skeleton_.joints[i].skeletonSpaceMatrix;
	}

	//SkeletonSpaceの情報を基に、SkinClusterのMatrixPaletteを更新する
	for (size_t jointIndex = 0; jointIndex < skeleton_.joints.size(); ++jointIndex)
	{
		assert(jointIndex < skinCluster_.inverseBindPoseMatrices.size());
		skinCluster_.mappedPalette[jointIndex].skeletonSpaceMatrix = skinCluster_.inverseBindPoseMatrices[jointIndex] * skeleton_.joints[jointIndex].skeletonSpaceMatrix;
		skinCluster_.mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix = Transpose(Inverse(skinCluster_.mappedPalette[jointIndex].skeletonSpaceMatrix));
	}
}

void Model::Draw(WorldTransform& worldTransform, const Camera& camera, const uint32_t animationData)
{
	//キーフレームのアニメーション
	if (isKeyframeAnimation_)
	{
		//animationTime_ += 1.0f / 60.0f;//時刻を進める。1/60で固定してあるが、計測した時間を使って可変フレーム対応する方が望ましい
		//animationTime_ = std::fmod(animationTime_, animation_.duration);//最後までいったら最初からリピート再生。リピートしなくても別にいい
		NodeAnimation& rootNodeAnimation = animation_[animationData].nodeAnimations[modelData_.rootNode.name];

		Vector3 translate{ 0.0f,0.0f,0.0f };
		Quaternion rotate{ 0.0f,0.0f,0.0f,1.0f };
		Vector3 scale = { 1.0f,1.0f,1.0f };

		if (!rootNodeAnimation.translate.keyframes.empty())
		{
			translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime_);
		}

		if (!rootNodeAnimation.rotate.keyframes.empty())
		{
			rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime_);
		}

		if (!rootNodeAnimation.scale.keyframes.empty())
		{
			scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime_);
		}

		Matrix4x4 localMatrix = MakeAffineMatrix(scale, rotate, translate);

		worldTransform.matWorld = Multiply(localMatrix, worldTransform.matWorld);
		worldTransform.TransferMatrix();
	}
	else
	{
		worldTransform.matWorld = Multiply(modelData_.rootNode.localMatrix, worldTransform.matWorld);
		worldTransform.TransferMatrix();
	}

	//マテリアルの更新
	material_->Update();

	//DirectionalLightの更新
	light_->Update();

	//PointLightの更新
	pointLight_->Update();

	//SpotLightの更新
	spotLight_->Update();

	//頂点データを設定
	mesh_->SetGraphicsCommand(skinCluster_.influenceBufferView);

	//マテリアルCBufferの場所を設定
	material_->SetGraphicsCommand(UINT(RootParameterIndex::Material));

	//WorldTransform用のCBufferの場所を設定
	sCommandList_->SetGraphicsRootConstantBufferView(UINT(RootParameterIndex::WorldTransform), worldTransform.constBuff->GetGPUVirtualAddress());

	//ViewProjection用のCBufferの場所を設定
	sCommandList_->SetGraphicsRootConstantBufferView(UINT(RootParameterIndex::ViewProjection), camera.constBuff_->GetGPUVirtualAddress());

	//DescriptorHeapを設定
	sTextureManager_->SetGraphicsDescriptorHeap();

	//DescriptorTableを設定
	sTextureManager_->SetGraphicsRootDescriptorTable(UINT(RootParameterIndex::Texture), textureHandle_);

	//Lightを設定
	light_->SetGraphicsCommand(UINT(RootParameterIndex::Light));

	//PointLightを設定
	pointLight_->SetGraphicsCommand(UINT(RootParameterIndex::PointLight));

	//SpotLightを設定
	spotLight_->SetGraphicsCommand(UINT(RootParameterIndex::SpotLight));

	sTextureManager_->SetGraphicsRootDescriptorTable(UINT(RootParameterIndex::Skinning), skinningTextureHandle_);

	sTextureManager_->SetGraphicsRootDescriptorTable(8, sEnvironmentTextureHandle_);

	//描画
	mesh_->Draw();
}

void Model::DrawBone(WorldTransform& worldTransform, const Camera& camera)
{
	/*worldTransform.matWorld = Multiply(modelData_.rootNode.localMatrix, worldTransform.matWorld);
	worldTransform.TransferMatrix();*/

	UpdateBoneVertices(skeleton_, skeleton_.root, boneVertices_);

	sDxCore_->GetCommandList()->IASetVertexBuffers(0, 1, &boneVertexBufferView_);

	sDxCore_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	//WorldTransform用のCBufferの場所を設定
	sCommandList_->SetGraphicsRootConstantBufferView(UINT(0), worldTransform.constBuff->GetGPUVirtualAddress());

	//ViewProjection用のCBufferの場所を設定
	sCommandList_->SetGraphicsRootConstantBufferView(UINT(1), camera.constBuff_->GetGPUVirtualAddress());

	//描画
	sDxCore_->GetCommandList()->DrawInstanced(UINT(boneVertices_.size()), 1, 0, 0);
}

void Model::Release()
{
	//解放
	sDxcUtils_.Reset();
	sDxcCompiler_.Reset();
	sIncludeHandler_.Reset();
	sRootSignature_.Reset();
	sGraphicsPipelineState_.Reset();
	sBoneRootSignature_.Reset();
	sBoneGraphicsPipelineState_.Reset();
}

Model* Model::CreateFromOBJ(const std::string& directoryPath, const std::string& filename)
{
	//新しいモデルを作成
	Model* model = new Model();

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
			model->modelData_ = modelData;
			modelExists = true;
			break;
		}
	}

	if (!modelExists)
	{
		//モデルデータを読み込む
		modelData = model->LoadModelFile(directoryPath, filename);
		modelData.name = filename;
		model->modelData_ = modelData;
		sModelDatas_.push_back(modelData);
	}

	//アニメーションの読み込み
	model->animation_ = model->LoadAnimationFile(directoryPath, filename);

	//スケルトンの生成
	model->skeleton_ = model->CreateSkelton(modelData.rootNode);

	//スキンクラスターの生成
	model->skinCluster_ = model->CreateSkinCluster(model->skeleton_, modelData);

	//骨
	if (!modelData.skinClusterData.empty())
	{
		model->CreateBoneVertices(model->skeleton_, model->skeleton_.root, model->boneVertices_);
		model->CreateBoneVertexBuffer();
	}

	//メッシュの作成
	model->mesh_ = std::make_unique<Mesh>();
	model->mesh_->Initialize(modelData.vertices, modelData.indices);

	//テクスチャのハンドルの取得
	model->textureHandle_ = sTextureManager_->LoadTexture(modelData.material.textureFilePath);

	//マテリアルの作成
	model->material_ = std::make_unique<Material>();
	model->material_->Initialize();

	//Lightの作成
	model->light_ = std::make_unique<Light>();
	model->light_->Initialize();

	//PointLightの作成
	model->pointLight_ = std::make_unique<PointLight>();
	model->pointLight_->Initialize();

	//SpotLightの作成
	model->spotLight_ = std::make_unique<SpotLight>();
	model->spotLight_->Initialize();

	return model;
}

void Model::PreDraw()
{
	//ルートシグネチャの設定
	sCommandList_->SetGraphicsRootSignature(sRootSignature_.Get());

	//パイプライン状態の設定
	sCommandList_->SetPipelineState(sGraphicsPipelineState_.Get());
}

void Model::PostDraw()
{

}

void Model::PreDrawBone()
{
	//ルートシグネチャの設定
	sCommandList_->SetGraphicsRootSignature(sBoneRootSignature_.Get());

	//パイプライン状態の設定
	sCommandList_->SetPipelineState(sBoneGraphicsPipelineState_.Get());
}

void Model::PostDrawBone()
{

}

void Model::InitializeDXC()
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

Microsoft::WRL::ComPtr<IDxcBlob> Model::CompileShader(const std::wstring& filePath, const wchar_t* profile)
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

void Model::CreatePSO()
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

	D3D12_DESCRIPTOR_RANGE skinningDescriptorRange[1] = {};
	skinningDescriptorRange[0].BaseShaderRegister = 0;//0から始まる
	skinningDescriptorRange[0].NumDescriptors = 1;//数は1つ
	skinningDescriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	skinningDescriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	D3D12_DESCRIPTOR_RANGE environmentDescriptorRange[1] = {};
	environmentDescriptorRange[0].BaseShaderRegister = 1;//1から始まる
	environmentDescriptorRange[0].NumDescriptors = 1;//数は1つ
	environmentDescriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	environmentDescriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	//RootParameter作成。複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameters[9] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVで使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0;//レジスタ番号0を使う
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	rootParameters[2].Descriptor.ShaderRegister = 1;//レジスタ番号1を使う
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[3].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
	rootParameters[3].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[4].Descriptor.ShaderRegister = 1;//レジスタ番号１を使う
	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[5].Descriptor.ShaderRegister = 2;//レジスタ番号2を使う
	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[6].Descriptor.ShaderRegister = 3;//レジスタ番号3を使う
	rootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[7].DescriptorTable.pDescriptorRanges = skinningDescriptorRange;//Tableの中身の配列を指定
	rootParameters[7].DescriptorTable.NumDescriptorRanges = _countof(skinningDescriptorRange);//Tableで利用する数
	rootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	rootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[8].DescriptorTable.pDescriptorRanges = environmentDescriptorRange;//Tableの中身の配列を指定
	rootParameters[8].DescriptorTable.NumDescriptorRanges = _countof(environmentDescriptorRange);//Tableで利用する数


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
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[5] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].InputSlot = 0;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].InputSlot = 0;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].InputSlot = 0;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[3].SemanticName = "WEIGHT";
	inputElementDescs[3].SemanticIndex = 0;
	inputElementDescs[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[3].InputSlot = 1;
	inputElementDescs[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[4].SemanticName = "INDEX";
	inputElementDescs[4].SemanticIndex = 0;
	inputElementDescs[4].Format = DXGI_FORMAT_R32G32B32A32_SINT;
	inputElementDescs[4].InputSlot = 1;
	inputElementDescs[4].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
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
	//半透明合成
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;//加算
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースのアルファ値
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f-ソースのアルファ値


	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


	//Shaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = CompileShader(L"Resource/Shaders/SkinningObject3d.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = CompileShader(L"Resource/Shaders/Object3d.PS.hlsl", L"ps_6_0");
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

void Model::CreateBonePSO()
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
	D3D12_ROOT_PARAMETER rootParameters[2] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //CBVで使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; //VertexShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0; //レジスタ番号0とバインド
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; //VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 1; //レジスタ番号1を使う

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
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&sBoneRootSignature_));
	assert(SUCCEEDED(hr));


	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[1] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].InputSlot = 0;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

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
	//半透明合成
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;//加算
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースのアルファ値
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f-ソースのアルファ値


	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


	//Shaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = CompileShader(L"Resource/Shaders/BoneVisualization.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = CompileShader(L"Resource/Shaders/BoneVisualization.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);


	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = false;
	//書き込みしない
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;


	//PSOを作成する
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = sBoneRootSignature_.Get();//RootSignature
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
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	//どのように画面に色を打ち込むかの設定(気にしなくて良い)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//実際に生成
	hr = sDevice_->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&sBoneGraphicsPipelineState_));
	assert(SUCCEEDED(hr));
}


ModelData Model::LoadModelFile(const std::string& directoryPath, const std::string& filename)
{
	ModelData modelData;
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes());//メッシュがないのは対応しない

	modelData.rootNode = ReadNode(scene->mRootNode);

	//Meshの解析
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
	{
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());//法線がないMeshは今回は非対応
		assert(mesh->HasTextureCoords(0));//TexcoordがないMeshは今回は非対応
		modelData.vertices.resize(mesh->mNumVertices);

		//ここからMeshの中身(Face)の解析を行っていく
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex)
		{
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];

			modelData.vertices[vertexIndex].position = { -position.x, position.y, position.z, 1.0f };
			modelData.vertices[vertexIndex].normal = { -normal.x, normal.y, normal.z };
			modelData.vertices[vertexIndex].texcoord = { texcoord.x, texcoord.y };
		}

		//ここからFaceの中身(Index)の解析を行っていく
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
		{
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);

			for (uint32_t element = 0; element < face.mNumIndices; ++element)
			{
				uint32_t vertexIndex = face.mIndices[element];
				modelData.indices.push_back(vertexIndex);
			}
		}

		//SkinCluster構築用のデータ取得
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
		{
			aiBone* bone = mesh->mBones[boneIndex];
			std::string jointName = bone->mName.C_Str();
			JointWeightData& jointWeightData = modelData.skinClusterData[jointName];

			aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
			aiVector3D scale, translate;
			aiQuaternion rotate;
			bindPoseMatrixAssimp.Decompose(scale, rotate, translate);

			Matrix4x4 bindPoseMatrix = MakeAffineMatrix({ scale.x,scale.y,scale.z },
				{ rotate.x,-rotate.y,-rotate.z,rotate.w }, { -translate.x,translate.y,translate.z });

			jointWeightData.inverseBindPoseMatrix = Inverse(bindPoseMatrix);

			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex)
			{
				jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight,bone->mWeights[weightIndex].mVertexId });
			}
		}
	}

	//Materialの解析
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex)
	{
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0)
		{
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			modelData.material.textureFilePath = directoryPath + "/" + textureFilePath.C_Str();
		}
	}

	return modelData;
}

MaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
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

std::vector<Animation> Model::LoadAnimationFile(const std::string& directoryPath, const std::string& filename)
{
	//アニメーションの読み込み
	std::vector<Animation> animation{};

	Assimp::Importer importer;

	std::string filePath = directoryPath + "/" + filename;

	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);

	if (scene->mNumAnimations != 0)
	{
		isKeyframeAnimation_ = true;

		for (uint32_t animationIndex = 0; animationIndex < scene->mNumAnimations; animationIndex++)
		{
			Animation animationData;
			aiAnimation* animationAssimp = scene->mAnimations[animationIndex];//最初のアニメーションだけ採用。もちろん複数対応することに越したことはない
			animationData.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);//時間の単位を秒に変換

			//assimpでは個々のNodeのAnimationをchannelと読んでいるのでchannelを回してNodeAnimationの情報を取ってくる
			for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex)
			{
				aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
				NodeAnimation& nodeAnimation = animationData.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];

				//Translate
				for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex)
				{
					aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
					KeyframeVector3 keyframe;
					keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);//ここも秒に変換
					keyframe.value = { -keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };//右手->左手
					nodeAnimation.translate.keyframes.push_back(keyframe);
				}
				//Rotate
				for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex)
				{
					aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
					KeyframeQuaternion keyframe;
					keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
					keyframe.value = { keyAssimp.mValue.x,-keyAssimp.mValue.y,-keyAssimp.mValue.z,keyAssimp.mValue.w };
					nodeAnimation.rotate.keyframes.push_back(keyframe);
				}
				//Scale
				for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex)
				{
					aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
					KeyframeVector3 keyframe;
					keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
					keyframe.value = { keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };
					nodeAnimation.scale.keyframes.push_back(keyframe);
				}
			}
			animation.push_back(animationData);
		}
	}
	else
	{
		isKeyframeAnimation_ = false;
	}

	return animation;
}

Node Model::ReadNode(aiNode* node)
{
	Node result;

	aiVector3D scale, translate;
	aiQuaternion rotate;

	node->mTransformation.Decompose(scale, rotate, translate);

	result.scale = { scale.x,scale.y,scale.z };
	result.rotate = { rotate.x,-rotate.y,-rotate.z,rotate.w };
	result.translate = { -translate.x,translate.y,translate.z };
	result.localMatrix = MakeAffineMatrix(result.scale, result.rotate, result.translate);

	//Node名を格納
	result.name = node->mName.C_Str();

	//子供の数だけ確保
	result.children.resize(node->mNumChildren);
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex)
	{
		//再帰的に読んで階層構造を作っていく
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}

	return result;
}

Vector3 Model::CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time)
{
	assert(!keyframes.empty());//キーがないものは返す値が分からないのでダメ
	//キーが2つか、時刻がキーフレーム前なら最初の値とする
	if (keyframes.size() == 1 || time <= keyframes[0].time)
	{
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size() - 1; ++index)
	{
		size_t nextIndex = index + 1;
		//indexとnextIndexの2つのkeyframeを取得して範囲内に時刻があるかを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time)
		{
			//範囲内を補間する
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}
	return (*keyframes.rbegin()).value;
}

Quaternion Model::CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time)
{
	assert(!keyframes.empty());//キーがないものは返す値が分からないのでダメ
	//キーが2つか、時刻がキーフレーム前なら最初の値とする
	if (keyframes.size() == 1 || time <= keyframes[0].time)
	{
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size() - 1; ++index)
	{
		size_t nextIndex = index + 1;
		//indexとnextIndexの2つのkeyframeを取得して範囲内に時刻があるかを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time)
		{
			//範囲内を補間する
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Slerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}
	return (*keyframes.rbegin()).value;
}

Skeleton Model::CreateSkelton(const Node& rootNode)
{
	//ジョイントの作成
	Skeleton skeleton;
	skeleton.root = CreateJoint(rootNode, {}, skeleton.joints);

	for (const Joint& joint : skeleton.joints)
	{
		skeleton.jointMap.emplace(joint.name, joint.index);
	}

	return skeleton;
}

int32_t Model::CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints)
{
	//Jointの生成
	Joint joint;
	joint.name = node.name;
	joint.localMatrix = node.localMatrix;
	joint.skeletonSpaceMatrix = MakeIdentity4x4();
	joint.translate = node.translate;
	joint.rotate = node.rotate;
	joint.scale = node.scale;
	joint.index = int32_t(joints.size());
	joint.parent = parent;
	joints.push_back(joint);

	for (const Node& child : node.children)
	{
		int32_t childIndex = CreateJoint(child, joint.index, joints);
		joints[joint.index].children.push_back(childIndex);
	}

	return joint.index;
}

void Model::ApplyAnimation(const uint32_t animationData)
{
	//アニメーションの適応
	if (animation_.size() != 0)
	{
		for (Joint& joint : skeleton_.joints)
		{
			if (auto it = animation_[animationData].nodeAnimations.find(joint.name); it != animation_[animationData].nodeAnimations.end())
			{
				const NodeAnimation& rootNodeAnimation = (*it).second;

				//Translation
				if (!rootNodeAnimation.translate.keyframes.empty())
				{
					joint.translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime_);
				}

				//Rotation
				if (!rootNodeAnimation.rotate.keyframes.empty())
				{
					joint.rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime_);
				}

				//Scale
				if (!rootNodeAnimation.scale.keyframes.empty())
				{
					joint.scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime_);
				}
			}
		}
	}


}

SkinCluster Model::CreateSkinCluster(const Skeleton& skeleton, const ModelData& modelData)
{
	//palette用のResourceを確保
	SkinCluster skinCluster;
	skinCluster.paletteResource = sDxCore_->CreateBufferResource(sizeof(WellForGPU) * skeleton.joints.size());
	WellForGPU* mappedPalette = nullptr;
	skinCluster.paletteResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
	skinCluster.mappedPalette = { mappedPalette, skeleton.joints.size() };
	skinningTextureHandle_ = sTextureManager_->CreateInstancingSRV(skinCluster.paletteResource, UINT(skeleton.joints.size()), sizeof(WellForGPU));

	//palette用のsrvを作成
	D3D12_SHADER_RESOURCE_VIEW_DESC paletteSrvDesc{};
	paletteSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	paletteSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	paletteSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	paletteSrvDesc.Buffer.FirstElement = 0;
	paletteSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	paletteSrvDesc.Buffer.NumElements = UINT(skeleton.joints.size());
	paletteSrvDesc.Buffer.StructureByteStride = sizeof(WellForGPU);

	//influence用のResourceを確保
	skinCluster.influenceResource = sDxCore_->CreateBufferResource(sizeof(VertexInfluence) * modelData.vertices.size());
	VertexInfluence* mappedInfluence = nullptr;
	skinCluster.influenceResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
	std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * modelData.vertices.size());
	skinCluster.mappedInfluence = { mappedInfluence, modelData.vertices.size() };

	//Influence用のVBVの作成
	skinCluster.influenceBufferView.BufferLocation = skinCluster.influenceResource->GetGPUVirtualAddress();
	skinCluster.influenceBufferView.SizeInBytes = UINT(sizeof(VertexInfluence) * modelData.vertices.size());
	skinCluster.influenceBufferView.StrideInBytes = sizeof(VertexInfluence);

	//InverseBindPoseMatrixを格納する場所を作成して、単位行列で埋める
	skinCluster.inverseBindPoseMatrices.resize(skeleton.joints.size());

	for (Matrix4x4& inverseBindPoseMatrix : skinCluster.inverseBindPoseMatrices)
	{
		inverseBindPoseMatrix = MakeIdentity4x4();
	}

	//ModelDataを解析してInfluenceを埋める
	for (const auto& jointWeight : modelData.skinClusterData)
	{
		auto it = skeleton.jointMap.find(jointWeight.first);
		if (it == skeleton.jointMap.end())
		{
			continue;
		}

		//(it).secondにはjointのindexが入っているので、該当のindexのinverseBindPoseMatrixを代入
		skinCluster.inverseBindPoseMatrices[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
		for (const auto& vertexWeight : jointWeight.second.vertexWeights)
		{
			auto& currentInfluence = skinCluster.mappedInfluence[vertexWeight.vertexIndex];
			for (uint32_t index = 0; index < kNumMaxInfluence; ++index)
			{
				if (currentInfluence.weights[index] == 0.0f)
				{
					currentInfluence.weights[index] = vertexWeight.weight;
					currentInfluence.jointIndices[index] = (*it).second;
					break;
				}
			}
		}
	}

	return skinCluster;
}

void Model::CreateBoneVertexBuffer()
{
	//骨用の頂点バッファを作成
	boneVertexBuffer_ = sDxCore_->CreateBufferResource(sizeof(Vector4) * boneVertices_.size());

	///骨用の頂点バッファビューの設定
	boneVertexBufferView_.BufferLocation = boneVertexBuffer_->GetGPUVirtualAddress();
	boneVertexBufferView_.SizeInBytes = UINT(sizeof(Vector4) * boneVertices_.size());
	boneVertexBufferView_.StrideInBytes = sizeof(Vector4);

	//データを転送
	Vector4* vertexData = nullptr;
	boneVertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, boneVertices_.data(), sizeof(Vector4) * boneVertices_.size());
	boneVertexBuffer_->Unmap(0, nullptr);
}


void Model::CreateBoneVertices(const Skeleton& skeleton, int32_t index, std::vector<Vector4>& vertices)
{
	//現在のジョイント(親ジョイント)を取得
	const Joint& parentJoint = skeleton.joints[index];

	//子ジョイントに対して処理を行う
	for (int32_t childIndex : parentJoint.children)
	{
		const Joint& childJoint = skeleton.joints[childIndex];
		vertices.push_back({ parentJoint.skeletonSpaceMatrix.m[3][0], parentJoint.skeletonSpaceMatrix.m[3][1], parentJoint.skeletonSpaceMatrix.m[3][2],1.0f });
		vertices.push_back({ childJoint.skeletonSpaceMatrix.m[3][0], childJoint.skeletonSpaceMatrix.m[3][1], childJoint.skeletonSpaceMatrix.m[3][2],1.0f });
		CreateBoneVertices(skeleton, childIndex, vertices);
	}
}

void Model::UpdateBoneVertices(const Skeleton& skeleton, int32_t index, std::vector<Vector4>& vertices)
{
	//以前の頂点データをクリア
	vertices.clear();

	//現在のジョイント(親ジョイント)を取得
	const Joint& parentJoint = skeleton.joints[index];

	//子ジョイントに対して処理を行う
	for (int32_t childIndex : parentJoint.children)
	{
		const Joint& childJoint = skeleton.joints[childIndex];
		vertices.push_back({ parentJoint.skeletonSpaceMatrix.m[3][0], parentJoint.skeletonSpaceMatrix.m[3][1], parentJoint.skeletonSpaceMatrix.m[3][2],1.0f });
		vertices.push_back({ childJoint.skeletonSpaceMatrix.m[3][0], childJoint.skeletonSpaceMatrix.m[3][1], childJoint.skeletonSpaceMatrix.m[3][2],1.0f });
		CreateBoneVertices(skeleton, childIndex, vertices);
	}

	Vector4* vertexData = nullptr;
	boneVertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, vertices.data(), sizeof(Vector4) * vertices.size());
	boneVertexBuffer_->Unmap(0, nullptr);
}

WorldTransform& Model::GetJointWorldTransform(const std::string& jointName)
{
	jointWorldTransform_.resize(skeleton_.joints.size());

	for (Joint& joint : skeleton_.joints)
	{
		if (joint.name == jointName)
		{
			//インデックスが範囲内か確認
			if (joint.index >= 0 && joint.index < jointWorldTransform_.size())
			{
				return jointWorldTransform_[joint.index];
			}
			else
			{
				return defaultTransform_;
			}
		}
	}

	//ジョイントが見つからなかった場合デフォルトを返す
	return defaultTransform_;
}