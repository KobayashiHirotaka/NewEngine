/**
 * @file PostProcess.cpp
 * @brief ポストプロセスエフェクトの管理(ブルーム、ビネット、グレースケールなど)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "PostProcess.h"

uint32_t PostProcess::descriptorSizeRTV;
uint32_t PostProcess::descriptorSizeSRV;
uint32_t PostProcess::descriptorSizeDSV;
PostProcess* PostProcess::instance_ = nullptr;

PostProcess* PostProcess::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new PostProcess();
	}
	return instance_;
}

void PostProcess::DeleteInstance()
{
	if (instance_ != nullptr)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

void PostProcess::Initialize()
{
	dxCore_ = DirectXCore::GetInstance();

	commandList_ = dxCore_->GetCommandList();

	device_ = dxCore_->GetDevice();

	descriptorSizeRTV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	descriptorSizeSRV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	descriptorSizeDSV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	// 頂点情報の設定
	vertices_.push_back(VertexPosUV{ {-1.0f,-1.0f,1.0,1.0f},{0.0f,1.0f} });
	vertices_.push_back(VertexPosUV{ {-1.0f,1.0f,1.0f,1.0f},{0.0f,0.0f} });
	vertices_.push_back(VertexPosUV{ {1.0f,-1.0f,1.0f,1.0f},{1.0f,1.0f} });
	vertices_.push_back(VertexPosUV{ {-1.0f,1.0f,1.0f,1.0f},{0.0f,0.0f} });
	vertices_.push_back(VertexPosUV{ {1.0f,1.0f,1.0f,1.0f},{1.0f,0.0f} });
	vertices_.push_back(VertexPosUV{ {1.0f,-1.0f,1.0f,1.0f},{1.0f,1.0f} });

	InitializeVertexBuffer();
	InitializeDXC();
	CreatePSO();
	CreateBlurPSO();
	CreatePostProcessPSO();

	multiPassRTVDescriptorHeap_ = dxCore_->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 14, false);
	multiPassSRVDescriptorHeap_ = dxCore_->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 14, true);
	multiPassDSVDescriptorHeap_ = dxCore_->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	//深度テクスチャの作成
	depthStencilResource_ = CreateDepthStencilTextureResource(WindowsApp::GetInstance()->kClientWidth, WindowsApp::GetInstance()->kClientHeight);

	projectionInverse_ = Inverse(MakePerspectiveFovMatrix(0.45f, (float)16 / 9, 1.0f, 300.0f));

	CreateDSV();

	CreateRenderTargets();

	SetupBlurConstantBuffers();

	//各ポストエフェクトの初期化
	Bloom();

	Vignette();

	GrayScale();

	BoxFilter();

	GaussianFilter();

	LuminanceBasedOutline();

	DepthBasedOutline();

	HSVFilter();
}

void PostProcess::Update()
{
	if (isPostProcessActive_ == false)
	{
		return;
	}

	//各ポストエフェクトの更新
	UpdateBloom();

	UpdateVignette();

	UpdateGrayScale();

	UpdateBoxFilter();

	UpdateGaussianFilter();

	UpdateLuminanceBasedOutline();

	UpdateDepthBasedOutline();

	UpdateHSVFilter();
}

void PostProcess::PreDraw()
{
	if (isPostProcessActive_ == false)
	{
		return;
	}

	// レンダーターゲットへのバリア
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = firstPassResource_.resource.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	commandList_->ResourceBarrier(1, &barrier);

	barrier.Transition.pResource = linearDepthResource_.resource.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	commandList_->ResourceBarrier(1, &barrier);

	// レンダーターゲットの設定
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	rtvHandles[0] = PostProcess::GetCPUDescriptorHandle(multiPassRTVDescriptorHeap_.Get(), descriptorSizeRTV, firstPassResource_.rtvIndex);
	rtvHandles[1] = PostProcess::GetCPUDescriptorHandle(multiPassRTVDescriptorHeap_.Get(), descriptorSizeRTV, linearDepthResource_.rtvIndex);
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = multiPassDSVDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	commandList_->OMSetRenderTargets(2, rtvHandles, false, &dsvHandle);

	// レンダーターゲットのクリア
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };
	float depthColor[] = { 1.0f,0.0f,0.0f,0.0f };
	commandList_->ClearRenderTargetView(rtvHandles[0], clearColor, 0, nullptr);
	commandList_->ClearRenderTargetView(rtvHandles[1], depthColor, 0, nullptr);
	commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// ビューポートの設定
	D3D12_VIEWPORT viewport{};
	viewport.Width = FLOAT(WindowsApp::GetInstance()->kClientWidth);
	viewport.Height = FLOAT(WindowsApp::GetInstance()->kClientHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	commandList_->RSSetViewports(1, &viewport);

	// シザー矩形の設定
	D3D12_RECT scissorRect{};
	scissorRect.left = 0;
	scissorRect.right = WindowsApp::GetInstance()->kClientWidth;
	scissorRect.top = 0;
	scissorRect.bottom = WindowsApp::GetInstance()->kClientHeight;
	commandList_->RSSetScissorRects(1, &scissorRect);
}

// 描画後の処理
void PostProcess::PostDraw()
{
	if (isPostProcessActive_ == false)
	{
		return;
	}

	// バリアを張る
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = firstPassResource_.resource.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	commandList_->ResourceBarrier(1, &barrier);

	barrier.Transition.pResource = linearDepthResource_.resource.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	commandList_->ResourceBarrier(1, &barrier);

	// 2パス目の描画
	PreSecondPassDraw();
	SecondPassDraw();
	PostSecondPassDraw();

	// 各種Blur処理
	if (isBlurActive_)
	{
		PreBlur(BlurState::Horizontal);
		Blur(BlurState::Horizontal, secondPassResource_.srvIndex, highIntensityResource_.srvIndex);
		PostBlur(BlurState::Horizontal);

		PreBlur(BlurState::Vertical);
		Blur(BlurState::Vertical, blurResources_[static_cast<int>(BlurState::Horizontal)].srvIndex, highIntensityBlurResource_[static_cast<int>(BlurState::Horizontal)].srvIndex);
		PostBlur(BlurState::Vertical);
	}

	if (isShrinkBlurActive_)
	{
		PreShrinkBlur(BlurState::Horizontal);
		ShrinkBlur(BlurState::Horizontal, blurResources_[static_cast<int>(BlurState::Vertical)].srvIndex, highIntensityBlurResource_[static_cast<int>(BlurState::Vertical)].srvIndex);
		PostShrinkBlur(BlurState::Horizontal);

		PreShrinkBlur(BlurState::Vertical);
		ShrinkBlur(BlurState::Vertical, shrinkBlurResources_[static_cast<int>(BlurState::Horizontal)].srvIndex, shrinkHighIntensityBlurResources_[static_cast<int>(BlurState::Horizontal)].srvIndex);
		PostShrinkBlur(BlurState::Vertical);
	}

	// バックバッファに戻す
	dxCore_->SetBackBuffer();

	// 描画
	Draw();
}

void PostProcess::InitializeDXC()
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

Microsoft::WRL::ComPtr<IDxcBlob> PostProcess::CompileShader(const std::wstring& filePath, const wchar_t* profile)
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

void PostProcess::InitializeVertexBuffer()
{
	vertexResource_ = dxCore_->CreateBufferResource(sizeof(VertexPosUV) * vertices_.size());

	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexPosUV) * vertices_.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexPosUV);

	VertexPosUV* vertexData = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, vertices_.data(), sizeof(VertexPosUV) * vertices_.size());
	vertexResource_->Unmap(0, nullptr);
}

void PostProcess::CreateRenderTargets()
{
	//リソースの作成
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };
	float depthColor[] = { 1.0f,0.0f,0.0f,0.0f };
	firstPassResource_.resource = CreateTextureResource(WindowsApp::GetInstance()->kClientWidth, WindowsApp::GetInstance()->kClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor);
	linearDepthResource_.resource = CreateTextureResource(WindowsApp::GetInstance()->kClientWidth, WindowsApp::GetInstance()->kClientHeight, DXGI_FORMAT_R32_FLOAT, depthColor);
	secondPassResource_.resource = CreateTextureResource(WindowsApp::GetInstance()->kClientWidth, WindowsApp::GetInstance()->kClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor);
	highIntensityResource_.resource = CreateTextureResource(WindowsApp::GetInstance()->kClientWidth, WindowsApp::GetInstance()->kClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor);
	blurResources_[0].resource = CreateTextureResource(WindowsApp::GetInstance()->kClientWidth, WindowsApp::GetInstance()->kClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor);
	blurResources_[1].resource = CreateTextureResource(WindowsApp::GetInstance()->kClientWidth, WindowsApp::GetInstance()->kClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor);
	highIntensityBlurResource_[0].resource = CreateTextureResource(WindowsApp::GetInstance()->kClientWidth, WindowsApp::GetInstance()->kClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor);
	highIntensityBlurResource_[1].resource = CreateTextureResource(WindowsApp::GetInstance()->kClientWidth, WindowsApp::GetInstance()->kClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor);
	shrinkBlurResources_[0].resource = CreateTextureResource(WindowsApp::GetInstance()->kClientWidth / 2, WindowsApp::GetInstance()->kClientHeight / 2, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor);
	shrinkBlurResources_[1].resource = CreateTextureResource(WindowsApp::GetInstance()->kClientWidth / 2, WindowsApp::GetInstance()->kClientHeight / 2, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor);
	shrinkHighIntensityBlurResources_[0].resource = CreateTextureResource(WindowsApp::GetInstance()->kClientWidth / 2, WindowsApp::GetInstance()->kClientHeight / 2, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor);
	shrinkHighIntensityBlurResources_[1].resource = CreateTextureResource(WindowsApp::GetInstance()->kClientWidth / 2, WindowsApp::GetInstance()->kClientHeight / 2, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor);

	//レンダーターゲットビュー
	firstPassResource_.rtvIndex = CreateRTV(firstPassResource_.resource, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	linearDepthResource_.rtvIndex = CreateRTV(linearDepthResource_.resource, DXGI_FORMAT_R32_FLOAT);
	secondPassResource_.rtvIndex = CreateRTV(secondPassResource_.resource, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	highIntensityResource_.rtvIndex = CreateRTV(highIntensityResource_.resource, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	blurResources_[0].rtvIndex = CreateRTV(blurResources_[0].resource, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	blurResources_[1].rtvIndex = CreateRTV(blurResources_[1].resource, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	highIntensityBlurResource_[0].rtvIndex = CreateRTV(highIntensityBlurResource_[0].resource, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	highIntensityBlurResource_[1].rtvIndex = CreateRTV(highIntensityBlurResource_[1].resource, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	shrinkBlurResources_[0].rtvIndex = CreateRTV(shrinkBlurResources_[0].resource, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	shrinkBlurResources_[1].rtvIndex = CreateRTV(shrinkBlurResources_[1].resource, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	shrinkHighIntensityBlurResources_[0].rtvIndex = CreateRTV(shrinkHighIntensityBlurResources_[0].resource, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	shrinkHighIntensityBlurResources_[1].rtvIndex = CreateRTV(shrinkHighIntensityBlurResources_[1].resource, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);

	//シェーダーリソースビューの作成
	firstPassResource_.srvIndex = CreateSRV(firstPassResource_.resource, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	linearDepthResource_.srvIndex = CreateSRV(linearDepthResource_.resource, DXGI_FORMAT_R32_FLOAT);
	secondPassResource_.srvIndex = CreateSRV(secondPassResource_.resource, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	highIntensityResource_.srvIndex = CreateSRV(highIntensityResource_.resource, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	blurResources_[0].srvIndex = CreateSRV(blurResources_[0].resource, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	blurResources_[1].srvIndex = CreateSRV(blurResources_[1].resource, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	highIntensityBlurResource_[0].srvIndex = CreateSRV(highIntensityBlurResource_[0].resource, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	highIntensityBlurResource_[1].srvIndex = CreateSRV(highIntensityBlurResource_[1].resource, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	shrinkBlurResources_[0].srvIndex = CreateSRV(shrinkBlurResources_[0].resource, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	shrinkBlurResources_[1].srvIndex = CreateSRV(shrinkBlurResources_[1].resource, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	shrinkHighIntensityBlurResources_[0].srvIndex = CreateSRV(shrinkHighIntensityBlurResources_[0].resource, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	shrinkHighIntensityBlurResources_[1].srvIndex = CreateSRV(shrinkHighIntensityBlurResources_[1].resource, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
}

void PostProcess::SetupBlurConstantBuffers()
{
	//ブラー用のCBVの作成
	blurConstantBuffer_ = dxCore_->CreateBufferResource(sizeof(BlurData));

	//ブラー用のCBVに書き込む
	BlurData* blurData = nullptr;
	blurConstantBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&blurData));
	blurData->textureWidth = WindowsApp::GetInstance()->kClientWidth;
	blurData->textureHeight = WindowsApp::GetInstance()->kClientHeight;

	float sigma = 5.0f;
	float total = 0.0f;
	for (int i = 0; i < 8; i++)
	{
		blurData->weights[i] = expf(-(i * i) / (2 * sigma * sigma));
		total += blurData->weights[i];
	}

	total = total * 2.0f - 1.0f;

	//最終的な合計値で重みをわる
	for (int i = 0; i < 8; i++)
	{
		blurData->weights[i] /= total;
	}


	blurConstantBuffer_->Unmap(0, nullptr);

	//縮小ぼかし用のCBVの作成
	shrinkBlurConstantBuffer_ = dxCore_->CreateBufferResource(sizeof(BlurData));

	//ブラー用のCBVに書き込む
	BlurData* shrinkBlurData = nullptr;
	shrinkBlurConstantBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&shrinkBlurData));
	shrinkBlurData->textureWidth = WindowsApp::GetInstance()->kClientWidth / 2;
	shrinkBlurData->textureHeight = WindowsApp::GetInstance()->kClientHeight / 2;

	for (int i = 0; i < 8; i++)
	{
		shrinkBlurData->weights[i] = blurData->weights[i];
	}

	shrinkBlurConstantBuffer_->Unmap(0, nullptr);
}

void PostProcess::CreatePSO()
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
	D3D12_ROOT_PARAMETER rootParameters[1] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
	rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数
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
	hr = device_->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
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

	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//Shaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = CompileShader(L"resource/shaders/MultiPass.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = CompileShader(L"resource/shaders/MultiPass.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

	//PSOを作成する
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();//RootSignature
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
	graphicsPipelineStateDesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//利用するトポロジ(形状)のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//どのように画面に色を打ち込むかの設定(気にしなくて良い)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//実際に生成
	hr = device_->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(hr));
}


void PostProcess::CreateBlurPSO()
{
	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//DescriptorRange作成
	D3D12_DESCRIPTOR_RANGE descriptorRange[2] = {};
	descriptorRange[0].BaseShaderRegister = 0;//0から始まる
	descriptorRange[0].NumDescriptors = 1;//数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算
	descriptorRange[1].BaseShaderRegister = 1;//0から始まる
	descriptorRange[1].NumDescriptors = 1;//数は1つ
	descriptorRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	//RootParameter作成。複数設定できるので配列。今回は結果一つだけなので長さ1の配列
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[0].DescriptorTable.pDescriptorRanges = &descriptorRange[0];//Tableの中身の配列を指定
	rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;//Tableで利用する数
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[1].DescriptorTable.pDescriptorRanges = &descriptorRange[1];//Tableの中身の配列を指定
	rootParameters[1].DescriptorTable.NumDescriptorRanges = 1;//Tableで利用する数
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].Descriptor.ShaderRegister = 0;
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
	hr = device_->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&blurRootSignature_));
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
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//シェーダーをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = CompileShader(L"resource/shaders/HorizontalBlur.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = CompileShader(L"resource/shaders/HorizontalBlur.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

	//PSOを作成する
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = blurRootSignature_.Get();//RootSignature
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
	graphicsPipelineStateDesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//利用するトポロジ(形状)のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//どのように画面に色を打ち込むかの設定(気にしなくて良い)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//実際に生成
	hr = device_->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&blurPipelineState_[static_cast<int>(BlurState::Horizontal)]));
	assert(SUCCEEDED(hr));

	//シェーダーをコンパイルする
	vertexShaderBlob = CompileShader(L"resource/shaders/VerticalBlur.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	pixelShaderBlob = CompileShader(L"resource/shaders/VerticalBlur.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

	//PSOを再設定
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };//VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };//PixelShader

	//実際に生成
	hr = device_->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&blurPipelineState_[static_cast<int>(BlurState::Vertical)]));
	assert(SUCCEEDED(hr));
}


void PostProcess::CreatePostProcessPSO()
{
	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//DescriptorRange作成
	D3D12_DESCRIPTOR_RANGE descriptorRange[7]{};
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算
	descriptorRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[1].BaseShaderRegister = 1;
	descriptorRange[1].NumDescriptors = 2;
	descriptorRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算
	descriptorRange[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[2].BaseShaderRegister = 3;
	descriptorRange[2].NumDescriptors = 1;
	descriptorRange[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算
	descriptorRange[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[3].BaseShaderRegister = 4;
	descriptorRange[3].NumDescriptors = 1;
	descriptorRange[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算
	descriptorRange[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[4].BaseShaderRegister = 5;
	descriptorRange[4].NumDescriptors = 1;
	descriptorRange[4].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算
	descriptorRange[5].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[5].BaseShaderRegister = 6;
	descriptorRange[5].NumDescriptors = 1;
	descriptorRange[5].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	//RootParameter作成。複数設定できるので配列。今回は結果一つだけなので長さ1の配列
	D3D12_ROOT_PARAMETER rootParameters[14] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[0].DescriptorTable.pDescriptorRanges = &descriptorRange[0];//Tableの中身の配列を指定
	rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;//Tableで利用する数
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[1].DescriptorTable.pDescriptorRanges = &descriptorRange[1];//Tableの中身の配列を指定
	rootParameters[1].DescriptorTable.NumDescriptorRanges = 1;//Tableで利用する数
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = &descriptorRange[2];//Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = 1;//Tableで利用する数
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[3].DescriptorTable.pDescriptorRanges = &descriptorRange[3];//Tableの中身の配列を指定
	rootParameters[3].DescriptorTable.NumDescriptorRanges = 1;//Tableで利用する数
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[4].DescriptorTable.pDescriptorRanges = &descriptorRange[4];//Tableの中身の配列を指定
	rootParameters[4].DescriptorTable.NumDescriptorRanges = 1;//Tableで利用する数
	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[5].DescriptorTable.pDescriptorRanges = &descriptorRange[5];//Tableの中身の配列を指定
	rootParameters[5].DescriptorTable.NumDescriptorRanges = 1;//Tableで利用する数
	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderを使う
	rootParameters[6].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド
	rootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderを使う
	rootParameters[7].Descriptor.ShaderRegister = 1;//レジスタ番号1とバインド
	rootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderを使う
	rootParameters[8].Descriptor.ShaderRegister = 2;//レジスタ番号2とバインド
	rootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderを使う
	rootParameters[9].Descriptor.ShaderRegister = 3;//レジスタ番号3とバインド
	rootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderを使う
	rootParameters[10].Descriptor.ShaderRegister = 4;//レジスタ番号4とバインド
	rootParameters[11].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[11].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderを使う
	rootParameters[11].Descriptor.ShaderRegister = 5;//レジスタ番号5とバインド
	rootParameters[12].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[12].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderを使う
	rootParameters[12].Descriptor.ShaderRegister = 6;//レジスタ番号6とバインド
	rootParameters[13].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[13].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderを使う
	rootParameters[13].Descriptor.ShaderRegister = 7;//レジスタ番号7とバインド
	descriptionRootSignature.pParameters = rootParameters;//ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);//配列の長さ

	//Sampler作成
	D3D12_STATIC_SAMPLER_DESC staticSamplers[2] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;//レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う

	staticSamplers[1].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;//ポイントフィルタ
	staticSamplers[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
	staticSamplers[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[1].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSamplers[1].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
	staticSamplers[1].ShaderRegister = 1;//レジスタ番号1を使う
	staticSamplers[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
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
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&postProcessRootSignature_));
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
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//シェーダーをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = CompileShader(L"resource/shaders/PostProcess.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = CompileShader(L"resource/shaders/PostProcess.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化
	depthStencilDesc.DepthEnable = true;
	//書き込み
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	//PSOを作成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = postProcessRootSignature_.Get();//RootSignature
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
	hr = device_->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&postProcessPipelineState_));
	assert(SUCCEEDED(hr));
}


void PostProcess::Draw()
{
	//バリアを張る
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = depthStencilResource_.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	commandList_->ResourceBarrier(1, &barrier);

	//ルートシグネチャを設定
	commandList_->SetGraphicsRootSignature(postProcessRootSignature_.Get());
	//パイプラインステートを設定
	commandList_->SetPipelineState(postProcessPipelineState_.Get());
	//VBVを設定
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//ディスクリプタヒープを設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { multiPassSRVDescriptorHeap_.Get() };
	commandList_->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	//ディスクリプタテーブルを設定
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandles[6];
	srvHandles[0] = PostProcess::GetGPUDescriptorHandle(multiPassSRVDescriptorHeap_.Get(), descriptorSizeSRV, depthSRVIndex_);
	srvHandles[1] = PostProcess::GetGPUDescriptorHandle(multiPassSRVDescriptorHeap_.Get(), descriptorSizeSRV, secondPassResource_.srvIndex);;
	srvHandles[2] = PostProcess::GetGPUDescriptorHandle(multiPassSRVDescriptorHeap_.Get(), descriptorSizeSRV, blurResources_[static_cast<int>(BlurState::Vertical)].srvIndex);
	srvHandles[3] = PostProcess::GetGPUDescriptorHandle(multiPassSRVDescriptorHeap_.Get(), descriptorSizeSRV, highIntensityBlurResource_[static_cast<int>(BlurState::Vertical)].srvIndex);
	srvHandles[4] = PostProcess::GetGPUDescriptorHandle(multiPassSRVDescriptorHeap_.Get(), descriptorSizeSRV, shrinkBlurResources_[static_cast<int>(BlurState::Vertical)].srvIndex);
	srvHandles[5] = PostProcess::GetGPUDescriptorHandle(multiPassSRVDescriptorHeap_.Get(), descriptorSizeSRV, shrinkHighIntensityBlurResources_[static_cast<int>(BlurState::Vertical)].srvIndex);
	commandList_->SetGraphicsRootDescriptorTable(0, srvHandles[0]);
	commandList_->SetGraphicsRootDescriptorTable(1, srvHandles[1]);
	commandList_->SetGraphicsRootDescriptorTable(2, srvHandles[2]);
	commandList_->SetGraphicsRootDescriptorTable(3, srvHandles[3]);
	commandList_->SetGraphicsRootDescriptorTable(4, srvHandles[4]);
	commandList_->SetGraphicsRootDescriptorTable(5, srvHandles[5]);
	//CBVを設定
	commandList_->SetGraphicsRootConstantBufferView(6, bloomConstantBuffer_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(7, vignetteConstantBuffer_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(8, grayScaleConstantBuffer_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(9, boxFilterConstantBuffer_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(10, gaussianFilterConstantBuffer_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(11, luminanceBasedOutlineConstantBuffer_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(12, depthBasedOutlineConstantBuffer_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(13, hsvFilterConstantBuffer_->GetGPUVirtualAddress());

	//形状を設定
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//描画
	commandList_->DrawInstanced(UINT(vertices_.size()), 1, 0, 0);

	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = depthStencilResource_.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	commandList_->ResourceBarrier(1, &barrier);
}


void PostProcess::PreSecondPassDraw()
{
	//バリアを張る
	D3D12_RESOURCE_BARRIER barrier{};
	//通常テクスチャ
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = secondPassResource_.resource.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	commandList_->ResourceBarrier(1, &barrier);
	//高輝度テクスチャ
	barrier.Transition.pResource = highIntensityResource_.resource.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	commandList_->ResourceBarrier(1, &barrier);

	//RTVハンドルを取得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	rtvHandles[0] = PostProcess::GetCPUDescriptorHandle(multiPassRTVDescriptorHeap_.Get(), descriptorSizeRTV, secondPassResource_.rtvIndex);
	rtvHandles[1] = PostProcess::GetCPUDescriptorHandle(multiPassRTVDescriptorHeap_.Get(), descriptorSizeRTV, highIntensityResource_.rtvIndex);
	//描画先のRTVを設定する
	commandList_->OMSetRenderTargets(2, rtvHandles, false, nullptr);
	//指定した色で画面をクリアする
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };
	commandList_->ClearRenderTargetView(rtvHandles[0], clearColor, 0, nullptr);
	commandList_->ClearRenderTargetView(rtvHandles[1], clearColor, 0, nullptr);

	//ビューポート
	D3D12_VIEWPORT viewport{};
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = FLOAT(WindowsApp::GetInstance()->kClientWidth);
	viewport.Height = FLOAT(WindowsApp::GetInstance()->kClientHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	//ビューポートを設定
	commandList_->RSSetViewports(1, &viewport);

	//シザー矩形
	D3D12_RECT scissorRect{};
	//基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = WindowsApp::GetInstance()->kClientWidth;
	scissorRect.top = 0;
	scissorRect.bottom = WindowsApp::GetInstance()->kClientHeight;
	//シザーを設定
	commandList_->RSSetScissorRects(1, &scissorRect);
}


void PostProcess::SecondPassDraw()
{
	//ルートシグネチャを設定
	commandList_->SetGraphicsRootSignature(rootSignature_.Get());
	//パイプラインステートを設定
	commandList_->SetPipelineState(pipelineState_.Get());
	//VBVを設定
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//ディスクリプタヒープを設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { multiPassSRVDescriptorHeap_.Get() };
	commandList_->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	//ディスクリプタテーブルを設定
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandle = PostProcess::GetGPUDescriptorHandle(multiPassSRVDescriptorHeap_.Get(), descriptorSizeSRV, firstPassResource_.srvIndex);
	commandList_->SetGraphicsRootDescriptorTable(0, srvHandle);
	//形状を設定
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//描画
	commandList_->DrawInstanced(UINT(vertices_.size()), 1, 0, 0);
}


void PostProcess::PostSecondPassDraw()
{
	//バリアを張る
	D3D12_RESOURCE_BARRIER barrier{};
	//通常テクスチャ
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = secondPassResource_.resource.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	commandList_->ResourceBarrier(1, &barrier);
	//高輝度テクスチャ
	barrier.Transition.pResource = highIntensityResource_.resource.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	commandList_->ResourceBarrier(1, &barrier);
}


void PostProcess::PreBlur(BlurState blurState)
{
	//バリアを張る
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = blurResources_[static_cast<size_t>(blurState)].resource.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	commandList_->ResourceBarrier(1, &barrier);
	barrier.Transition.pResource = highIntensityBlurResource_[static_cast<size_t>(blurState)].resource.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	commandList_->ResourceBarrier(1, &barrier);

	//RTVハンドルを取得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	rtvHandles[0] = PostProcess::GetCPUDescriptorHandle(multiPassRTVDescriptorHeap_.Get(), descriptorSizeRTV, blurResources_[static_cast<size_t>(blurState)].rtvIndex);
	rtvHandles[1] = PostProcess::GetCPUDescriptorHandle(multiPassRTVDescriptorHeap_.Get(), descriptorSizeRTV, highIntensityBlurResource_[static_cast<size_t>(blurState)].rtvIndex);
	//描画先のRTVを設定する
	commandList_->OMSetRenderTargets(2, rtvHandles, false, nullptr);
	//指定した色で画面をクリアする
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };
	commandList_->ClearRenderTargetView(rtvHandles[0], clearColor, 0, nullptr);
	commandList_->ClearRenderTargetView(rtvHandles[1], clearColor, 0, nullptr);

	//ビューポート
	D3D12_VIEWPORT viewport{};
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = FLOAT(WindowsApp::GetInstance()->kClientWidth);
	viewport.Height = FLOAT(WindowsApp::GetInstance()->kClientHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	//ビューポートを設定
	commandList_->RSSetViewports(1, &viewport);

	//シザー矩形
	D3D12_RECT scissorRect{};
	//基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = WindowsApp::GetInstance()->kClientWidth;
	scissorRect.top = 0;
	scissorRect.bottom = WindowsApp::GetInstance()->kClientHeight;
	//シザーを設定
	commandList_->RSSetScissorRects(1, &scissorRect);
}


void PostProcess::Blur(BlurState blurState, uint32_t srvIndex, uint32_t highIntensitySrvIndex)
{
	//ルートシグネチャを設定
	commandList_->SetGraphicsRootSignature(blurRootSignature_.Get());
	//パイプラインステートを設定
	commandList_->SetPipelineState(blurPipelineState_[static_cast<size_t>(blurState)].Get());
	//VBVを設定
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//ディスクリプタテーブルを設定
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandles[2];
	srvHandles[0] = PostProcess::GetGPUDescriptorHandle(multiPassSRVDescriptorHeap_.Get(), descriptorSizeSRV, srvIndex);
	srvHandles[1] = PostProcess::GetGPUDescriptorHandle(multiPassSRVDescriptorHeap_.Get(), descriptorSizeSRV, highIntensitySrvIndex);
	commandList_->SetGraphicsRootDescriptorTable(0, srvHandles[0]);
	commandList_->SetGraphicsRootDescriptorTable(1, srvHandles[1]);
	//CBVをセット
	commandList_->SetGraphicsRootConstantBufferView(2, blurConstantBuffer_->GetGPUVirtualAddress());
	//形状を設定
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//描画
	commandList_->DrawInstanced(UINT(vertices_.size()), 1, 0, 0);
}


void PostProcess::PostBlur(BlurState blurState)
{
	//バリアを張る
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = blurResources_[static_cast<size_t>(blurState)].resource.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	commandList_->ResourceBarrier(1, &barrier);
	barrier.Transition.pResource = highIntensityBlurResource_[static_cast<size_t>(blurState)].resource.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	commandList_->ResourceBarrier(1, &barrier);
}


void PostProcess::PreShrinkBlur(BlurState blurState)
{
	//バリアを張る
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = shrinkBlurResources_[static_cast<size_t>(blurState)].resource.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	commandList_->ResourceBarrier(1, &barrier);
	barrier.Transition.pResource = shrinkHighIntensityBlurResources_[static_cast<size_t>(blurState)].resource.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	commandList_->ResourceBarrier(1, &barrier);

	//RTVハンドルを取得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	rtvHandles[0] = PostProcess::GetCPUDescriptorHandle(multiPassRTVDescriptorHeap_.Get(), descriptorSizeRTV, shrinkBlurResources_[static_cast<size_t>(blurState)].rtvIndex);
	rtvHandles[1] = PostProcess::GetCPUDescriptorHandle(multiPassRTVDescriptorHeap_.Get(), descriptorSizeRTV, shrinkHighIntensityBlurResources_[static_cast<size_t>(blurState)].rtvIndex);
	//描画先のRTVを設定する
	commandList_->OMSetRenderTargets(2, rtvHandles, false, nullptr);
	//指定した色で画面をクリアする
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };
	commandList_->ClearRenderTargetView(rtvHandles[0], clearColor, 0, nullptr);
	commandList_->ClearRenderTargetView(rtvHandles[1], clearColor, 0, nullptr);

	//ビューポート
	D3D12_VIEWPORT viewport{};
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = FLOAT(WindowsApp::GetInstance()->kClientWidth) / 2;
	viewport.Height = FLOAT(WindowsApp::GetInstance()->kClientHeight) / 2;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	//ビューポートを設定
	commandList_->RSSetViewports(1, &viewport);

	//シザー矩形
	D3D12_RECT scissorRect{};
	//基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = WindowsApp::GetInstance()->kClientWidth / 2;
	scissorRect.top = 0;
	scissorRect.bottom = WindowsApp::GetInstance()->kClientHeight / 2;
	//シザーを設定
	commandList_->RSSetScissorRects(1, &scissorRect);
}


void PostProcess::ShrinkBlur(BlurState blurState, uint32_t srvIndex, uint32_t highIntensitySrvIndex)
{
	//ルートシグネチャを設定
	commandList_->SetGraphicsRootSignature(blurRootSignature_.Get());
	//パイプラインステートを設定
	commandList_->SetPipelineState(blurPipelineState_[static_cast<size_t>(blurState)].Get());
	//VBVを設定
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//ディスクリプタテーブルを設定
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandles[2];
	srvHandles[0] = PostProcess::GetGPUDescriptorHandle(multiPassSRVDescriptorHeap_.Get(), descriptorSizeSRV, srvIndex);
	srvHandles[1] = PostProcess::GetGPUDescriptorHandle(multiPassSRVDescriptorHeap_.Get(), descriptorSizeSRV, highIntensitySrvIndex);
	commandList_->SetGraphicsRootDescriptorTable(0, srvHandles[0]);
	commandList_->SetGraphicsRootDescriptorTable(1, srvHandles[1]);
	//CBVをセット
	commandList_->SetGraphicsRootConstantBufferView(2, shrinkBlurConstantBuffer_->GetGPUVirtualAddress());
	//形状を設定
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//描画
	commandList_->DrawInstanced(UINT(vertices_.size()), 1, 0, 0);
}


void PostProcess::PostShrinkBlur(BlurState blurState)
{
	//バリアを張る
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = shrinkBlurResources_[static_cast<size_t>(blurState)].resource.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	commandList_->ResourceBarrier(1, &barrier);
	barrier.Transition.pResource = shrinkHighIntensityBlurResources_[static_cast<size_t>(blurState)].resource.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	commandList_->ResourceBarrier(1, &barrier);
}


Microsoft::WRL::ComPtr<ID3D12Resource> PostProcess::CreateTextureResource(uint32_t width, uint32_t height, DXGI_FORMAT format, const float* clearColor)
{
	//ヒープの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	//リソースの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	//ClearValue
	D3D12_CLEAR_VALUE clearValue;
	clearValue.Format = format;
	clearValue.Color[0] = clearColor[0];
	clearValue.Color[1] = clearColor[1];
	clearValue.Color[2] = clearColor[2];
	clearValue.Color[3] = clearColor[3];

	//リソースの作成
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr;
	hr = device_->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&clearValue,
		IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));

	return resource;
}

void PostProcess::Bloom()
{
	//Bloom用のCBVの作成
	bloomConstantBuffer_ = dxCore_->CreateBufferResource(sizeof(BloomData));

	//Bloom用のリソースに書き込む
	BloomData* bloomData = nullptr;
	bloomConstantBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&bloomData));
	bloomData->enable = isBloomActive_;
	bloomData->intensity = bloomIntensity_;
	bloomConstantBuffer_->Unmap(0, nullptr);
}

void PostProcess::UpdateBloom()
{
	//Bloom用のリソースに書き込む
	BloomData* bloomData = nullptr;
	bloomConstantBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&bloomData));
	bloomData->enable = isBloomActive_;
	bloomData->intensity = bloomIntensity_;
	bloomConstantBuffer_->Unmap(0, nullptr);

	//ImGui::Begin("Bloom");
	//ImGui::SliderFloat("intensity", &bloomIntensity_, 0.0f, 1.0f);
	//ImGui::End();
}

void PostProcess::Vignette()
{
	//ビネット用のCBVの作成
	vignetteConstantBuffer_ = dxCore_->CreateBufferResource(sizeof(VignetteData));

	//ビネット用のリソースに書き込む
	VignetteData* vignetteData = nullptr;
	vignetteConstantBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vignetteData));
	vignetteData->enable = isVignetteActive_;
	vignetteData->intensity = vignetteIntensity_;
	vignetteConstantBuffer_->Unmap(0, nullptr);
}

void PostProcess::UpdateVignette()
{
	//ビネット用のリソースに書き込む
	VignetteData* vignetteData = nullptr;
	vignetteConstantBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vignetteData));
	vignetteData->enable = isVignetteActive_;
	vignetteData->intensity = vignetteIntensity_;
	vignetteConstantBuffer_->Unmap(0, nullptr);
}

void PostProcess::GrayScale()
{
	//グレイスケール用のCBVの作成
	grayScaleConstantBuffer_ = dxCore_->CreateBufferResource(sizeof(GrayScaleData));

	//グレイスケール用のリソースに書き込む
	GrayScaleData* grayScaleData = nullptr;
	grayScaleConstantBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&grayScaleData));
	grayScaleData->enable = isGrayScaleActive_;
	grayScaleConstantBuffer_->Unmap(0, nullptr);
}

void PostProcess::UpdateGrayScale()
{
	//グレイスケール用のリソースに書き込む
	GrayScaleData* grayScaleData = nullptr;
	grayScaleConstantBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&grayScaleData));
	grayScaleData->enable = isGrayScaleActive_;
	grayScaleConstantBuffer_->Unmap(0, nullptr);
}

void PostProcess::BoxFilter()
{
	//BoxFilter用のCBVの作成
	boxFilterConstantBuffer_ = dxCore_->CreateBufferResource(sizeof(BoxFilterData));

	//BoxFilter用のリソースに書き込む
	BoxFilterData* boxFilterData = nullptr;
	boxFilterConstantBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&boxFilterData));
	boxFilterData->enable = isBoxFilterActive_;
	boxFilterConstantBuffer_->Unmap(0, nullptr);
}

void PostProcess::UpdateBoxFilter()
{
	//BoxFilter用のリソースに書き込む
	BoxFilterData* boxFilterData = nullptr;
	boxFilterConstantBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&boxFilterData));
	boxFilterData->enable = isBoxFilterActive_;
	boxFilterConstantBuffer_->Unmap(0, nullptr);
}

void PostProcess::GaussianFilter()
{
	//GaussianFilter用のCBVの作成
	gaussianFilterConstantBuffer_ = dxCore_->CreateBufferResource(sizeof(GaussianFilterData));

	//GaussianFilter用のリソースに書き込む
	GaussianFilterData* gaussianFilterData = nullptr;
	gaussianFilterConstantBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&gaussianFilterData));
	gaussianFilterData->enable = isGaussianFilterActive_;
	gaussianFilterConstantBuffer_->Unmap(0, nullptr);
}

void PostProcess::UpdateGaussianFilter()
{
	//GaussianFilter用のリソースに書き込む
	GaussianFilterData* gaussianFilterData = nullptr;
	gaussianFilterConstantBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&gaussianFilterData));
	gaussianFilterData->enable = isGaussianFilterActive_;
	gaussianFilterConstantBuffer_->Unmap(0, nullptr);
}

void PostProcess::LuminanceBasedOutline()
{
	//LuminanceOutline用のCBVの作成
	luminanceBasedOutlineConstantBuffer_ = dxCore_->CreateBufferResource(sizeof(LuminanceBasedOutlineData));

	//LuminanceOutline用のリソースに書き込む
	LuminanceBasedOutlineData* luminanceBasedOutlineData = nullptr;
	luminanceBasedOutlineConstantBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&luminanceBasedOutlineData));
	luminanceBasedOutlineData->enable = isLuminanceBasedOutlineActive_;
	luminanceBasedOutlineConstantBuffer_->Unmap(0, nullptr);
}

void PostProcess::UpdateLuminanceBasedOutline()
{
	//LuminanceOutline用のリソースに書き込む
	LuminanceBasedOutlineData* luminanceBasedOutlineData = nullptr;
	luminanceBasedOutlineConstantBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&luminanceBasedOutlineData));
	luminanceBasedOutlineData->enable = isLuminanceBasedOutlineActive_;
	luminanceBasedOutlineConstantBuffer_->Unmap(0, nullptr);
}

void PostProcess::DepthBasedOutline()
{
	//DepthOutline用のCBVの作成
	depthBasedOutlineConstantBuffer_ = dxCore_->CreateBufferResource(sizeof(DepthBasedOutlineData));

	//DepthOutline用のリソースに書き込む
	DepthBasedOutlineData* depthBasedOutlineData = nullptr;
	depthBasedOutlineConstantBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&depthBasedOutlineData));
	depthBasedOutlineData->enable = isDepthBasedOutlineActive_;
	depthBasedOutlineData->projectionInverse = projectionInverse_;
	depthBasedOutlineConstantBuffer_->Unmap(0, nullptr);
}

void PostProcess::UpdateDepthBasedOutline()
{
	//DepthOutline用のリソースに書き込む
	DepthBasedOutlineData* depthBasedOutlineData = nullptr;
	depthBasedOutlineConstantBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&depthBasedOutlineData));
	depthBasedOutlineData->enable = isDepthBasedOutlineActive_;
	depthBasedOutlineData->projectionInverse = projectionInverse_;
	depthBasedOutlineConstantBuffer_->Unmap(0, nullptr);
}

void PostProcess::HSVFilter()
{
	//HSVFilter用のCBVの作成
	hsvFilterConstantBuffer_ = dxCore_->CreateBufferResource(sizeof(HSVFilterData));

	//HSVFilter用のリソースに書き込む
	HSVFilterData* hsvFilterData = nullptr;
	hsvFilterConstantBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&hsvFilterData));
	hsvFilterData->enable = isHSVFilterActive_;
	hsvFilterData->hue = hue_;
	hsvFilterData->saturation = saturation_;
	hsvFilterData->value = value_;
	hsvFilterConstantBuffer_->Unmap(0, nullptr);
}

void PostProcess::UpdateHSVFilter()
{
	//HSVFilter用のリソースに書き込む
	HSVFilterData* hsvFilterData = nullptr;
	hsvFilterConstantBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&hsvFilterData));
	hsvFilterData->enable = isHSVFilterActive_;
	hsvFilterData->hue = hue_;
	hsvFilterData->saturation = saturation_;
	hsvFilterData->value = value_;
	hsvFilterConstantBuffer_->Unmap(0, nullptr);

	/*ImGui::Begin("HSVFilter");
	ImGui::SliderFloat("hue", &hue_, -1.0f, 1.0f);
	ImGui::SliderFloat("saturation", &saturation_, -1.0f, 1.0f);
	ImGui::SliderFloat("value", &value_, -1.0f, 1.0f);
	ImGui::End();*/
}

