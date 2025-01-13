/**
 * @file ImGuiManager.h
 * @brief ImGuiの管理(初期化、描画など)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/Base/WindowsApp/WindowsApp.h"
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Externals/imgui/imgui.h"
#include "Engine/Externals/imgui/imgui_impl_dx12.h"

class ImGuiManager
{
public:
	/// <summary>インスタンスの取得</summary>
	static ImGuiManager* GetInstance();

	/// <summary>初期化</summary>
	void Initialize();

	/// <summary>フレーム開始処理</summary>
	void BeginFlame();

	/// <summary>フレーム	終了処理</summary>
	void EndFlame();

	/// <summary>描画</summary>
	void Draw();

	/// <summary>終了処理</summary>
	void ShutDown();

private:
	//シングルトン
	ImGuiManager() = default;
	~ImGuiManager() = default;
	ImGuiManager(const ImGuiManager&) = delete;
	ImGuiManager& operator = (const ImGuiManager&) = delete;

private:
	//WindowsAppのポインタ
	WindowsApp* win_ = nullptr;

	//DirectXCoreのポインタ
	DirectXCore* dxCore_ = nullptr;

	//SRVのディスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_ = nullptr;
};

