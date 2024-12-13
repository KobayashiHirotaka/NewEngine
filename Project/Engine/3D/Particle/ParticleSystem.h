/**
 * @file ParticleSystem.h
 * @brief パーティクルシステムの初期化、更新、エミッターの追加などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Base/TextureManager/TextureManager.h"
#include "EmitterBuilder.h"

class ParticleSystem
{
public:
	//パーティクルの最大インスタンス数
	const uint32_t kMaxInstance = 10000;

	//GPUに送るパーティクルデータ
	struct ParticleForGPU
	{
		//ワールド行列
		Matrix4x4 world;

		//色
		Vector4 color;
	};

	/// <summary>初期化</summary>
	void Initialize();

	/// <summary>更新</summary>
	void Update();

	/// <summary>パーティクルエミッターの追加</summary>
	void AddParticleEmitter(ParticleEmitter* particleEmitter) { particleEmitters_.push_back(std::unique_ptr<ParticleEmitter>(particleEmitter)); };

	/// <summary>パーティクルエミッターの取得</summary>
	ParticleEmitter* GetParticleEmitter(const std::string& emitterName);

	/// <summary>パーティクルエミッターの追加</summary>
	std::list<ParticleEmitter*> GetParticleEmitters(const std::string& emitterName);

	//Getter
	//Index
	const uint32_t& GetSrvIndex() const { return srvIndex_; };

	//NumInstance
	const uint32_t& GetNumInstance() const { return numInstances_; };

private:
	/// <summary>インスタンシングリソースを作成</summary>
	void CreateInstancingResource();

	/// <summary>インスタンシングリソースを更新</summary>
	void UpdateInstancingResource();

private:
	//Instancing用のワールド変換リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource_ = nullptr;

	//Instancing用のSRVのインデックス
	uint32_t srvIndex_ = 0;

	//エミッターのリスト
	std::list<std::unique_ptr<ParticleEmitter>> particleEmitters_{};

	//インスタンス数
	uint32_t numInstances_ = 0;
};

