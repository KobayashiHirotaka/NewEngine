/**
 * @file ImGuiManager.cpp
 * @brief ImGuiの管理(初期化、描画など)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "ImGuiManager.h"

ImGuiManager* ImGuiManager::GetInstance()
{
	//インスタンスを生成
	static ImGuiManager sInstance;
	return &sInstance;
}

void ImGuiManager::Initialize()
{
	//WindowsAppのインスタンスの取得
	win_ = WindowsApp::GetInstance();

	//DirectXCoreのインスタンスの取得
	dxCore_ = DirectXCore::GetInstance();

	//SRVディスクリプタヒープを作成
	srvDescriptorHeap_ = dxCore_->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1, true);

	//ImGuiの初期設定
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(win_->GetHwnd());
	ImGui_ImplDX12_Init(dxCore_->GetDevice(),
		2,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		srvDescriptorHeap_.Get(),
		srvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap_->GetGPUDescriptorHandleForHeapStart());
}

void ImGuiManager::Draw()
{
	//描画処理
#ifdef _USE_IMGUI
	ID3D12GraphicsCommandList* commandList = dxCore_->GetCommandList();

	ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap_.Get() };
	commandList->SetDescriptorHeaps(1, descriptorHeaps);

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
#endif
}

void ImGuiManager::BeginFlame()
{
	//新しいフレームを開始
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::EndFlame()
{
	//描画処理を完了
	ImGui::Render();
}


void ImGuiManager::ShutDown()
{
	//ImGuiのDX12とWin32関連の終了処理
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();

	//ImGuiのコンテキストを破棄
	ImGui::DestroyContext();
}
