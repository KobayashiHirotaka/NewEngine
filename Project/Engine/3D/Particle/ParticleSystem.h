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
	const uint32_t kMaxInstance = 10000;

	struct ParticleForGPU
	{
		Matrix4x4 world;
		Vector4 color;
	};

	void Initialize();

	void Update();

	void AddParticleEmitter(ParticleEmitter* particleEmitter) { particleEmitters_.push_back(std::unique_ptr<ParticleEmitter>(particleEmitter)); };

	const uint32_t& GetSrvIndex() const { return srvIndex_; };

	const uint32_t& GetNumInstance() const { return numInstances_; };

	ParticleEmitter* GetParticleEmitter(const std::string& emitterName);

	std::list<ParticleEmitter*> GetParticleEmitters(const std::string& emitterName);

private:
	void CreateInstancingResource();

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

