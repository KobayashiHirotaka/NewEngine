#include "ImGuiManager.h"

ImGuiManager* ImGuiManager::GetInstance()
{
	static ImGuiManager instance;
	return &instance;
}

void ImGuiManager::Initialize(WindowsApp* win, DirectXCore* dxCore)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(win->GetHwnd());
	ImGui_ImplDX12_Init(dxCore->GetDevice(),
		dxCore->GetSwapChainDesc().BufferCount,
		dxCore->GetRtvDesc().Format,
		dxCore->GetSrvDescriptorHeap(),
		dxCore->GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
		dxCore->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
}

void ImGuiManager::BeginFlame()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::EndFlame()
{
	ImGui::Render();
}
