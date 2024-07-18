#include "Enemy.h"
#include "Application/GameObject/Character/Player/Player.h"
#include "Application/Game/Scenes/GamePlayScene.h"

Enemy::~Enemy()
{
	delete hpBar_.sprite_;
	delete guardGaugeBar_.sprite_;
	delete finisherGaugeBar_.sprite_;
}

void Enemy::Initialize()
{
	IGame3dObject::SetTag("Enemy");

	//modelManagerのinstance
	modelManager_ = ModelManager::GetInstance();

	//inputのinstance
	input_ = Input::GetInstance();

	//audioのinstance
	audio_ = Audio::GetInstance();

	//worldTransformの初期化
	worldTransform_.Initialize();

	//当たり判定の設定
	SetAABB(aabb_);

	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(kCollisionMaskEnemy);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);

	//リソースの初期化(sprite,se)
	//各ゲージの初期化
	hpBar_ = {
		true,
		TextureManager::LoadTexture("resource/images/HP.png"),
		{720.0f, barSpace_},
		0.0f,
		{-barSize_  ,7.2f},
		nullptr,
	};

	hpBar_.sprite_ = Sprite::Create(hpBar_.textureHandle_, hpBar_.position_);

	guardGaugeBar_ = {
		true,
		TextureManager::LoadTexture("resource/images/guardGauge.png"),
		{720.0f, guardGaugeBarSpace_},
		0.0f,
		{-guardGaugeBarSize_  ,7.0f},
		nullptr,
	};

	guardGaugeBar_.sprite_ = Sprite::Create(guardGaugeBar_.textureHandle_, guardGaugeBar_.position_);

	finisherGaugeBar_ = {
		true,
		TextureManager::LoadTexture("resource/images/guardGauge.png"),
		{978.0f, finisherGaugeBarSpace_},
		0.0f,
		{-finisherGaugeBarSize_  ,20.0f},
		nullptr,
	};

	finisherGaugeBar_.sprite_ = Sprite::Create(finisherGaugeBar_.textureHandle_, finisherGaugeBar_.position_);

	hitTextureHandle_ = TextureManager::LoadTexture("resource/images/Hit.png");
	hitSprite_.reset(Sprite::Create(hitTextureHandle_, { 40.0f, 180.0f }));

	comboNumTextureHandle_ = TextureManager::LoadTexture("resource/number/0.png");
	comboNumSprite_.reset(Sprite::Create(comboNumTextureHandle_, { 10.0f, 290.0f }));

	//seの初期化
	attackSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/Attack.mp3");
	weaponAttackSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/WeaponAttack.mp3");
	damageSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/Damage.mp3");
	guardSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/Guard.mp3");

	//パーティクルの初期化
	particleModel_.reset(ParticleModel::CreateFromOBJ("resource/Particle", "Particle.obj"));
	particleSystem_ = std::make_unique<ParticleSystem>();
	particleSystem_->Initialize();

	//worldTransformの更新
	worldTransform_.UpdateMatrixEuler();
}

void Enemy::Update()
{
	//テスト用の処理
	if (input_->PressKey(DIK_D))
	{
		guardGauge_ += 1.0f;
	}

	//アニメーションテスト用
	if (input_->PressKey(DIK_0))
	{
		animationIndex_ = 0;
	}
	if (input_->PressKey(DIK_1))
	{
		animationIndex_ = 1;
	}
	if (input_->PressKey(DIK_2))
	{
		animationIndex_ = 2;
	}
	if (input_->PressKey(DIK_3))
	{
		animationIndex_ = 3;
	}
	if (input_->PressKey(DIK_4))
	{
		animationIndex_ = 4;
	}
	if (input_->PressKey(DIK_5))
	{
		animationIndex_ = 5;
	}
	if (input_->PressKey(DIK_6))
	{
		animationIndex_ = 6;
	}
	if (input_->PressKey(DIK_7))
	{
		animationIndex_ = 7;
	}
	if (input_->PressKey(DIK_8))
	{
		animationIndex_ = 8;
	}
	if (input_->PressKey(DIK_9))
	{
		animationIndex_ = 9;
	}
	if (input_->PressKey(DIK_Q))
	{
		animationIndex_ = 10;
	}

	//ここまでテスト用の処理

	isShake_ = false;

	//0は後ろ歩き,1は前歩き,2は停止
	model_->ApplyAnimation(animationIndex_);

	model_->Update();

	//リセットのときの処理
	if (isReset_)
	{
		Reset();
	}

	//EnemyのBehavior
	if (behaviorRequest_)
	{
		behavior_ = behaviorRequest_.value();

		switch (behavior_)
		{
		case Behavior::kRoot:
		default:
			BehaviorRootInitialize();
			break;

		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;

		case Behavior::kJump:
			BehaviorJumpInitialize();
			break;

		case Behavior::kThrow:
			BehaviorThrowInitialize();
			break;

		case Behavior::kStan:
			BehaviorStanInitialize();
			break;
		}

		behaviorRequest_ = std::nullopt;
	}

	switch (behavior_)
	{
	case Behavior::kRoot:
	default:
		if (GamePlayScene::roundStartTimer_ <= 0 && GamePlayScene::migrationTimer == 200)
		{
			BehaviorRootUpdate();
		}
		break;

	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;

	case Behavior::kJump:
		BehaviorJumpUpdate();
		break;

	case Behavior::kThrow:
		BehaviorThrowUpdate();
		break;

	case Behavior::kStan:
		BehaviorStanUpdate();
		break;
	}

	//振り向きの処理
	Vector3 playerWorldPosition = player_->GetWorldPosition();

	Vector3 enemyWorldPosition = GetWorldPosition();

	if (enemyWorldPosition.x > playerWorldPosition.x && behavior_ != Behavior::kJump
		&& behavior_ != Behavior::kAttack && !isDown_)
	{
		enemyDirection_ = Direction::Left;
		worldTransform_.rotation.y = 4.6f;
	}

	if (enemyWorldPosition.x < playerWorldPosition.x && behavior_ != Behavior::kJump
		&& behavior_ != Behavior::kAttack && !isDown_)
	{
		enemyDirection_ = Direction::Right;
		worldTransform_.rotation.y = 1.7f;
	}

	if (workAttack_.isAttack && worldTransform_.translation.x >= 3.5f && enemyDirection_ == Direction::Right)
	{
		worldTransform_.translation.x = 3.5f;
	}

	if (workAttack_.isAttack && worldTransform_.translation.x <= -3.5f && enemyDirection_ == Direction::Left)
	{
		worldTransform_.translation.x = -3.5f;
	}

	//画面端の処理
	if (worldTransform_.translation.x >= 4.0f)
	{
		worldTransform_.translation.x = 4.0f;
	}

	if (worldTransform_.translation.x <= -4.0f)
	{
		worldTransform_.translation.x = -4.0f;
	}

	//ジャンプ中にプレイヤーと当たったときの処理
	if (behaviorRequest_ == Behavior::kJump && isHit_)
	{
		worldTransform_.translation.y = 0.0f;
	}

	DownAnimation();

	//パーティクルの更新
	particleSystem_->Update();

	isHit_ = false;

	isPlayerHit_ = false;

	//各ゲージの更新処理
	HPBarUpdate();

	GuardGaugeBarUpdate();

	FinisherGaugeBarUpdate();

	//コンボを食らっているとき
	if (isHitLightPunch_)
	{
		comboCount_ = 1;
		comboTimer_--;
	}

	if (isHitTCMiddlePunch_)
	{
		comboCount_ = 2;
		comboTimer_ = 60;
		comboTimer_--;
	}

	if (isHitTCHighPunch_)
	{
		comboCount_ = 3;
		comboTimer_ = 60;
		comboTimer_--;
	}

	if (isHitHighPunch_ && comboCount_ == 0)
	{
		comboCount_ = 1;
		comboTimer_ = 60;
		comboTimer_--;
	}

	if (isHitHighPunch_ && comboCount_ >= 2)
	{
		comboCount_ = 3;
		comboTimer_ = 60;
		comboTimer_--;
	}

	if (isHitTackle_ && comboCount_ == 3)
	{
		comboCount_ = 4;
		comboTimer_ = 60;
		comboTimer_--;
	}

	if (isHitTackle_ && comboCount_ == 1)
	{
		comboCount_ = 2;
		comboTimer_ = 60;
		comboTimer_--;
	}

	if (comboTimer_ < 60)
	{
		comboTimer_--;
	}

	if (comboTimer_ < 0)
	{
		comboTimer_ = 60;
		comboCount_ = 0;
	}

	if (!player_->GetIsAttack())
	{
		guardAnimationTimer_ = 60;
	}

	ComboNumberSpriteUpdate();

	//imGui
	model_->GetLight()->ImGui("DirectionalLight");
	model_->GetPointLight()->ImGui("PointLight");
	model_->GetSpotLight()->ImGui("SpotLight");

	ImGui::Begin("Enemy");
	ImGui::SliderFloat3("WTFT", &worldTransform_.translation.x, -100.0f, 100.0f);
	ImGui::SliderFloat3("WTFR", &worldTransform_.rotation.x, 0.0f, 16.0f);
	ImGui::Text("isGuard %d", isGuard_);
	ImGui::Text("isHit %d", isHit_);
	ImGui::End();

	//worldTransformの更新
	worldTransform_.UpdateMatrixEuler();
}