Microsoft::WRL::ComPtr<ID3D12Resource> PostProcess::CreateDepthStencilTextureResource(int32_t width, int32_t height)
{
	//生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;//Textureの幅
	resourceDesc.Height = height;//Textureの高さ
	resourceDesc.MipLevels = 1;//Mipmapの数
	resourceDesc.DepthOrArraySize = 1;//奥行or配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;//サンプルカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;//2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//DepthStencilとして使う通知

	//利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//VRAM上に作る


	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;//1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//フォーマット。Resourceと合わせる


	//Resourceの作成
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr;
	hr = device_->CreateCommittedResource(
		&heapProperties,//Heapの設定
		D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定。特になし。
		&resourceDesc,//Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,//深度値を書き込む状態にしておく
		&depthClearValue,//Clear最適値
		IID_PPV_ARGS(&resource));//作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));

	return resource;
}


uint32_t PostProcess::CreateRTV(const Microsoft::WRL::ComPtr<ID3D12Resource>& resource, DXGI_FORMAT format)
{
	rtvIndex_++;

	//RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = format;

	//RTVの作成
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = PostProcess::GetCPUDescriptorHandle(multiPassRTVDescriptorHeap_.Get(), descriptorSizeRTV, rtvIndex_);
	device_->CreateRenderTargetView(resource.Get(), &rtvDesc, rtvHandle);

	return rtvIndex_;
}


