/**
 * @file ParticleEmitter.cpp
 * @brief エミッターに設定されたパラメータに基づいてパーティクルを生成、更新を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "ParticleEmitter.h"

void ParticleEmitter::Initialize()
{

}

void ParticleEmitter::Update()
{
    //フレーム時間
    const float kDeltaTime = 1.0f / 60.0f;
    frequencyTime_ += kDeltaTime;

    //パーティクルを発生させる
    if (frequency_ <= frequencyTime_)
    {
        for (uint32_t index = 0; index < particleCount_; ++index)
        {
            EmitParticle();
        }
        frequencyTime_ -= frequency_;
    }

    //パーティクルの更新
    for (auto& particle : particles_)
    {
        particle->Update();
    }

    //エミッターの死亡フラグ
    deleteTimer_ += kDeltaTime;
    if (deleteTimer_ > deleteTime_)
    {
        isDead_ = true;
    }
}

void ParticleEmitter::EmitParticle()
{
    //発生位置を設定
    Vector3 translation = {
        translation_.x + Random::GetRandomFloat(area_.min.x, area_.max.x),
        translation_.y + Random::GetRandomFloat(area_.min.y, area_.max.y),
        translation_.z + Random::GetRandomFloat(area_.min.z, area_.max.z)
    };

    //回転の範囲を設定
    Vector3 rotation = {
        Random::GetRandomFloat(rotation_.min.x, rotation_.max.x),
        Random::GetRandomFloat(rotation_.min.y, rotation_.max.y),
        Random::GetRandomFloat(rotation_.min.z, rotation_.max.z)
    };

    //スケールの範囲を設定
    Vector3 scale = {
        Random::GetRandomFloat(scale_.min.x, scale_.max.x),
        Random::GetRandomFloat(scale_.min.y, scale_.max.y),
        Random::GetRandomFloat(scale_.min.z, scale_.max.z)
    };

    //方位角と仰角を設定
    const float kDegreesToRadians = 180.0f;

    float azimuth = Random::GetRandomFloat(azimuth_.min, azimuth_.max);
    float azimuthRadian = azimuth * float(std::numbers::pi / kDegreesToRadians);

    float elevation = Random::GetRandomFloat(elevation_.min, elevation_.max);
    float elevationRadian = elevation * float(std::numbers::pi / kDegreesToRadians);

    // 速度を設定
    Vector3 velocity = {
        Random::GetRandomFloat(velocity_.min.x, velocity_.max.x) * std::cos(elevationRadian) * std::cos(azimuthRadian),
        Random::GetRandomFloat(velocity_.min.y, velocity_.max.y) * std::cos(elevationRadian) * std::sin(azimuthRadian),
        Random::GetRandomFloat(velocity_.min.z, velocity_.max.z) * std::sin(elevationRadian)
    };

    //色を設定
    Vector4 color = {
        Random::GetRandomFloat(color_.min.x, color_.max.x),
        Random::GetRandomFloat(color_.min.y, color_.max.y),
        Random::GetRandomFloat(color_.min.z, color_.max.z),
        Random::GetRandomFloat(color_.min.w, color_.max.w)
    };

    //生存時間を設定
    float lifeTime = Random::GetRandomFloat(lifeTime_.min, lifeTime_.max);

    std::unique_ptr<BaseParticle> particle;
    switch (particleType_)
    {
    case ParticleType::kBase:
        //ベースパーティクルの生成
        particle = std::make_unique<BaseParticle>();
        break;

    case ParticleType::kNormal:
        //通常のパーティクルの生成
        particle = std::make_unique<Particle>();
        break;
    }

    //パーティクルを初期化し、リストに追加
    if (particle)
    {
        particle->Initialize(translation, rotation, scale, velocity, color, lifeTime);
        particles_.push_back(std::move(particle));
    }
}