void Enemy::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, camera, animationIndex_);
}

void Enemy::BoneDraw(const Camera& camera)
{
	model_->BoneDraw(worldTransform_, camera, animationIndex_);
}

void Enemy::DrawSprite()
{
	if (hp_ >= 0)
	{
		hpBar_.sprite_->Draw();
	}

	guardGaugeBar_.sprite_->Draw();

	finisherGaugeBar_.sprite_->Draw();

	if (comboCount_ >= 2)
	{
		hitSprite_->Draw();
		comboNumSprite_->Draw();
	}
}

void Enemy::DrawParticle(const Camera& camera)
{
	particleModel_->Draw(particleSystem_.get(), camera);
}

void Enemy::BehaviorRootInitialize()
{
	animationIndex_ = 4;
}

void Enemy::BehaviorRootUpdate()
{
	float animationTime = 0.0f;

	patternCount_ = 1;

	//移動処理
	if (patternCount_ == 1 && isDown_ == false && comboCount_ == 0)
	{
		moveTimer_--;

		bool isFrontMove_ = false;
		bool isBackMove_ = false;
		velocity_ = { 0.0f, 0.0f, 0.0f };

		if (moveTimer_ <= 30 && enemyDirection_ == Direction::Left && !isHit_)
		{
			velocity_.x = -0.01f;
			isFrontMove_ = false;
			isBackMove_ = true;
			isGuard_ = false;
		}

		if (moveTimer_ <= 30 && enemyDirection_ == Direction::Right && !isHit_)
		{
			velocity_.x = 0.01f;
			isFrontMove_ = true;
			isBackMove_ = false;
			isGuard_ = false;
		}

		if (moveTimer_ > 30 && enemyDirection_ == Direction::Right)
		{
			velocity_.x = -0.01f;
			isFrontMove_ = false;
			isBackMove_ = true;
			isGuard_ = true;
		}

		if (moveTimer_ > 30 && enemyDirection_ == Direction::Left)
		{
			velocity_.x = 0.01f;
			isFrontMove_ = true;
			isBackMove_ = false;
			isGuard_ = true;
		}

		//移動
		if (isFrontMove_)
		{
			animationIndex_ = 0;

			UpdateAnimationTime(animationTime, true, 30.0f, animationIndex_, model_);

			velocity_ = Normalize(velocity_);
			velocity_ = Multiply(frontSpeed_, velocity_);

			// 平行移動
			worldTransform_.translation = Add(worldTransform_.translation, velocity_);

			worldTransform_.UpdateMatrixEuler();
		}
		else if (isBackMove_)
		{
			animationIndex_ = 1;

			UpdateAnimationTime(animationTime, true, 40.0f, animationIndex_, model_);

			velocity_ = Normalize(velocity_);
			velocity_ = Multiply(backSpeed_, velocity_);

			// 平行移動
			worldTransform_.translation = Add(worldTransform_.translation, velocity_);

			worldTransform_.UpdateMatrixEuler();
		}
		else
		{
			animationIndex_ = 4;

			UpdateAnimationTime(animationTime, true, 60.0f, animationIndex_, model_);
		}

		if (moveTimer_ <= 0)
		{
			moveTimer_ = Random(30, 90);
			patternCount_ = 2;
		}
	}

	//攻撃
	//突進攻撃
	if (patternCount_ == 2 && !isDown_)
	{
		behaviorRequest_ = Behavior::kAttack;
		animationTime = 0.0f;
		model_->SetAnimationTime(animationTime);
		workAttack_.isTackle = true;
	}

	//ジャンプ
	if (patternCount_ == 3 && !isDown_)
	{
		behaviorRequest_ = Behavior::kJump;
	}
}

void Enemy::BehaviorAttackInitialize()
{
	attackAnimationFrame_ = 0;
}

