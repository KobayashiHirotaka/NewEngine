/**
 * @file EmitterBuilder.h
 * @brief パーティクルエミッターの生成を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "ParticleEmitter.h"

class EmitterBuilder
{
public:
	/// <summary>コンストラクタ</summary>
	EmitterBuilder();

	/// <summary>デストラクタ</summary>
	~EmitterBuilder();

	/// <summary>エミッターの名前を設定</summary>
	EmitterBuilder& SetEmitterName(const std::string& emitterName);

	/// <summary>パーティクルのタイプを設定</summary>
	EmitterBuilder& SetParticleType(ParticleEmitter::ParticleType particleType);

	/// <summary>エミッターの位置を設定</summary>
	EmitterBuilder& SetTranslation(const Vector3& translation);

	/// <summary>エミッターの発生エリアを設定</summary>
	EmitterBuilder& SetArea(const Vector3& min, const Vector3& max);

	/// <summary>パーティクルの回転範囲を設定</summary>
	EmitterBuilder& SetRotation(const Vector3& min, const Vector3& max);

	/// <summary>パーティクルのスケール範囲を設定</summary>
	EmitterBuilder& SetScale(const Vector3& min, const Vector3& max);

	/// <summary>パーティクルの水平角度範囲を設定</summary>
	EmitterBuilder& SetAzimuth(float min, float max);

	/// <summary>パーティクルの垂直角度範囲を設定</summary>
	EmitterBuilder& SetElevation(float min, float max);

	/// <summary>パーティクルの速度範囲を設定</summary>
	EmitterBuilder& SetVelocity(const Vector3& min, const Vector3& max);

	/// <summary>パーティクルの色範囲を設定</summary>
	EmitterBuilder& SetColor(const Vector4& min, const Vector4& max);

	/// <summary>パーティクルのライフタイム範囲を設定</summary>
	EmitterBuilder& SetLifeTime(float min, float max);

	/// <summary>パーティクルの数を設定</summary>
	EmitterBuilder& SetCount(uint32_t count);

	/// <summary>パーティクルの発生頻度を設定</summary>
	EmitterBuilder& SetFrequency(float frequency);

	/// <summary>エミッターの削除時間を設定</summary>
	EmitterBuilder& SetDeleteTime(float deleteTime);

	/// <summary>設定に基づいてエミッターを構築</summary>
	ParticleEmitter* Build();

private:
	//ParticleEmitterポインタ
	ParticleEmitter* particleEmitter_ = nullptr;
};