uint32_t PostProcess::CreateSRV(const Microsoft::WRL::ComPtr<ID3D12Resource>& resource, DXGI_FORMAT format)
{
	srvIndex_++;

	//SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	//SRVの作成
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = PostProcess::GetCPUDescriptorHandle(multiPassSRVDescriptorHeap_.Get(), descriptorSizeSRV, srvIndex_);
	device_->CreateShaderResourceView(resource.Get(), &srvDesc, srvHandle);

	return srvIndex_;
}


void PostProcess::CreateDSV()
{
	srvIndex_++;

	depthSRVIndex_ = srvIndex_;

	//DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//Format。基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2DTexture

	//DSVHeapの先頭にDSVを作る
	device_->CreateDepthStencilView(depthStencilResource_.Get(), &dsvDesc, multiPassDSVDescriptorHeap_->GetCPUDescriptorHandleForHeapStart());

	D3D12_SHADER_RESOURCE_VIEW_DESC depthTextureSrvDesc{};

	D3D12_CPU_DESCRIPTOR_HANDLE srvCPUHandle =
		GetCPUDescriptorHandle(multiPassSRVDescriptorHeap_.Get(), descriptorSizeSRV, srvIndex_);

	D3D12_GPU_DESCRIPTOR_HANDLE srvGPUHandle =
		GetGPUDescriptorHandle(multiPassSRVDescriptorHeap_.Get(), descriptorSizeSRV, srvIndex_);

	//DXGI_FORMAT_D24_UNORM
	depthTextureSrvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	depthTextureSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	depthTextureSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	depthTextureSrvDesc.Texture2D.MipLevels = 1;
	device_->CreateShaderResourceView(depthStencilResource_.Get(), &depthTextureSrvDesc, srvCPUHandle);
}


D3D12_CPU_DESCRIPTOR_HANDLE PostProcess::GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, const uint32_t descriptorSize, uint32_t index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += static_cast<D3D12_CPU_DESCRIPTOR_HANDLE>((descriptorSize * index)).ptr;
	return handleCPU;
}


D3D12_GPU_DESCRIPTOR_HANDLE PostProcess::GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, const uint32_t descriptorSize, uint32_t index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += static_cast<D3D12_GPU_DESCRIPTOR_HANDLE>((descriptorSize * index)).ptr;
	return handleGPU;
}