void Enemy::BehaviorAttackUpdate()
{
	//タックル攻撃
	if (workAttack_.isTackle)
	{
		animationIndex_ = 7;
		isGuard_ = false;
		float animationTime = 0.0f;
		float animationDuration;
		animationTime = model_->GetAnimationTime();
		animationDuration = model_->GetAnimation()[animationIndex_].duration;

		float particlePositionX = 0.0f;

		if (!isDown_)
		{
			animationTime += 1.0f / 40.0f;
		}

		model_->SetAnimationTime(animationTime);
		model_->ApplyAnimation(animationIndex_);

		if (enemyDirection_ == Direction::Right)
		{
			aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
			SetAABB(aabb_);

			if (attackAnimationFrame_ >= 25 && attackAnimationFrame_ < 40)
			{
				workAttack_.isAttack = true;
				worldTransform_.translation.x += 0.15f;
			}

			if (attackAnimationFrame_ >= 25 && attackAnimationFrame_ < 60)
			{
				particlePositionX = 0.1f;
				particlePositionX += 0.3f;

				ParticleEmitter* newParticleEmitter = EmitterBuilder()
					.SetParticleType(ParticleEmitter::ParticleType::kNormal)
					.SetTranslation({ worldTransform_.translation.x + particlePositionX,
							worldTransform_.translation.y + 0.6f,  worldTransform_.translation.z })
					.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
					.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
					.SetScale({ 0.1f, 0.1f, 0.1f }, { 0.4f ,0.4f ,0.4f })
					.SetAzimuth(172.0f, 180.0f)
					.SetElevation(0.0f, 0.0f)
					.SetVelocity({ 0.08f ,0.08f ,0.08f }, { 0.1f ,0.1f ,0.1f })
					.SetColor({ 0.0f ,0.0f ,1.0f ,1.0f }, { 0.0f ,0.5f ,1.0f ,1.0f })
					.SetLifeTime(0.1f, 0.6f)
					.SetCount(100)
					.SetFrequency(4.0f)
					.SetDeleteTime(2.0f)
					.Build();
				particleSystem_->AddParticleEmitter(newParticleEmitter);
			}
		}
		else if (enemyDirection_ == Direction::Left)
		{
			aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
			SetAABB(aabb_);

			if (attackAnimationFrame_ >= 25 && attackAnimationFrame_ < 40)
			{
				workAttack_.isAttack = true;
				worldTransform_.translation.x -= 0.15f;
			}

			if (attackAnimationFrame_ >= 25 && attackAnimationFrame_ < 60)
			{
				particlePositionX = 0.1f;
				particlePositionX += 0.3f;

				ParticleEmitter* newParticleEmitter = EmitterBuilder()
					.SetParticleType(ParticleEmitter::ParticleType::kNormal)
					.SetTranslation({ worldTransform_.translation.x - particlePositionX,
							worldTransform_.translation.y + 0.6f,  worldTransform_.translation.z })
					.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
					.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
					.SetScale({ 0.1f, 0.1f, 0.1f }, { 0.4f ,0.4f ,0.4f })
					.SetAzimuth(0.0f, 8.0f)
					.SetElevation(0.0f, 0.0f)
					.SetVelocity({ 0.08f ,0.08f ,0.08f }, { 0.1f ,0.1f ,0.1f })
					.SetColor({ 0.0f ,0.0f ,1.0f ,1.0f }, { 0.0f ,0.5f ,1.0f ,1.0f })
					.SetLifeTime(0.1f, 0.6f)
					.SetCount(100)
					.SetFrequency(4.0f)
					.SetDeleteTime(2.0f)
					.Build();
				particleSystem_->AddParticleEmitter(newParticleEmitter);
			}
		}

		if (attackAnimationFrame_ >= 55)
		{
			workAttack_.isAttack = false;
		}

		if (isDown_ || animationTime >= animationDuration)
		{
			patternCount_ = 1;
			behaviorRequest_ = Behavior::kRoot;
			workAttack_.isAttack = false;
			workAttack_.isTackle = false;
			animationTime = 0.0f;
			attackAnimationFrame_ = 0;
			model_->SetAnimationTime(animationTime);
			aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
			SetAABB(aabb_);
		}

		attackAnimationFrame_++;
	}
}

void Enemy::BehaviorJumpInitialize()
{
	worldTransform_.translation.y = 0.0f;

	const float kJumpFirstSpeed_ = 0.3f;

	velocity_.y = kJumpFirstSpeed_;
}

void Enemy::BehaviorJumpUpdate()
{
	float animationTime = 0.0f;
	animationIndex_ = 4;

	UpdateAnimationTime(animationTime, true, 60.0f, animationIndex_, model_);

	worldTransform_.translation = Add(worldTransform_.translation, velocity_);

	const float kGravityAcceleration_ = 0.02f;

	Vector3 accelerationVector_ = { 0.0f,-kGravityAcceleration_,0.0f };

	velocity_ = Add(velocity_, accelerationVector_);

	if (worldTransform_.translation.y <= 0.0f)
	{
		behaviorRequest_ = Behavior::kRoot;
		//workAttack_.isJumpAttack = false;
		worldTransform_.translation.y = 0.0f;
		animationTime = 0.0f;
		model_->SetAnimationTime(animationTime);
	}
}

void Enemy::BehaviorThrowInitialize()
{
	attackAnimationFrame_ = 0;
}

void Enemy::BehaviorThrowUpdate()
{

}

void Enemy::BehaviorStanInitialize()
{
	animationIndex_ = 8;
}

