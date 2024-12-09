/**
 * @file ImGuiManager.h
 * @brief ImGuiの管理(初期化、描画など)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/Base/WindowsApp/WindowsApp.h"
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/externals/imgui/imgui.h"
#include "Engine/externals/imgui/imgui_impl_dx12.h"

class ImGuiManager
{
public:
	static ImGuiManager* GetInstance();

	void Initialize();

	void BeginFlame();

	void EndFlame();

	void Draw();

	void ShutDown();

private:
	ImGuiManager() = default;
	~ImGuiManager() = default;
	ImGuiManager(const ImGuiManager&) = delete;
	ImGuiManager& operator = (const ImGuiManager&) = delete;

private:
	WindowsApp* win_ = nullptr;

	DirectXCore* dxCore_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_ = nullptr;
};

