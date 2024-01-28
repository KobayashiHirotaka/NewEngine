#include "ParticleSystem.h"

void ParticleSystem::Initialize()
{
    //インスタンシング用のリソースの作成
    CreateInstancingResource();

    //SRVの作成
    srvIndex_ = TextureManager::GetInstance()->CreateInstancingSRV(instancingResource_, kMaxInstance, sizeof(ParticleForGPU));
}

void ParticleSystem::Update()
{
    //エミッターの削除
    particleEmitters_.remove_if([](std::unique_ptr<ParticleEmitter>& emitter) {
        if (emitter->GetIsDead()) {
            emitter.reset();
            return true;
        }
        return false;
        });

    //エミッターの更新
    for (auto& emitter : particleEmitters_)
    {
        emitter->Update();
    }

    //インスタンシング用のリソースの更新
    UpdateInstancingResource();
}

void ParticleSystem::CreateInstancingResource()
{
    //Instancing用のWorldTransformリソースを作成
    instancingResource_ = DirectXCore::GetInstance()->CreateBufferResource(sizeof(ParticleForGPU) * kMaxInstance);

    //書き込むためのアドレスを取得
    ParticleForGPU* instancingData = nullptr;
    instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData));

    //単位行列を書き込んでおく
    for (uint32_t index = 0; index < kMaxInstance; ++index)
    {
        instancingData[index].world = MakeIdentity4x4();
        instancingData[index].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    }

    instancingResource_->Unmap(0, nullptr);
}

void ParticleSystem::UpdateInstancingResource()
{
    ParticleForGPU* instancingData = nullptr;
    numInstances_ = 0;
    instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData));

    for (auto emitterIterator = particleEmitters_.begin(); emitterIterator != particleEmitters_.end();)
    {
        //パーティクルのリストを取得
        auto& particles = emitterIterator->get()->GetParticles();
        for (auto particleIterator = particles.begin(); particleIterator != particles.end();)
        {
            if (particleIterator->get()->IsDead())
            {
                particleIterator = particles.erase(particleIterator);
                continue;
            }

            Matrix4x4 worldMatrix = MakeAffineMatrix(
                particleIterator->get()->GetScale(),
                particleIterator->get()->GetRotation(),
                particleIterator->get()->GetTranslation()
            );
            if (numInstances_ < kMaxInstance)
            {
                instancingData[numInstances_].world = worldMatrix;
                instancingData[numInstances_].color = particleIterator->get()->GetColor();
                numInstances_++;
            }
            ++particleIterator;
        }
        ++emitterIterator;
    }
    instancingResource_->Unmap(0, nullptr);
}

ParticleEmitter* ParticleSystem::GetParticleEmitter(const std::string& emitterName)
{
    // エミッターのリストから探す
    for (auto& emitter : particleEmitters_)
    {
        if (emitter->GetEmitterName() == emitterName)
        {
            return emitter.get();
        }
    }

    return nullptr;
}

std::list<ParticleEmitter*> ParticleSystem::GetParticleEmitters(const std::string& emitterName)
{
    std::list<ParticleEmitter*> matchingEmitters;

    for (auto& emitter : particleEmitters_)
    {
        if (emitter->GetEmitterName() == emitterName)
        {
            matchingEmitters.push_back(emitter.get());
        }
    }

    return matchingEmitters;
}