void Enemy::BehaviorStanUpdate()
{
	animationIndex_ = 8;
	float animationTime = 0.0f;
	float animationDuration;
	animationTime = model_->GetAnimationTime();
	animationDuration = model_->GetAnimation()[animationIndex_].duration;

	if (enemyDirection_ == Direction::Left)
	{
		aabb_ = { {-0.6f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
		SetAABB(aabb_);
	}
	else if(enemyDirection_ == Direction::Right)
	{
		aabb_ = { {-0.3f,-0.3f,-0.3f},{0.6f,0.3f,0.3f} };
		SetAABB(aabb_);
	}

	if (!isDown_)
	{
		animationTime += 1.0f / 60.0f;
	}

	model_->SetAnimationTime(animationTime);
	model_->ApplyAnimation(animationIndex_);

	if (animationTime >= animationDuration || isDown_)
	{
		behaviorRequest_ = Behavior::kRoot;
		animationTime = 0.0f;
		attackAnimationFrame_ = 0;
		guardGauge_ = 0.0f;
		model_->SetAnimationTime(animationTime);
		aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
		SetAABB(aabb_);
	}
}

void Enemy::UpdateAnimationTime(float animationTime, bool isLoop, float frameRate, int animationIndex, std::unique_ptr<Model>& modelFighterBody)
{
	animationTime = modelFighterBody->GetAnimationTime();

	if (!isDown_)
	{
		animationTime += 1.0f / frameRate;

		if (isLoop)
		{
			animationTime = std::fmod(animationTime, modelFighterBody->GetAnimation()[animationIndex].duration);
		}
	}

	modelFighterBody->SetAnimationTime(animationTime);
	modelFighterBody->ApplyAnimation(animationIndex);
	modelFighterBody->Update();
}

void Enemy::OnCollision(Collider* collider, float damage)
{
	//プレイヤーの近接攻撃との当たり判定
	if (collider->GetCollisionAttribute() & kCollisionAttributePlayer)
	{
		isHit_ = true;

		if (player_->GetIsAttack() && !player_->GetIsTackle() && isGuard_ && enemyDirection_ == Direction::Right)
		{
			guardAnimationTimer_--;

			audio_->SoundPlayMP3(guardSoundHandle_, false, 1.0f);
			worldTransform_.translation.x -= 0.3f;
			guardGauge_ += 1.0f;

			if (guardAnimationTimer_ > 55)
			{

				ParticleEmitter* newParticleEmitter = EmitterBuilder()
					.SetParticleType(ParticleEmitter::ParticleType::kNormal)
					.SetTranslation({ worldTransform_.translation.x + 0.1f,
					worldTransform_.translation.y + 0.5f,  worldTransform_.translation.z })
					.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
					.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
					.SetScale({ 0.1f, 0.1f,0.1f }, { 0.2f ,0.2f ,0.2f })
					.SetAzimuth(0.0f, 360.0f)
					.SetElevation(0.0f, 0.0f)
					.SetVelocity({ 0.03f ,0.03f ,0.03f }, { 0.06f ,0.06f ,0.06f })
					.SetColor({ 1.0f ,1.0f ,1.0f ,1.0f }, { 1.0f ,1.0f ,1.0f ,1.0f })
					.SetLifeTime(0.1f, 1.0f)
					.SetCount(100)
					.SetFrequency(4.0f)
					.SetDeleteTime(2.0f)
					.Build();
				particleSystem_->AddParticleEmitter(newParticleEmitter);
			}
		}

		if (player_->GetIsAttack() && !player_->GetIsTackle() && isGuard_ && enemyDirection_ == Direction::Left)
		{
			guardAnimationTimer_--;

			audio_->SoundPlayMP3(guardSoundHandle_, false, 1.0f);
			worldTransform_.translation.x += 0.3f;
			guardGauge_ += 1.0f;

			if (guardAnimationTimer_ > 55)
			{

				ParticleEmitter* newParticleEmitter = EmitterBuilder()
					.SetParticleType(ParticleEmitter::ParticleType::kNormal)
					.SetTranslation({ worldTransform_.translation.x - 0.1f,
					worldTransform_.translation.y + 0.5f,  worldTransform_.translation.z })
					.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
					.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
					.SetScale({ 0.1f, 0.1f,0.1f }, { 0.2f ,0.2f ,0.2f })
					.SetAzimuth(0.0f, 360.0f)
					.SetElevation(0.0f, 0.0f)
					.SetVelocity({ 0.03f ,0.03f ,0.03f }, { 0.06f ,0.06f ,0.06f })
					.SetColor({ 1.0f ,1.0f ,1.0f ,1.0f }, { 1.0f ,1.0f ,1.0f ,1.0f })
					.SetLifeTime(0.1f, 1.0f)
					.SetCount(100)
					.SetFrequency(4.0f)
					.SetDeleteTime(2.0f)
					.Build();
				particleSystem_->AddParticleEmitter(newParticleEmitter);
			}
		}

		if (player_->GetIsAttack() && player_->GetIsTackle() && isGuard_ && enemyDirection_ == Direction::Right)
		{
			guardAnimationTimer_--;

			audio_->SoundPlayMP3(guardSoundHandle_, false, 1.0f);
			worldTransform_.translation.x -= 0.2f;
			guardGauge_ += 1.0f;

			if (guardAnimationTimer_ > 55)
			{

				ParticleEmitter* newParticleEmitter = EmitterBuilder()
					.SetParticleType(ParticleEmitter::ParticleType::kNormal)
					.SetTranslation({ worldTransform_.translation.x + 0.1f,
					worldTransform_.translation.y + 0.5f,  worldTransform_.translation.z })
					.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
					.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
					.SetScale({ 0.1f, 0.1f,0.1f }, { 0.2f ,0.2f ,0.2f })
					.SetAzimuth(0.0f, 360.0f)
					.SetElevation(0.0f, 0.0f)
					.SetVelocity({ 0.03f ,0.03f ,0.03f }, { 0.06f ,0.06f ,0.06f })
					.SetColor({ 1.0f ,1.0f ,1.0f ,1.0f }, { 1.0f ,1.0f ,1.0f ,1.0f })
					.SetLifeTime(0.1f, 1.0f)
					.SetCount(100)
					.SetFrequency(4.0f)
					.SetDeleteTime(2.0f)
					.Build();
				particleSystem_->AddParticleEmitter(newParticleEmitter);
			}
		}

		if (player_->GetIsAttack() && player_->GetIsTackle() && isGuard_ && enemyDirection_ == Direction::Left)
		{
			guardAnimationTimer_--;

			audio_->SoundPlayMP3(guardSoundHandle_, false, 1.0f);
			worldTransform_.translation.x += 0.2f;
			guardGauge_ += 1.0f;

			if (guardAnimationTimer_ > 55)
			{

				ParticleEmitter* newParticleEmitter = EmitterBuilder()
					.SetParticleType(ParticleEmitter::ParticleType::kNormal)
					.SetTranslation({ worldTransform_.translation.x - 0.1f,
					worldTransform_.translation.y + 0.5f,  worldTransform_.translation.z })
					.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
					.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
					.SetScale({ 0.1f, 0.1f,0.1f }, { 0.2f ,0.2f ,0.2f })
					.SetAzimuth(0.0f, 360.0f)
					.SetElevation(0.0f, 0.0f)
					.SetVelocity({ 0.03f ,0.03f ,0.03f }, { 0.06f ,0.06f ,0.06f })
					.SetColor({ 1.0f ,1.0f ,1.0f ,1.0f }, { 1.0f ,1.0f ,1.0f ,1.0f })
					.SetLifeTime(0.1f, 1.0f)
					.SetCount(100)
					.SetFrequency(4.0f)
					.SetDeleteTime(2.0f)
					.Build();
				particleSystem_->AddParticleEmitter(newParticleEmitter);
			}
		}

		//弱パンチ
		if (player_->GetIsLightPunch() && !isDown_ && !isGuard_)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 2.0f;
			hp_ -= damage;
			isHitLightPunch_ = true;

			HitStop(10);
		}

		//中パンチ
		if (player_->GetIsMiddlePunch() && !isDown_ && !isGuard_)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 5.0f;
			hp_ -= damage;
			isHitMiddlePunch_ = true;

			HitStop(10);
		}

		//強パンチ
		if (player_->GetIsHighPunch() && !isDown_ && !isGuard_)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 10.0f;
			hp_ -= damage;
			isHitHighPunch_ = true;

			HitStop(10);
		}

		//TC中パンチ
		if (player_->GetIsTCMiddlePunch() && !isDown_ && !isGuard_)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 2.0f;
			hp_ -= damage;
			isHitTCMiddlePunch_ = true;

			HitStop(10);
		}

		//TC強パンチ
		if (player_->GetIsTCHighPunch() && !isDown_ && !isGuard_)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 2.0f;
			hp_ -= damage;
			isHitTCHighPunch_ = true;

			HitStop(10);
		}

		//タックル
		//キャンセルじゃないとき
		if (player_->GetIsTackle() && player_->GetIsAttack() && !isDown_ && !isGuard_)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 15.0f;
			hp_ -= damage;
			isHitTackle_ = true;

			HitStop(30);
		}

		//キャンセルのとき
		if (player_->GetIsTackle() && player_->GetIsAttack() && isDown_ && !isGuard_ && worldTransform_.translation.y > 0.5f)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 4.0f;
			hp_ -= damage;
			downAnimationTimer_ = 60;
			float animationTime = 0.0f;
			model_->SetAnimationTime(animationTime);
			isHitHighPunch_ = false;
			isHitTackle_ = true;

			HitStop(10);
		}
	}
}

void Enemy::HPBarUpdate()
{
	hpBar_.size_ = { (hp_ / maxHp_) * barSize_,7.0f };

	hpBar_.sprite_->SetSize(hpBar_.size_);

	if (hp_ > 50)
	{
		hpBar_.sprite_->SetColor({ 0.0f, 1.0f, 0.0f, 1.0f });
	}

	if (hp_ <= 50 && hp_ > 25)
	{
		hpBar_.sprite_->SetColor({ 1.0f, 0.8f, 0.0f, 1.0f });
	}
	else if (hp_ <= 25)
	{
		hpBar_.sprite_->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });
	}
}

void Enemy::GuardGaugeBarUpdate()
{
	if (guardGauge_ > 0 && guardGauge_ < 50.0f)
	{
		guardGauge_ -= 0.03f;
	}

	guardGaugeBar_.size_ = { (guardGauge_ / maxGuardGauge_) * guardGaugeBarSize_,7.0f };

	guardGaugeBar_.sprite_->SetSize(guardGaugeBar_.size_);

	guardGaugeBar_.sprite_->SetColor({ 0.0f, 0.5f, 1.0f, 1.0f });

	if (guardGauge_ >= 50.0f)
	{
		guardGauge_ = 50.0f;
		isGuard_ = false;
		workAttack_.isAttack = false;
		behaviorRequest_ = Behavior::kStan;
	}
}

