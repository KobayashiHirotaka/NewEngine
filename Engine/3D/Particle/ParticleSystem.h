#pragma once
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

	const uint32_t& GetNumInstance() const { return numInstance_; };

	ParticleEmitter* GetParticleEmitter(const std::string& name);

	std::list<ParticleEmitter*> GetParticleEmitters(const std::string& name);

private:
	void CreateInstancingResource();

	void UpdateInstancingResource();

private:
	//Instancing用のWorldTransform
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource_ = nullptr;

	//Instancing用のSRVの番号
	uint32_t srvIndex_ = 0;

	//エミッターのリスト
	std::list<std::unique_ptr<ParticleEmitter>> particleEmitters_{};

	//インスタンス数
	uint32_t numInstance_ = 0;
};

