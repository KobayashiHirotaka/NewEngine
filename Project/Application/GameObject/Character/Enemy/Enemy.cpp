/**
 * @file Enemy.cpp
 * @brief 敵の管理(移動、攻撃など)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "Enemy.h"
#include "Application/GameObject/Character/Player/Player.h"
#include "Application/Game/Scenes/GamePlayScene/GamePlayScene.h"
#include "Application/Game/GameTimer/GameTimer.h"

Enemy::~Enemy()
{
	
}

void Enemy::Initialize()
{
	//タグの設定
	IGame3dObject::SetTag("Enemy");

	//初期化
	BaseCharacter::Initialize();

	//WorldTransformの初期化
	worldTransform_.Initialize();

	//当たり判定の設定
	collider_ = std::make_unique<Collider>();
	collider_->SetAABB(aabb_);

	collider_->SetCollisionAttribute(kCollisionAttributeEnemy);
	collider_->SetCollisionMask(kCollisionMaskEnemy);
	collider_->SetCollisionPrimitive(kCollisionPrimitiveAABB);

	collider_->SetGameObject(this);

	//LineBoxの描画
	lineBox_.reset(LineBox::Create(aabb_));

	//リソース
	//体力ゲージ
	const Vector2 kHpBarPosition = { 742.0f, kBarSpace_ };
	const Vector2 kHpBarSize = { -barSize_  ,7.2f };

	hpBar_ = {
		true,
		Engine::TextureManager::LoadTexture("Resource/Images/HP.png"),
		kHpBarPosition,
		0.0f,
		kHpBarSize,
		nullptr,
	};

	hpBar_.sprite_.reset(Sprite::Create(hpBar_.textureHandle_, hpBar_.position_));

	//ガードゲージ
	const Vector2 kGuardGaugeBarPosition = { 742.0f, kGuardGaugeBarSpace_ };
	const Vector2 kGuardGaugeBarSize = { -guardGaugeBarSize_  ,7.0f };

	guardGaugeBar_ = {
		true,
		Engine::TextureManager::LoadTexture("Resource/Images/GuardGauge.png"),
		{kGuardGaugeBarPosition},
		0.0f,
		{kGuardGaugeBarSize},
		nullptr,
	};

	guardGaugeBar_.sprite_.reset(Sprite::Create(guardGaugeBar_.textureHandle_, guardGaugeBar_.position_));

	//必殺技ゲージ
	const Vector2 kFinisherGaugeBarPosition = { 979.0f, kFinisherGaugeBarSpace_ };
	const Vector2 kFinisherGaugeBarSize = { -finisherGaugeBarSize_  ,19.3f };

	finisherGaugeBar_ = {
		true,
		Engine::TextureManager::LoadTexture("Resource/Images/FinisherGauge.png"),
		{kFinisherGaugeBarPosition},
		0.0f,
		{kFinisherGaugeBarSize},
		nullptr,
	};

	finisherGaugeBar_.sprite_.reset(Sprite::Create(finisherGaugeBar_.textureHandle_, finisherGaugeBar_.position_));

	//ヒット表示
	const Vector2 kHitSpritePosition = { 40.0f, 180.0f };

	hitTextureHandle_ = Engine::TextureManager::LoadTexture("Resource/Images/Hit.png");
	hitSprite_.reset(Sprite::Create(hitTextureHandle_, kHitSpritePosition));

	//コンボ表示
	const Vector2 kComboNumSpritePosition = { 10.0f, 290.0f };

	comboNumTextureHandle_ = Engine::TextureManager::LoadTexture("Resource/Number/0.png");
	comboNumSprite_.reset(Sprite::Create(comboNumTextureHandle_, kComboNumSpritePosition));

	//キャラクターアイコン
	const Vector2 kEnemyIconPosition = { 1110.0f, 20.0f };
	const Vector2 kEnemyIconSize = { 120.0f,120.0f };

	enemyIconTextureHandle_ = Engine::TextureManager::LoadTexture("Resource/Images/EnemyIcon.png");

	enemyIconSprite_.reset(Sprite::Create(enemyIconTextureHandle_, kEnemyIconPosition));
	enemyIconSprite_->SetSize(kEnemyIconSize);

	//弾のモデルを生成
	bulletModel_.reset(Model::CreateFromOBJ("Resource/Bullet", "Bullet.obj"));

	//パーティクル
	particleEffectPlayer_ = std::make_unique<ParticleEffectPlayer>();
	particleEffectPlayer_->Initialize();

	//SEの読み込み
	attackSoundHandle_ = audio_->LoadSoundMP3("Resource/Sounds/Attack.mp3");
	weaponAttackSoundHandle_ = audio_->LoadSoundMP3("Resource/Sounds/WeaponAttack.mp3");
	damageSoundHandle_ = audio_->LoadSoundMP3("Resource/Sounds/HitPunch1.mp3");
	guardSoundHandle_ = audio_->LoadSoundMP3("Resource/Sounds/Guard.mp3");

	//基本データの設定
	baseData_.hp_ = baseData_.kMaxHp_;
	moveData_.frontSpeed_ = kMaxFrontSpeed_;
	moveData_.backSpeed_ = kMaxBackSpeed_;

	//行動パターンの初期化
	patternCount_ = 0;

	//WorldTransformの更新
	worldTransform_.UpdateMatrixEuler();
}

void Enemy::Update()
{
#ifdef _ADJUSTMENT

	if (input_->PressKey(DIK_A))
	{
		worldTransform_.translation.x -= 0.05f;
	}

	if (input_->PressKey(DIK_D))
	{
		worldTransform_.translation.x += 0.05f;
	}

#endif

	//更新
	BaseCharacter::Update();

	//WorldTransformの更新
	worldTransform_.UpdateMatrixEuler();

	//当たり判定の更新
	collider_->Update();
}

void Enemy::Draw(const Camera& camera)
{
	//モデル描画
	model_->Draw(worldTransform_, camera, animationIndex_);
}

void Enemy::DrawBone(const Camera& camera)
{
	//骨の描画
	model_->DrawBone(worldTransform_, camera);
}

void Enemy::DrawCollision(const Camera& camera)
{
	//LineBoxの描画
	lineBox_->Draw(worldTransform_, camera);
}

void Enemy::DrawSprite()
{
	//体力ゲージの描画
	if (baseData_.hp_ >= 0)
	{
		hpBar_.sprite_->Draw();
	}

	//ガードゲージの描画
	guardGaugeBar_.sprite_->Draw();

	//必殺技ゲージの描画
	finisherGaugeBar_.sprite_->Draw();

	//エネミーアイコンの描画
	enemyIconSprite_->Draw();

	//コンボ表示の描画
	if (comboCount_ >= kComboCount_[2])
	{
		hitSprite_->Draw();
		comboNumSprite_->Draw();
	}
}

void Enemy::DrawParticle(const Camera& camera)
{
	//パーティクルの描画
	particleEffectPlayer_->Draw(camera);

	for (auto& bullet : bullets_)
	{
		bullet->DrawParticle(camera);
	}
}

void Enemy::ImGui()
{
	
}

void Enemy::OnCollision(Collider* collider)
{
	if (collider->GetCollisionAttribute() & kCollisionAttributePlayer)
	{
		characterState_.isHitCharacter = true;
	}
}

void Enemy::EndAttack(bool& isAttackType)
{
	//攻撃の終了処理
	player_->SetIsGuarded(false);
	BaseCharacter::EndAttack(isAttackType);
}

void Enemy::ApplyDamage()
{
	
}

void Enemy::ResetCollision()
{
	//当たり判定のリセット
	aabb_ = defaultCollsiion_;
	collider_->SetAABB(aabb_);
}

void Enemy::Reset()
{
	
}

void Enemy::ShootBullet(const Vector3& startPosition, const Vector3& velocity)
{
	//弾を生成し、リストに追加する
	auto newBullet = std::make_unique<EnemyBullet>();
	newBullet->Create(bulletModel_.get(), startPosition, velocity);
	bullets_.push_back(std::move(newBullet));
}

void Enemy::UpdateBullets()
{
	//弾の更新と衝突判定などを行う
	for (auto it = bullets_.begin(); it != bullets_.end();)
	{
		(*it)->Update();
		if ((*it)->GetIsDead())
		{
			it = bullets_.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void Enemy::HitCombo()
{
	
}

void Enemy::ComboCountUpdate(const int )
{
	
}

Vector3 Enemy::GetWorldPosition()
{
	Vector3 pos{};
	pos.x = worldTransform_.matWorld.m[3][0];
	pos.y = worldTransform_.matWorld.m[3][1];
	pos.z = worldTransform_.matWorld.m[3][2];
	return pos;
}