void Enemy::FinisherGaugeBarUpdate()
{
	finisherGaugeBar_.size_ = { (finisherGauge_ / maxFinisherGauge_) * finisherGaugeBarSize_,20.0f };

	finisherGaugeBar_.sprite_->SetSize(finisherGaugeBar_.size_);

	if (finisherGauge_ < maxFinisherGauge_)
	{
		finisherGaugeBar_.sprite_->SetColor({ 0.0f, 0.5f, 1.0f, 1.0f });
	}
	else
	{
		finisherGaugeBar_.sprite_->SetColor({ 1.0f, 0.5f, 0.0f, 1.0f });
	}

	if (finisherGauge_ >= maxFinisherGauge_)
	{
		finisherGauge_ = 50.0f;
	}
}

void Enemy::Reset()
{
	hp_ = maxHp_;

	guardGauge_ = 0.0f;

	finisherGauge_ = 0.0f;

	downAnimationTimer_ = 60;

	isHitLightPunch_ = false;
	isHitMiddlePunch_ = false;
	isHitHighPunch_ = false;
	isHitTCMiddlePunch_ = false;
	isHitTCHighPunch_ = false;
	isHitTackle_ = false;
	isHitThrow_ = false;
	isDown_ = false;

	isHit_ = false;

	workAttack_.isAttack = false;
	workAttack_.isLightPunch = false;
	workAttack_.isMiddlePunch = false;
	workAttack_.isHighPunch = false;
	workAttack_.isTCMiddlePunch = false;
	workAttack_.isTCHighPunch = false;
	workAttack_.isTackle = false;
	workAttack_.isFinisher = false;
	workAttack_.isJumpAttack = false;

	isThrow_ = false;

	finisherEffectTimer_ = 90;
	isFinisherEffect_ = false;
	finisherCount_ = 0;

	animationIndex_ = 4;

	attackAnimationFrame_ = 0;

	behavior_ = Behavior::kRoot;

	worldTransform_.translation = { 3.0f,0.0f,0.0f };
	enemyDirection_ = Direction::Left;

	comboCount_ = 0;

	worldTransform_.UpdateMatrixEuler();

	isReset_ = false;
}

void Enemy::HitStop(int milliseconds)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void Enemy::DownAnimation()
{
	//弱攻撃
	if (isHitLightPunch_ && enemyDirection_ == Direction::Right)
	{
		isDown_ = true;
		downAnimationTimer_--;

		if (downAnimationTimer_ > 55)
		{
			ParticleEmitter* newParticleEmitter = EmitterBuilder()
				.SetParticleType(ParticleEmitter::ParticleType::kNormal)
				.SetTranslation({ worldTransform_.translation.x - 0.1f,
					worldTransform_.translation.y + 0.5f,  worldTransform_.translation.z })
				.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetScale({ 0.1f, 0.1f,0.1f }, { 0.2f ,0.2f ,0.2f })
				.SetAzimuth(0.0f, 360.0f)
				.SetElevation(0.0f, 0.0f)
				.SetVelocity({ 0.03f ,0.03f ,0.03f }, { 0.06f ,0.06f ,0.06f })
				.SetColor({ 1.0f ,0.5f ,0.0f ,1.0f }, { 1.0f ,0.5f ,0.0f ,1.0f })
				.SetLifeTime(0.1f, 1.0f)
				.SetCount(50)
				.SetFrequency(4.0f)
				.SetDeleteTime(1.0f)
				.Build();
			particleSystem_->AddParticleEmitter(newParticleEmitter);
		}

		animationIndex_ = 3;
		float animationTime = 0.0f;
		float animationDuration;
		animationTime = model_->GetAnimationTime();
		animationDuration = model_->GetAnimation()[animationIndex_].duration;

		animationTime += 1.0f / 30.0f;

		model_->SetAnimationTime(animationTime);
		model_->ApplyAnimation(animationIndex_);

		if (!player_->GetIsLightPunch() && hp_ > 0.0f)
		{
			animationIndex_ = 4;
			downAnimationTimer_ = 60;
			animationTime = 0.0f;
			model_->SetAnimationTime(animationTime);
			isHitLightPunch_ = false;
			isDown_ = false;
		}
	}

	if (isHitLightPunch_ && enemyDirection_ == Direction::Left)
	{
		isDown_ = true;
		downAnimationTimer_--;

		if (downAnimationTimer_ > 55)
		{
			ParticleEmitter* newParticleEmitter = EmitterBuilder()
				.SetParticleType(ParticleEmitter::ParticleType::kNormal)
				.SetTranslation({ worldTransform_.translation.x + 0.1f,
					worldTransform_.translation.y + 0.5f,  worldTransform_.translation.z })
				.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetScale({ 0.1f, 0.1f,0.1f }, { 0.2f ,0.2f ,0.2f })
				.SetAzimuth(0.0f, 360.0f)
				.SetElevation(0.0f, 0.0f)
				.SetVelocity({ 0.03f ,0.03f ,0.03f }, { 0.06f ,0.06f ,0.06f })
				.SetColor({ 1.0f ,0.5f ,0.0f ,1.0f }, { 1.0f ,0.5f ,0.0f ,1.0f })
				.SetLifeTime(0.1f, 1.0f)
				.SetCount(50)
				.SetFrequency(4.0f)
				.SetDeleteTime(1.0f)
				.Build();
			particleSystem_->AddParticleEmitter(newParticleEmitter);
		}

		animationIndex_ = 3;
		float animationTime = 0.0f;
		float animationDuration;
		animationTime = model_->GetAnimationTime();
		animationDuration = model_->GetAnimation()[animationIndex_].duration;

		animationTime += 1.0f / 30.0f;

		model_->SetAnimationTime(animationTime);
		model_->ApplyAnimation(animationIndex_);

		if (!player_->GetIsLightPunch() && hp_ > 0.0f)
		{
			animationIndex_ = 4;
			downAnimationTimer_ = 60;
			animationTime = 0.0f;
			model_->SetAnimationTime(animationTime);
			isHitLightPunch_ = false;
			isDown_ = false;
		}
	}

	//中攻撃
	if (isHitMiddlePunch_ && player_->GetRotation().y == 1.7f)
	{
		isDown_ = true;
		downAnimationTimer_--;

		if (downAnimationTimer_ > 55)
		{
			ParticleEmitter* newParticleEmitter = EmitterBuilder()
				.SetParticleType(ParticleEmitter::ParticleType::kNormal)
				.SetTranslation({ worldTransform_.translation.x - 0.1f,
					worldTransform_.translation.y + 0.5f,  worldTransform_.translation.z })
				.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetScale({ 0.1f, 0.1f,0.1f }, { 0.2f ,0.2f ,0.2f })
				.SetAzimuth(0.0f, 360.0f)
				.SetElevation(0.0f, 0.0f)
				.SetVelocity({ 0.03f ,0.03f ,0.03f }, { 0.06f ,0.06f ,0.06f })
				.SetColor({ 1.0f ,0.5f ,0.0f ,1.0f }, { 1.0f ,0.5f ,0.0f ,1.0f })
				.SetLifeTime(0.1f, 1.0f)
				.SetCount(50)
				.SetFrequency(4.0f)
				.SetDeleteTime(1.0f)
				.Build();
			particleSystem_->AddParticleEmitter(newParticleEmitter);
		}

		animationIndex_ = 3;
		float animationTime = 0.0f;
		float animationDuration;
		animationTime = model_->GetAnimationTime();
		animationDuration = model_->GetAnimation()[animationIndex_].duration;

		animationTime += 1.0f / 30.0f;

		model_->SetAnimationTime(animationTime);
		model_->ApplyAnimation(animationIndex_);

		if (!player_->GetIsMiddlePunch() && hp_ > 0.0f)
		{
			animationIndex_ = 4;
			downAnimationTimer_ = 60;
			animationTime = 0.0f;
			model_->SetAnimationTime(animationTime);
			isHitMiddlePunch_ = false;
			isDown_ = false;
		}
	}

	if (isHitMiddlePunch_ && player_->GetRotation().y == 4.6f)
	{
		isDown_ = true;
		downAnimationTimer_--;

		if (downAnimationTimer_ > 55)
		{
			ParticleEmitter* newParticleEmitter = EmitterBuilder()
				.SetParticleType(ParticleEmitter::ParticleType::kNormal)
				.SetTranslation({ worldTransform_.translation.x + 0.1f,
					worldTransform_.translation.y + 0.5f,  worldTransform_.translation.z })
				.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetScale({ 0.1f, 0.1f,0.1f }, { 0.2f ,0.2f ,0.2f })
				.SetAzimuth(0.0f, 360.0f)
				.SetElevation(0.0f, 0.0f)
				.SetVelocity({ 0.03f ,0.03f ,0.03f }, { 0.06f ,0.06f ,0.06f })
				.SetColor({ 1.0f ,0.5f ,0.0f ,1.0f }, { 1.0f ,0.5f ,0.0f ,1.0f })
				.SetLifeTime(0.1f, 1.0f)
				.SetCount(50)
				.SetFrequency(4.0f)
				.SetDeleteTime(1.0f)
				.Build();
			particleSystem_->AddParticleEmitter(newParticleEmitter);
		}

		animationIndex_ = 3;
		float animationTime = 0.0f;
		float animationDuration;
		animationTime = model_->GetAnimationTime();
		animationDuration = model_->GetAnimation()[animationIndex_].duration;

		animationTime += 1.0f / 30.0f;

		model_->SetAnimationTime(animationTime);
		model_->ApplyAnimation(animationIndex_);

		if (!player_->GetIsMiddlePunch() && hp_ > 0.0f)
		{
			animationIndex_ = 4;
			downAnimationTimer_ = 60;
			animationTime = 0.0f;
			model_->SetAnimationTime(animationTime);
			isHitMiddlePunch_ = false;
			isDown_ = false;
		}
	}

	//強攻撃
	if (isHitHighPunch_ && player_->GetRotation().y == 1.7f)
	{
		isDown_ = true;
		downAnimationTimer_--;

		if (downAnimationTimer_ > 55)
		{
			ParticleEmitter* newParticleEmitter = EmitterBuilder()
				.SetParticleType(ParticleEmitter::ParticleType::kNormal)
				.SetTranslation({ worldTransform_.translation.x - 0.1f,
					worldTransform_.translation.y + 0.7f,  worldTransform_.translation.z })
				.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetScale({ 0.1f, 0.1f,0.1f }, { 0.2f ,0.2f ,0.2f })
				.SetAzimuth(0.0f, 360.0f)
				.SetElevation(0.0f, 0.0f)
				.SetVelocity({ 0.03f ,0.03f ,0.03f }, { 0.06f ,0.06f ,0.06f })
				.SetColor({ 1.0f ,0.5f ,0.0f ,1.0f }, { 1.0f ,0.5f ,0.0f ,1.0f })
				.SetLifeTime(0.1f, 1.0f)
				.SetCount(50)
				.SetFrequency(4.0f)
				.SetDeleteTime(1.0f)
				.Build();
			particleSystem_->AddParticleEmitter(newParticleEmitter);

			const float kJumpFirstSpeed_ = 0.15f;
			velocity_.x = 0.025f;
			velocity_.y = kJumpFirstSpeed_;
		}
		else if (downAnimationTimer_ <= 55 && downAnimationTimer_ > -30)
		{
			worldTransform_.translation = Add(worldTransform_.translation, velocity_);

			const float kGravityAcceleration_ = 0.005f;

			Vector3 accelerationVector_ = { 0.0f,-kGravityAcceleration_,0.0f };

			velocity_ = Add(velocity_, accelerationVector_);

			if (worldTransform_.translation.y <= 0.0f)
			{
				velocity_.x = 0.0f;
				worldTransform_.translation.y = 0.0f;
			}
		}

		animationIndex_ = 5;
		float animationTime = 0.0f;
		float animationDuration;
		animationTime = model_->GetAnimationTime();
		animationDuration = model_->GetAnimation()[animationIndex_].duration;

		animationTime += 1.0f / 30.0f;

		model_->SetAnimationTime(animationTime);
		model_->ApplyAnimation(animationIndex_);

		if (downAnimationTimer_ <= -30 && worldTransform_.translation.y <= 0.0f && hp_ > 0.0f)
		{
			animationIndex_ = 4;
			downAnimationTimer_ = 60;
			animationTime = 0.0f;
			model_->SetAnimationTime(animationTime);
			isHitHighPunch_ = false;
			isDown_ = false;
		}
	}

	if (isHitHighPunch_ && player_->GetRotation().y == 4.6f)
	{
		isDown_ = true;
		downAnimationTimer_--;

		if (downAnimationTimer_ > 55)
		{
			ParticleEmitter* newParticleEmitter = EmitterBuilder()
				.SetParticleType(ParticleEmitter::ParticleType::kNormal)
				.SetTranslation({ worldTransform_.translation.x + 0.1f,
					worldTransform_.translation.y + 0.7f,  worldTransform_.translation.z })
				.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetScale({ 0.1f, 0.1f,0.1f }, { 0.2f ,0.2f ,0.2f })
				.SetAzimuth(0.0f, 360.0f)
				.SetElevation(0.0f, 0.0f)
				.SetVelocity({ 0.03f ,0.03f ,0.03f }, { 0.06f ,0.06f ,0.06f })
				.SetColor({ 1.0f ,0.5f ,0.0f ,1.0f }, { 1.0f ,0.5f ,0.0f ,1.0f })
				.SetLifeTime(0.1f, 1.0f)
				.SetCount(50)
				.SetFrequency(4.0f)
				.SetDeleteTime(1.0f)
				.Build();
			particleSystem_->AddParticleEmitter(newParticleEmitter);

			const float kJumpFirstSpeed_ = 0.15f;
			velocity_.x = -0.025f;
			velocity_.y = kJumpFirstSpeed_;
		}
		else if (downAnimationTimer_ <= 55 && downAnimationTimer_ > -30)
		{
			worldTransform_.translation = Add(worldTransform_.translation, velocity_);

			const float kGravityAcceleration_ = 0.005f;

			Vector3 accelerationVector_ = { 0.0f,-kGravityAcceleration_,0.0f };

			velocity_ = Add(velocity_, accelerationVector_);

			if (worldTransform_.translation.y <= 0.0f)
			{
				velocity_.x = 0.0f;
				worldTransform_.translation.y = 0.0f;
			}
		}

		animationIndex_ = 5;
		float animationTime = 0.0f;
		float animationDuration;
		animationTime = model_->GetAnimationTime();
		animationDuration = model_->GetAnimation()[animationIndex_].duration;

		animationTime += 1.0f / 30.0f;

		model_->SetAnimationTime(animationTime);
		model_->ApplyAnimation(animationIndex_);

		if (downAnimationTimer_ <= -30 && worldTransform_.translation.y <= 0.0f && hp_ > 0.0f)
		{
			animationIndex_ = 4;
			downAnimationTimer_ = 60;
			animationTime = 0.0f;
			model_->SetAnimationTime(animationTime);
			isHitHighPunch_ = false;
			isDown_ = false;
		}
	}

	//TC中攻撃
	if (isHitTCMiddlePunch_ && player_->GetRotation().y == 1.7f)
	{
		isDown_ = true;
		downAnimationTimer_--;

		if (downAnimationTimer_ > 55)
		{
			ParticleEmitter* newParticleEmitter = EmitterBuilder()
				.SetParticleType(ParticleEmitter::ParticleType::kNormal)
				.SetTranslation({ worldTransform_.translation.x - 0.1f,
					worldTransform_.translation.y + 0.5f,  worldTransform_.translation.z })
				.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetScale({ 0.1f, 0.1f,0.1f }, { 0.2f ,0.2f ,0.2f })
				.SetAzimuth(0.0f, 360.0f)
				.SetElevation(0.0f, 0.0f)
				.SetVelocity({ 0.03f ,0.03f ,0.03f }, { 0.06f ,0.06f ,0.06f })
				.SetColor({ 1.0f ,0.5f ,0.0f ,1.0f }, { 1.0f ,0.5f ,0.0f ,1.0f })
				.SetLifeTime(0.1f, 1.0f)
				.SetCount(50)
				.SetFrequency(4.0f)
				.SetDeleteTime(1.0f)
				.Build();
			particleSystem_->AddParticleEmitter(newParticleEmitter);
		}

		animationIndex_ = 3;
		float animationTime = 0.0f;
		float animationDuration;
		animationTime = model_->GetAnimationTime();
		animationDuration = model_->GetAnimation()[animationIndex_].duration;

		animationTime += 1.0f / 30.0f;

		model_->SetAnimationTime(animationTime);
		model_->ApplyAnimation(animationIndex_);

		if (!player_->GetIsTCMiddlePunch() && hp_ > 0.0f)
		{
			animationIndex_ = 4;
			downAnimationTimer_ = 60;
			animationTime = 0.0f;
			model_->SetAnimationTime(animationTime);
			isHitTCMiddlePunch_ = false;
			isDown_ = false;
		}
	}

	if (isHitTCMiddlePunch_ && player_->GetRotation().y == 4.6f)
	{
		isDown_ = true;
		downAnimationTimer_--;

		if (downAnimationTimer_ > 55)
		{
			ParticleEmitter* newParticleEmitter = EmitterBuilder()
				.SetParticleType(ParticleEmitter::ParticleType::kNormal)
				.SetTranslation({ worldTransform_.translation.x + 0.1f,
					worldTransform_.translation.y + 0.5f,  worldTransform_.translation.z })
				.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetScale({ 0.1f, 0.1f,0.1f }, { 0.2f ,0.2f ,0.2f })
				.SetAzimuth(0.0f, 360.0f)
				.SetElevation(0.0f, 0.0f)
				.SetVelocity({ 0.03f ,0.03f ,0.03f }, { 0.06f ,0.06f ,0.06f })
				.SetColor({ 1.0f ,0.5f ,0.0f ,1.0f }, { 1.0f ,0.5f ,0.0f ,1.0f })
				.SetLifeTime(0.1f, 1.0f)
				.SetCount(50)
				.SetFrequency(4.0f)
				.SetDeleteTime(1.0f)
				.Build();
			particleSystem_->AddParticleEmitter(newParticleEmitter);
		}

		animationIndex_ = 3;
		float animationTime = 0.0f;
		float animationDuration;
		animationTime = model_->GetAnimationTime();
		animationDuration = model_->GetAnimation()[animationIndex_].duration;

		animationTime += 1.0f / 30.0f;

		model_->SetAnimationTime(animationTime);
		model_->ApplyAnimation(animationIndex_);

		if (!player_->GetIsTCMiddlePunch() && hp_ > 0.0f)
		{
			animationIndex_ = 4;
			downAnimationTimer_ = 60;
			animationTime = 0.0f;
			model_->SetAnimationTime(animationTime);
			isHitTCMiddlePunch_ = false;
			isDown_ = false;
		}
	}

	//TC強攻撃
	if (isHitTCHighPunch_ && player_->GetRotation().y == 1.7f)
	{
		isDown_ = true;
		downAnimationTimer_--;

		if (downAnimationTimer_ > 55)
		{
			ParticleEmitter* newParticleEmitter = EmitterBuilder()
				.SetParticleType(ParticleEmitter::ParticleType::kNormal)
				.SetTranslation({ worldTransform_.translation.x - 0.1f,
					worldTransform_.translation.y + 0.5f,  worldTransform_.translation.z })
				.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetScale({ 0.1f, 0.1f,0.1f }, { 0.2f ,0.2f ,0.2f })
				.SetAzimuth(0.0f, 360.0f)
				.SetElevation(0.0f, 0.0f)
				.SetVelocity({ 0.03f ,0.03f ,0.03f }, { 0.06f ,0.06f ,0.06f })
				.SetColor({ 1.0f ,0.5f ,0.0f ,1.0f }, { 1.0f ,0.5f ,0.0f ,1.0f })
				.SetLifeTime(0.1f, 1.0f)
				.SetCount(50)
				.SetFrequency(4.0f)
				.SetDeleteTime(1.0f)
				.Build();
			particleSystem_->AddParticleEmitter(newParticleEmitter);
		}

		if (downAnimationTimer_ > 35 && worldTransform_.translation.x < 4.0f)
		{
			worldTransform_.translation.x += 0.02f;

		}

		animationIndex_ = 5;
		float animationTime = 0.0f;
		float animationDuration;
		animationTime = model_->GetAnimationTime();
		animationDuration = model_->GetAnimation()[animationIndex_].duration;

		animationTime += 1.0f / 30.0f;

		model_->SetAnimationTime(animationTime);
		model_->ApplyAnimation(animationIndex_);

		aabb_ = { {0.1f,-0.3f,-0.3f},{0.8f,0.0f,0.3f} };
		SetAABB(aabb_);

		if (!player_->GetIsTCHighPunch() && hp_ > 0.0f)
		{
			animationIndex_ = 4;
			downAnimationTimer_ = 60;
			animationTime = 0.0f;
			model_->SetAnimationTime(animationTime);
			aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
			SetAABB(aabb_);
			isHitTCHighPunch_ = false;
			isDown_ = false;
		}
	}

	if (isHitTCHighPunch_ && player_->GetRotation().y == 4.6f)
	{
		isDown_ = true;
		downAnimationTimer_--;

		if (downAnimationTimer_ > 55)
		{
			ParticleEmitter* newParticleEmitter = EmitterBuilder()
				.SetParticleType(ParticleEmitter::ParticleType::kNormal)
				.SetTranslation({ worldTransform_.translation.x + 0.1f,
					worldTransform_.translation.y + 0.5f,  worldTransform_.translation.z })
				.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetScale({ 0.1f, 0.1f,0.1f }, { 0.2f ,0.2f ,0.2f })
				.SetAzimuth(0.0f, 360.0f)
				.SetElevation(0.0f, 0.0f)
				.SetVelocity({ 0.03f ,0.03f ,0.03f }, { 0.06f ,0.06f ,0.06f })
				.SetColor({ 1.0f ,0.5f ,0.0f ,1.0f }, { 1.0f ,0.5f ,0.0f ,1.0f })
				.SetLifeTime(0.1f, 1.0f)
				.SetCount(50)
				.SetFrequency(4.0f)
				.SetDeleteTime(1.0f)
				.Build();
			particleSystem_->AddParticleEmitter(newParticleEmitter);
		}

		if (downAnimationTimer_ > 35 && worldTransform_.translation.x > -4.0f)
		{
			worldTransform_.translation.x -= 0.02f;

		}

		animationIndex_ = 5;
		float animationTime = 0.0f;
		float animationDuration;
		animationTime = model_->GetAnimationTime();
		animationDuration = model_->GetAnimation()[animationIndex_].duration;

		animationTime += 1.0f / 30.0f;

		model_->SetAnimationTime(animationTime);
		model_->ApplyAnimation(animationIndex_);

		aabb_ = { {-0.8f,-0.3f,-0.3f},{-0.1f,0.0f,0.3f} };
		SetAABB(aabb_);

		if (!player_->GetIsTCHighPunch() && hp_ > 0.0f)
		{
			animationIndex_ = 4;
			downAnimationTimer_ = 60;
			animationTime = 0.0f;
			model_->SetAnimationTime(animationTime);
			aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
			SetAABB(aabb_);
			isHitTCHighPunch_ = false;
			isDown_ = false;
		}
	}

	//タックル攻撃
	if (isHitTackle_ && player_->GetRotation().y == 1.7f)
	{
		isDown_ = true;
		downAnimationTimer_--;

		if (downAnimationTimer_ > 55)
		{
			ParticleEmitter* newParticleEmitter = EmitterBuilder()
				.SetParticleType(ParticleEmitter::ParticleType::kNormal)
				.SetTranslation({ worldTransform_.translation.x - 0.1f,
					worldTransform_.translation.y + 0.5f,  worldTransform_.translation.z })
				.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetScale({ 0.1f, 0.1f,0.1f }, { 0.2f ,0.2f ,0.2f })
				.SetAzimuth(0.0f, 360.0f)
				.SetElevation(0.0f, 0.0f)
				.SetVelocity({ 0.03f ,0.03f ,0.03f }, { 0.06f ,0.06f ,0.06f })
				.SetColor({ 1.0f ,0.5f ,0.0f ,1.0f }, { 1.0f ,0.5f ,0.0f ,1.0f })
				.SetLifeTime(0.1f, 1.0f)
				.SetCount(50)
				.SetFrequency(4.0f)
				.SetDeleteTime(1.0f)
				.Build();
			particleSystem_->AddParticleEmitter(newParticleEmitter);
		}

		if (downAnimationTimer_ > 35 && worldTransform_.translation.x < 4.0f)
		{
			worldTransform_.translation.x += 0.08f;

		}

		if (worldTransform_.translation.y > 0.0f)
		{
			worldTransform_.translation.y -= 0.03f;
		}
		else if (worldTransform_.translation.y <= 0.0f)
		{
			worldTransform_.translation.y = 0.0f;
		}

		animationIndex_ = 5;
		float animationTime = 0.0f;
		float animationDuration;
		animationTime = model_->GetAnimationTime();
		animationDuration = model_->GetAnimation()[animationIndex_].duration;

		animationTime += 1.0f / 30.0f;

		model_->SetAnimationTime(animationTime);
		model_->ApplyAnimation(animationIndex_);

		aabb_ = { {0.1f,-0.3f,-0.3f},{0.8f,0.0f,0.3f} };
		SetAABB(aabb_);

		if (!player_->GetIsTackle() && hp_ > 0.0f)
		{
			animationIndex_ = 4;
			downAnimationTimer_ = 60;
			animationTime = 0.0f;
			model_->SetAnimationTime(animationTime);
			aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
			SetAABB(aabb_);
			isHitTackle_ = false;
			isDown_ = false;
		}
	}

	if (isHitTackle_ && player_->GetRotation().y == 4.6f)
	{
		isDown_ = true;
		downAnimationTimer_--;

		if (downAnimationTimer_ > 55)
		{
			ParticleEmitter* newParticleEmitter = EmitterBuilder()
				.SetParticleType(ParticleEmitter::ParticleType::kNormal)
				.SetTranslation({ worldTransform_.translation.x + 0.1f,
					worldTransform_.translation.y + 0.5f,  worldTransform_.translation.z })
				.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetScale({ 0.1f, 0.1f,0.1f }, { 0.2f ,0.2f ,0.2f })
				.SetAzimuth(0.0f, 360.0f)
				.SetElevation(0.0f, 0.0f)
				.SetVelocity({ 0.03f ,0.03f ,0.03f }, { 0.06f ,0.06f ,0.06f })
				.SetColor({ 1.0f ,0.5f ,0.0f ,1.0f }, { 1.0f ,0.5f ,0.0f ,1.0f })
				.SetLifeTime(0.1f, 1.0f)
				.SetCount(50)
				.SetFrequency(4.0f)
				.SetDeleteTime(1.0f)
				.Build();
			particleSystem_->AddParticleEmitter(newParticleEmitter);
		}

		if (downAnimationTimer_ > 35 && worldTransform_.translation.x > -4.0f)
		{
			worldTransform_.translation.x -= 0.08f;

		}


		if (worldTransform_.translation.y > 0.0f)
		{
			worldTransform_.translation.y -= 0.03f;
		}
		else if (worldTransform_.translation.y <= 0.0f)
		{
			worldTransform_.translation.y = 0.0f;
		}

		animationIndex_ = 5;
		float animationTime = 0.0f;
		float animationDuration;
		animationTime = model_->GetAnimationTime();
		animationDuration = model_->GetAnimation()[animationIndex_].duration;

		animationTime += 1.0f / 30.0f;

		model_->SetAnimationTime(animationTime);
		model_->ApplyAnimation(animationIndex_);

		aabb_ = { {-0.8f,-0.3f,-0.3f},{-0.1f,0.0f,0.3f} };
		SetAABB(aabb_);

		if (!player_->GetIsTackle() && hp_ > 0.0f)
		{
			animationIndex_ = 4;
			downAnimationTimer_ = 60;
			animationTime = 0.0f;
			model_->SetAnimationTime(animationTime);
			aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
			SetAABB(aabb_);
			isHitTackle_ = false;
			isDown_ = false;
		}
	}
}

void Enemy::ComboNumberSpriteUpdate()
{
	int comboNum = comboCount_;

	comboNumTextureHandle_ = TextureManager::LoadTexture("resource/number/" + std::to_string(comboNum) + ".png");

	comboNumSprite_->SetTexture(comboNumTextureHandle_);
}

Vector3 Enemy::GetWorldPosition()
{
	Vector3 pos{};
	pos.x = worldTransform_.matWorld.m[3][0];
	pos.y = worldTransform_.matWorld.m[3][1];
	pos.z = worldTransform_.matWorld.m[3][2];
	return pos;
}

int Enemy::Random(int min_value, int max_value)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(min_value, max_value);

	return dis(gen);
}
