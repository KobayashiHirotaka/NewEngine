#include "Enemy.h"
#include "Application/GameObject/Character/Player/Player.h"
#include "Application/Game/Scenes/GamePlayScene.h"

Enemy::~Enemy()
{
	delete hpBar_.sprite_;
	delete guardGaugeBar_.sprite_;
	delete finisherGaugeBar_.sprite_;

	for (auto& bullet : bullets_) 
	{
		delete bullet;
	}
	bullets_.clear();
}

void Enemy::Initialize()
{
	IGame3dObject::SetTag("Enemy");

	//Inputのinstance
	input_ = Input::GetInstance();

	//Audioのinstance
	audio_ = Audio::GetInstance();

	//WorldTransformの初期化
	worldTransform_.Initialize();

	//当たり判定の設定
	SetAABB(aabb_);

	bulletModel_.reset(Model::CreateFromOBJ("resource/bullet", "bullet.obj"));

	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(kCollisionMaskEnemy);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);

	//リソース
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

	//SEの初期化
	attackSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/Attack.mp3");
	weaponAttackSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/WeaponAttack.mp3");
	damageSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/Damage.mp3");
	guardSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/Guard.mp3");

	//パーティクル
	particleEffectPlayer_ = std::make_unique<ParticleEffectPlayer>();
	particleEffectPlayer_->Initialize();

	//WorldTransformの更新
	worldTransform_.UpdateMatrixEuler();
}

void Enemy::Update()
{
	ICharacter::Update();

	//振り向きの処理
	Vector3 playerWorldPosition = player_->GetWorldPosition();

	Vector3 enemyWorldPosition = GetWorldPosition();

	if (enemyWorldPosition.x > playerWorldPosition.x && characterState_.behavior != Behavior::kJump
		&& characterState_.behavior != Behavior::kAttack && !characterState_.isDown)
	{
		characterState_.direction = Direction::Left;
		worldTransform_.rotation.y = 4.6f;
	}

	if (enemyWorldPosition.x < playerWorldPosition.x && characterState_.behavior != Behavior::kJump
		&& characterState_.behavior != Behavior::kAttack && !characterState_.isDown)
	{
		characterState_.direction = Direction::Right;
		worldTransform_.rotation.y = 1.7f;
	}

	BulletsUpdate();

	//コンボを食らっているとき
	if (characterState_.isHitLightPunch)
	{
		comboCount_ = 1;
		timerData_.comboTimer--;
	}

	if (characterState_.isHitTCMiddlePunch)
	{
		comboCount_ = 2;
		timerData_.comboTimer = 60;
		timerData_.comboTimer--;
	}

	if (characterState_.isHitTCHighPunch)
	{
		comboCount_ = 3;
		timerData_.comboTimer = 60;
		timerData_.comboTimer--;
	}

	if (characterState_.isHitHighPunch && comboCount_ == 0)
	{
		comboCount_ = 1;
		timerData_.comboTimer = 60;
		timerData_.comboTimer--;
	}

	if (characterState_.isHitHighPunch && comboCount_ >= 2)
	{
		comboCount_ = 3;
		timerData_.comboTimer = 60;
		timerData_.comboTimer--;
	}

	if (characterState_.isHitTackle && comboCount_ == 3)
	{
		comboCount_ = 4;
		timerData_.comboTimer = 60;
		timerData_.comboTimer--;
	}

	if (characterState_.isHitTackle && comboCount_ == 1)
	{
		comboCount_ = 2;
		timerData_.comboTimer = 60;
		timerData_.comboTimer--;
	}

	if (timerData_.comboTimer < 60)
	{
		timerData_.comboTimer--;
	}

	if (timerData_.comboTimer < 0)
	{
		timerData_.comboTimer = 60;
		comboCount_ = 0;
	}

	if (!player_->GetIsAttack())
	{
		timerData_.guardAnimationTimer = 60;
	}

	ComboNumberSpriteUpdate();

	particleEffectPlayer_->Update();

	//WorldTransformの更新
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

void Enemy::SpriteDraw()
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

void Enemy::BulletDraw(const Camera& camera)
{
	for (auto& bullet : bullets_)
	{
		bullet->Draw(camera);
	}
}

void Enemy::ParticleDraw(const Camera& camera)
{
	particleEffectPlayer_->Draw(camera);

	for (auto& bullet : bullets_)
	{
		bullet->ParticleDraw(camera);
	}
}

void Enemy::ImGui(const char* title)
{
	ImGui::Begin(title);
	ImGui::DragFloat3("WTFT", &worldTransform_.translation.x, -14.0f, 14.0f);
	ImGui::DragFloat3("WTFR", &worldTransform_.rotation.x, 0.0f, 150.0f);
	ImGui::DragFloat3("WTFS", &worldTransform_.scale.x, 0.0f, 300.0f);

	ImGui::SliderFloat3("WTFT", &worldTransform_.translation.x, -100.0f, 100.0f);
	ImGui::SliderFloat3("WTFR", &worldTransform_.rotation.x, 0.0f, 16.0f);

	ImGui::Text("isGuard %d", characterState_.isGuard);
	ImGui::Text("isHit %d", characterState_.isHitCharacter);
	ImGui::Text("patternCount %d", patternCount_);

	ImGui::Checkbox("isDebug_", &isDebug_);

	model_->GetLight()->ImGui("DirectionalLight");
	model_->GetPointLight()->ImGui("PointLight");
	model_->GetSpotLight()->ImGui("SpotLight");
	ImGui::End();
}

void Enemy::BehaviorRootInitialize()
{
	animationIndex_ = 5;

	patternCount_ = Random(1, 2);
}

void Enemy::BehaviorRootUpdate()
{
	if (!isDebug_)
	{
		//移動処理(後ろ歩きスタート)
		if (patternCount_ == 1 && characterState_.isDown == false && comboCount_ == 0)
		{
			moveTimer_--;

			bool isFrontMove_ = false;
			bool isBackMove_ = false;
			moveData_.velocity = { 0.0f, 0.0f, 0.0f };

			if (moveTimer_ <= 30 && characterState_.direction == Direction::Left)
			{
				moveData_.velocity.x = 0.01f;
				isFrontMove_ = false;
				isBackMove_ = true;
				characterState_.isGuard = false;
			}

			if (moveTimer_ <= 30 && characterState_.direction == Direction::Right)
			{
				moveData_.velocity.x = 0.01f;
				isFrontMove_ = true;
				isBackMove_ = false;
				characterState_.isGuard = false;
			}

			if (moveTimer_ > 30 && characterState_.direction == Direction::Right)
			{
				moveData_.velocity.x = -0.01f;
				isFrontMove_ = false;
				isBackMove_ = true;
				characterState_.isGuard = true;
			}

			if (moveTimer_ > 30 && characterState_.direction == Direction::Left)
			{
				moveData_.velocity.x = -0.01f;
				isFrontMove_ = true;
				isBackMove_ = false;
				characterState_.isGuard = true;
			}

			//移動
			if (isFrontMove_)
			{
				animationIndex_ = 0;
				UpdateAnimationTime(animationTime_, true, 30.0f, animationIndex_, animationDuration_, model_);

				moveData_.velocity = Normalize(moveData_.velocity);
				moveData_.velocity = Multiply(frontSpeed_, moveData_.velocity);

				// 平行移動
				worldTransform_.translation = Add(worldTransform_.translation, moveData_.velocity);

				worldTransform_.UpdateMatrixEuler();
			}
			else if (isBackMove_)
			{
				animationIndex_ = 2;
				UpdateAnimationTime(animationTime_, true, 40.0f, animationIndex_, animationDuration_, model_);

				moveData_.velocity = Normalize(moveData_.velocity);
				moveData_.velocity = Multiply(backSpeed_, moveData_.velocity);

				// 平行移動
				worldTransform_.translation = Add(worldTransform_.translation, moveData_.velocity);

				worldTransform_.UpdateMatrixEuler();
			}
			else
			{
				animationIndex_ = 5;
				UpdateAnimationTime(animationTime_, true, 60.0f, animationIndex_, animationDuration_, model_);
			}

			if (moveTimer_ <= 0)
			{
				moveTimer_ = Random(30, 60);
				patternCount_ = Random(4, 4);
			}
		}

		//移動処理(前歩きスタート)
		if (patternCount_ == 2 && characterState_.isDown == false && comboCount_ == 0)
		{
			moveTimer_--;

			bool isFrontMove_ = false;
			bool isBackMove_ = false;
			moveData_.velocity = { 0.0f, 0.0f, 0.0f };

			if (moveTimer_ < 30 && characterState_.direction == Direction::Left && !characterState_.isHitCharacter)
			{
				moveData_.velocity.x = 0.01f;
				isFrontMove_ = false;
				isBackMove_ = true;
				characterState_.isGuard = false;
			}

			if (moveTimer_ < 30 && characterState_.direction == Direction::Right && !characterState_.isHitCharacter)
			{
				moveData_.velocity.x = 0.01f;
				isFrontMove_ = true;
				isBackMove_ = false;
				characterState_.isGuard = false;
			}

			if (moveTimer_ >= 30 && characterState_.direction == Direction::Right)
			{
				moveData_.velocity.x = -0.01f;
				isFrontMove_ = false;
				isBackMove_ = true;
				characterState_.isGuard = true;
			}

			if (moveTimer_ >= 30 && characterState_.direction == Direction::Left)
			{
				moveData_.velocity.x = -0.01f;
				isFrontMove_ = true;
				isBackMove_ = false;
				characterState_.isGuard = true;
			}


			//移動
			if (isFrontMove_)
			{
				animationIndex_ = 0;
				UpdateAnimationTime(animationTime_, true, 30.0f, animationIndex_, animationDuration_, model_);

				moveData_.velocity = Normalize(moveData_.velocity);
				moveData_.velocity = Multiply(frontSpeed_, moveData_.velocity);

				// 平行移動
				worldTransform_.translation = Add(worldTransform_.translation, moveData_.velocity);

				worldTransform_.UpdateMatrixEuler();
			}
			else if (isBackMove_)
			{
				animationIndex_ = 2;
				UpdateAnimationTime(animationTime_, true, 40.0f, animationIndex_, animationDuration_, model_);

				moveData_.velocity = Normalize(moveData_.velocity);
				moveData_.velocity = Multiply(backSpeed_, moveData_.velocity);

				// 平行移動
				worldTransform_.translation = Add(worldTransform_.translation, moveData_.velocity);

				worldTransform_.UpdateMatrixEuler();
			}
			else
			{
				animationIndex_ = 5;

				UpdateAnimationTime(animationTime_, true, 60.0f, animationIndex_, animationDuration_, model_);
			}

			if (moveTimer_ <= 0)
			{
				moveTimer_ = Random(30, 60);
				patternCount_ = Random(4, 4);
			}
		}

		//攻撃
		//突進攻撃
		if (patternCount_ == 3 && !characterState_.isDown)
		{
			AttackStart(attackData_.isTackle);
		}

		//弾攻撃
		if (patternCount_ == 4 && !characterState_.isDown)
		{
			AttackStart(attackData_.isShot);
		}

	}
}

void Enemy::BehaviorAttackInitialize()
{
	attackData_.attackAnimationFrame = 0;
}

void Enemy::BehaviorAttackUpdate()
{
	//タックル攻撃
	if (attackData_.isTackle)
	{
		animationIndex_ = 8;
		characterState_.isGuard = false;
		

		float particlePositionX = 0.0f;

		if (!characterState_.isDown)
		{
			UpdateAnimationTime(animationTime_, false, 40.0f, animationIndex_, animationDuration_, model_);
		}

		if (characterState_.direction == Direction::Right)
		{
			aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
			SetAABB(aabb_);

			if (attackData_.attackAnimationFrame >= 25 && attackData_.attackAnimationFrame < 40)
			{
				attackData_.isAttack = true;
				worldTransform_.translation.x += 0.15f;
			}

			if (attackData_.attackAnimationFrame >= 25 && attackData_.attackAnimationFrame < 60)
			{
				particlePositionX = 0.1f;
				particlePositionX += 0.3f;

				particleEffectPlayer_->PlayParticle("RightNackle", { worldTransform_.translation.x + particlePositionX,
					worldTransform_.translation.y + 0.6f,worldTransform_.translation.z });
			}
		}
		else if (characterState_.direction == Direction::Left)
		{
			aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
			SetAABB(aabb_);

			if (attackData_.attackAnimationFrame >= 25 && attackData_.attackAnimationFrame < 40)
			{
				attackData_.isAttack = true;
				worldTransform_.translation.x -= 0.15f;
			}

			if (attackData_.attackAnimationFrame >= 25 && attackData_.attackAnimationFrame < 60)
			{
				particlePositionX = 0.1f;
				particlePositionX += 0.3f;

				particleEffectPlayer_->PlayParticle("LeftNackle", { worldTransform_.translation.x - particlePositionX,
					worldTransform_.translation.y + 0.6f,worldTransform_.translation.z });
			}
		}

		if (attackData_.attackAnimationFrame >= 55)
		{
			attackData_.isAttack = false;
		}

		if (characterState_.isDown || attackData_.attackAnimationFrame >= 100)
		{
			patternCount_ = Random(1, 2);
			characterState_.behaviorRequest = Behavior::kRoot;
			attackData_.isAttack = false;
			attackData_.isTackle = false;
			animationTime_ = 0.0f;
			attackData_.attackAnimationFrame = 0;
			model_->SetAnimationTime(animationTime_);
			aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
			SetAABB(aabb_);
		}

		attackData_.attackAnimationFrame++;
	}

	//弾攻撃
	if (attackData_.isShot)
	{
		animationIndex_ = 1;
		characterState_.isGuard = false;

		if (!characterState_.isDown)
		{
			UpdateAnimationTime(animationTime_, false, 40.0f, animationIndex_, animationDuration_, model_);
		}

		// まだ弾を発射していない場合
		if (!hasShot_) 
		{  
			if (characterState_.direction == Direction::Right)
			{
				Vector3 bulletStartPosition = { worldTransform_.translation.x + 0.2f, worldTransform_.translation.y + 0.5f, worldTransform_.translation.z };  // 弾の発射位置を敵の位置に設定
				Vector3 bulletVelocity = Vector3{ 0.1f, 0.0f, 0.0f };  // 弾の速度を設定

				BulletShoot(bulletStartPosition, bulletVelocity);
			}
			else if (characterState_.direction == Direction::Left)
			{
				Vector3 bulletStartPosition = { worldTransform_.translation.x - 0.2f, worldTransform_.translation.y + 0.5f, worldTransform_.translation.z };  // 弾の発射位置を敵の位置に設定
				Vector3 bulletVelocity = Vector3{ -0.1f, 0.0f, 0.0f };  // 弾の速度を設定

				BulletShoot(bulletStartPosition, bulletVelocity);
			}

			hasShot_ = true;  // 弾を発射したことを記録
		}

		if (characterState_.isDown || attackData_.attackAnimationFrame >= 60)
		{
			patternCount_ = Random(1, 2);
			characterState_.behaviorRequest = Behavior::kRoot;
			attackData_.isAttack = false;
			attackData_.isShot = false;
			animationTime_ = 0.0f;
			attackData_.attackAnimationFrame = 0;
			model_->SetAnimationTime(animationTime_);
			hasShot_ = false;
		}

		attackData_.attackAnimationFrame++;
	}
}

void Enemy::BehaviorJumpInitialize()
{
	worldTransform_.translation.y = 0.0f;

	const float kJumpFirstSpeed_ = 0.3f;

	moveData_.velocity.y = kJumpFirstSpeed_;
}

void Enemy::BehaviorJumpUpdate()
{
	animationIndex_ = 4;
	UpdateAnimationTime(animationTime_, true, 60.0f, animationIndex_, animationDuration_, model_);

	worldTransform_.translation = Add(worldTransform_.translation, moveData_.velocity);

	const float kGravityAcceleration_ = 0.02f;

	Vector3 accelerationVector_ = { 0.0f,-kGravityAcceleration_,0.0f };

	moveData_.velocity = Add(moveData_.velocity, accelerationVector_);

	if (worldTransform_.translation.y <= 0.0f)
	{
		characterState_.behaviorRequest = Behavior::kRoot;
		worldTransform_.translation.y = 0.0f;
		animationTime_ = 0.0f;
		model_->SetAnimationTime(animationTime_);
	}
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

	if (characterState_.direction == Direction::Left)
	{
		aabb_ = { {-0.6f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
		SetAABB(aabb_);
	}
	else if(characterState_.direction == Direction::Right)
	{
		aabb_ = { {-0.3f,-0.3f,-0.3f},{0.6f,0.3f,0.3f} };
		SetAABB(aabb_);
	}

	if (!characterState_.isDown)
	{
		animationTime += 1.0f / 60.0f;
	}

	model_->SetAnimationTime(animationTime);
	model_->ApplyAnimation(animationIndex_);

	if (animationTime >= animationDuration || characterState_.isDown)
	{
		characterState_.behaviorRequest = Behavior::kRoot;
		animationTime = 0.0f;
		attackData_.attackAnimationFrame = 0;
		guardGauge_ = 0.0f;
		model_->SetAnimationTime(animationTime);
		aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
		SetAABB(aabb_);
	}
}

void Enemy::UpdateAnimationTime(float animationTime, bool isLoop, float frameRate, int animationIndex,
	float animationDuration, std::unique_ptr<Model>& modelFighterBody)
{
	ICharacter::UpdateAnimationTime(animationTime, isLoop, frameRate, animationIndex, animationDuration, modelFighterBody);
}

void Enemy::OnCollision(Collider* collider, float damage)
{
	//プレイヤーの近接攻撃との当たり判定
	if (collider->GetCollisionAttribute() & kCollisionAttributePlayer)
	{
		characterState_.isHitCharacter = true;

		if (player_->GetIsAttack() && !player_->GetIsTackle() && characterState_.isGuard && characterState_.direction == Direction::Right)
		{
			timerData_.guardAnimationTimer--;

			audio_->SoundPlayMP3(guardSoundHandle_, false, 1.0f);
			worldTransform_.translation.x -= 0.3f;
			guardGauge_ += 1.0f;

			if (timerData_.guardAnimationTimer > 55)
			{

				particleEffectPlayer_->PlayParticle("Guard", { worldTransform_.translation.x + 0.1f,
					worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });
			}
		}

		if (player_->GetIsAttack() && !player_->GetIsTackle() && characterState_.isGuard && characterState_.direction == Direction::Left)
		{
			timerData_.guardAnimationTimer--;

			audio_->SoundPlayMP3(guardSoundHandle_, false, 1.0f);
			worldTransform_.translation.x += 0.3f;
			guardGauge_ += 1.0f;

			if (timerData_.guardAnimationTimer > 55)
			{

				particleEffectPlayer_->PlayParticle("Guard", { worldTransform_.translation.x - 0.1f,
					worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });
			}
		}

		if (player_->GetIsAttack() && player_->GetIsTackle() && characterState_.isGuard && characterState_.direction == Direction::Right)
		{
			timerData_.guardAnimationTimer--;

			audio_->SoundPlayMP3(guardSoundHandle_, false, 1.0f);
			worldTransform_.translation.x -= 0.2f;
			guardGauge_ += 1.0f;

			if (timerData_.guardAnimationTimer > 55)
			{
				particleEffectPlayer_->PlayParticle("Guard", { worldTransform_.translation.x + 0.1f,
									worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });
			}
		}

		if (player_->GetIsAttack() && player_->GetIsTackle() && characterState_.isGuard && characterState_.direction == Direction::Left)
		{
			timerData_.guardAnimationTimer--;

			audio_->SoundPlayMP3(guardSoundHandle_, false, 1.0f);
			worldTransform_.translation.x += 0.2f;
			guardGauge_ += 1.0f;

			if (timerData_.guardAnimationTimer > 55)
			{

				particleEffectPlayer_->PlayParticle("Guard", { worldTransform_.translation.x - 0.1f,
					worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });
			}
		}

		//弱パンチ
		if (player_->GetIsLightPunch() && !characterState_.isDown && !characterState_.isGuard)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 2.0f;
			hp_ -= damage;
			characterState_.isHitLightPunch = true;

			HitStop(10);
		}

		//中パンチ
		if (player_->GetIsMiddlePunch() && !characterState_.isDown && !characterState_.isGuard)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 5.0f;
			hp_ -= damage;
			characterState_.isHitMiddlePunch = true;

			HitStop(10);
		}

		//強パンチ
		if (player_->GetIsHighPunch() && !characterState_.isDown && !characterState_.isGuard)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 10.0f;
			hp_ -= damage;
			characterState_.isHitHighPunch = true;

			HitStop(10);
		}

		//TC中パンチ
		if (player_->GetIsTCMiddlePunch() && !characterState_.isDown && !characterState_.isGuard)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 2.0f;
			hp_ -= damage;
			characterState_.isHitTCMiddlePunch = true;

			HitStop(10);
		}

		//TC強パンチ
		if (player_->GetIsTCHighPunch() && !characterState_.isDown && !characterState_.isGuard)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 2.0f;
			hp_ -= damage;
			characterState_.isHitTCHighPunch = true;

			HitStop(10);
		}

		//タックル
		//キャンセルじゃないとき
		if (player_->GetIsTackle() && player_->GetIsAttack() && !characterState_.isDown && !characterState_.isGuard)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 15.0f;
			hp_ -= damage;
			characterState_.isHitTackle = true;

			HitStop(30);
		}

		//キャンセルのとき
		if (player_->GetIsTackle() && player_->GetIsAttack() && characterState_.isDown && !characterState_.isGuard && worldTransform_.translation.y > 0.5f)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 4.0f;
			hp_ -= damage;
			timerData_.downAnimationTimer = 60;
			float animationTime = 0.0f;
			model_->SetAnimationTime(animationTime);
			characterState_.isHitHighPunch = false;
			characterState_.isHitTackle = true;

			HitStop(10);
		}
	}
}

void Enemy::AttackStart(bool& isAttackType)
{
	ICharacter::AttackStart(isAttackType);
}

void Enemy::AttackEnd()
{

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
		characterState_.isGuard = false;
		attackData_.isAttack = false;
		characterState_.behaviorRequest = Behavior::kStan;
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
	ICharacter::Reset();

	hp_ = 100.0f;

	animationIndex_ = 4;

	worldTransform_.translation = { 3.0f,0.0f,0.0f };
	characterState_.direction = Direction::Left;

	worldTransform_.UpdateMatrixEuler();
}

void Enemy::HitStop(int milliseconds)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void Enemy::DownAnimation()
{
	//弱攻撃
	if (characterState_.isHitLightPunch && characterState_.direction == Direction::Right)
	{
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;

		if (timerData_.downAnimationTimer > 55)
		{
			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x - 0.1f,
						worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });
		}

		animationIndex_ = 4;
		UpdateAnimationTime(animationTime_, false, 30.0f, animationIndex_, animationDuration_, model_);

		if (!player_->GetIsLightPunch() && hp_ > 0.0f)
		{
			animationIndex_ = 5;
			timerData_.downAnimationTimer = 60;
			animationTime_ = 0.0f;
			model_->SetAnimationTime(animationTime_);
			characterState_.isHitLightPunch = false;
			characterState_.isDown = false;
		}
	}

	if (characterState_.isHitLightPunch && characterState_.direction == Direction::Left)
	{
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;

		if (timerData_.downAnimationTimer > 55)
		{
			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x + 0.1f,
						worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });
		}

		animationIndex_ = 4;
		UpdateAnimationTime(animationTime_, false, 30.0f, animationIndex_, animationDuration_, model_);

		if (!player_->GetIsLightPunch() && hp_ > 0.0f)
		{
			animationIndex_ = 5;
			timerData_.downAnimationTimer = 60;
			animationTime_ = 0.0f;
			model_->SetAnimationTime(animationTime_);
			characterState_.isHitLightPunch = false;
			characterState_.isDown = false;
		}
	}

	//中攻撃
	if (characterState_.isHitMiddlePunch && characterState_.direction == Direction::Left)
	{
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;

		if (timerData_.downAnimationTimer > 55)
		{
			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x - 0.1f,
						worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });
		}

		animationIndex_ = 4;
		UpdateAnimationTime(animationTime_, false, 30.0f, animationIndex_, animationDuration_, model_);

		if (!player_->GetIsMiddlePunch() && hp_ > 0.0f)
		{
			animationIndex_ = 5;
			timerData_.downAnimationTimer = 60;
			animationTime_ = 0.0f;
			model_->SetAnimationTime(animationTime_);
			characterState_.isHitMiddlePunch = false;
			characterState_.isDown = false;
		}
	}

	if (characterState_.isHitMiddlePunch && characterState_.direction == Direction::Right)
	{
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;

		if (timerData_.downAnimationTimer > 55)
		{
			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x + 0.1f,
							worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });
		}

		animationIndex_ = 4;
		UpdateAnimationTime(animationTime_, false, 30.0f, animationIndex_, animationDuration_, model_);

		if (!player_->GetIsMiddlePunch() && hp_ > 0.0f)
		{
			animationIndex_ = 5;
			timerData_.downAnimationTimer = 60;
			animationTime_ = 0.0f;
			model_->SetAnimationTime(animationTime_);
			characterState_.isHitMiddlePunch = false;
			characterState_.isDown = false;
		}
	}

	//強攻撃
	if (characterState_.isHitHighPunch && characterState_.direction == Direction::Left)
	{
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;

		if (timerData_.downAnimationTimer > 55)
		{
			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x - 0.1f,
						worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });

			const float kJumpFirstSpeed_ = 0.15f;
			moveData_.velocity.x = 0.025f;
			moveData_.velocity.y = kJumpFirstSpeed_;
		}
		else if (timerData_.downAnimationTimer <= 55 && timerData_.downAnimationTimer > -30)
		{
			worldTransform_.translation = Add(worldTransform_.translation, moveData_.velocity);

			const float kGravityAcceleration_ = 0.005f;

			Vector3 accelerationVector_ = { 0.0f,-kGravityAcceleration_,0.0f };

			moveData_.velocity = Add(moveData_.velocity, accelerationVector_);

			if (worldTransform_.translation.y <= 0.0f)
			{
				moveData_.velocity.x = 0.0f;
				worldTransform_.translation.y = 0.0f;
			}
		}

		animationIndex_ = 6;
		UpdateAnimationTime(animationTime_, false, 30.0f, animationIndex_, animationDuration_, model_);

		if (timerData_.downAnimationTimer <= -30 && worldTransform_.translation.y <= 0.0f && hp_ > 0.0f)
		{
			animationIndex_ = 5;
			timerData_.downAnimationTimer = 60;
			animationTime_ = 0.0f;
			model_->SetAnimationTime(animationTime_);
			characterState_.isHitHighPunch = false;
			characterState_.isDown = false;
		}
	}

	if (characterState_.isHitHighPunch && characterState_.direction == Direction::Right)
	{
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;

		if (timerData_.downAnimationTimer > 55)
		{
			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x + 0.1f,
						worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });

			const float kJumpFirstSpeed_ = 0.15f;
			moveData_.velocity.x = -0.025f;
			moveData_.velocity.y = kJumpFirstSpeed_;
		}
		else if (timerData_.downAnimationTimer <= 55 && timerData_.downAnimationTimer > -30)
		{
			worldTransform_.translation = Add(worldTransform_.translation, moveData_.velocity);

			const float kGravityAcceleration_ = 0.005f;

			Vector3 accelerationVector_ = { 0.0f,-kGravityAcceleration_,0.0f };

			moveData_.velocity = Add(moveData_.velocity, accelerationVector_);

			if (worldTransform_.translation.y <= 0.0f)
			{
				moveData_.velocity.x = 0.0f;
				worldTransform_.translation.y = 0.0f;
			}
		}

		animationIndex_ = 6;
		UpdateAnimationTime(animationTime_, false, 30.0f, animationIndex_, animationDuration_, model_);

		if (timerData_.downAnimationTimer <= -30 && worldTransform_.translation.y <= 0.0f && hp_ > 0.0f)
		{
			animationIndex_ = 5;
			timerData_.downAnimationTimer = 60;
			animationTime_ = 0.0f;
			model_->SetAnimationTime(animationTime_);
			characterState_.isHitHighPunch = false;
			characterState_.isDown = false;
		}
	}

	//TC中攻撃
	if (characterState_.isHitTCMiddlePunch && characterState_.direction == Direction::Left)
	{
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;

		if (timerData_.downAnimationTimer > 55)
		{
			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x - 0.1f,
						worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });
		}

		animationIndex_ = 4;
		UpdateAnimationTime(animationTime_, false, 30.0f, animationIndex_, animationDuration_, model_);

		if (!player_->GetIsTCMiddlePunch() && hp_ > 0.0f)
		{
			animationIndex_ = 5;
			timerData_.downAnimationTimer = 60;
			animationTime_ = 0.0f;
			model_->SetAnimationTime(animationTime_);
			characterState_.isHitTCMiddlePunch = false;
			characterState_.isDown = false;
		}
	}

	if (characterState_.isHitTCMiddlePunch && characterState_.direction == Direction::Right)
	{
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;

		if (timerData_.downAnimationTimer > 55)
		{
			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x + 0.1f,
						worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });
		}

		animationIndex_ = 4;
		UpdateAnimationTime(animationTime_, false, 30.0f, animationIndex_, animationDuration_, model_);

		if (!player_->GetIsTCMiddlePunch() && hp_ > 0.0f)
		{
			animationIndex_ = 5;
			timerData_.downAnimationTimer = 60;
			animationTime_ = 0.0f;
			model_->SetAnimationTime(animationTime_);
			characterState_.isHitTCMiddlePunch = false;
			characterState_.isDown = false;
		}
	}

	//TC強攻撃
	if (characterState_.isHitTCHighPunch && characterState_.direction == Direction::Left)
	{
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;

		if (timerData_.downAnimationTimer > 55)
		{
			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x - 0.1f,
						worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });
		}

		if (timerData_.downAnimationTimer > 35 && worldTransform_.translation.x < 4.0f)
		{
			worldTransform_.translation.x += 0.02f;

		}

		animationIndex_ = 6;
		UpdateAnimationTime(animationTime_, false, 30.0f, animationIndex_, animationDuration_, model_);

		aabb_ = { {0.1f,-0.3f,-0.3f},{0.8f,0.0f,0.3f} };
		SetAABB(aabb_);

		if (!player_->GetIsTCHighPunch() && hp_ > 0.0f)
		{
			animationIndex_ = 5;
			timerData_.downAnimationTimer = 60;
			animationTime_ = 0.0f;
			model_->SetAnimationTime(animationTime_);
			aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
			SetAABB(aabb_);
			characterState_.isHitTCHighPunch = false;
			characterState_.isDown = false;
		}
	}

	if (characterState_.isHitTCHighPunch && characterState_.direction == Direction::Right)
	{
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;

		if (timerData_.downAnimationTimer > 55)
		{
			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x + 0.1f,
						worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });
		}

		if (timerData_.downAnimationTimer > 35 && worldTransform_.translation.x > -4.0f)
		{
			worldTransform_.translation.x -= 0.02f;

		}

		animationIndex_ = 6;
		UpdateAnimationTime(animationTime_, false, 30.0f, animationIndex_, animationDuration_, model_);

		aabb_ = { {-0.8f,-0.3f,-0.3f},{-0.1f,0.0f,0.3f} };
		SetAABB(aabb_);

		if (!player_->GetIsTCHighPunch() && hp_ > 0.0f)
		{
			animationIndex_ = 5;
			timerData_.downAnimationTimer = 60;
			animationTime_ = 0.0f;
			model_->SetAnimationTime(animationTime_);
			aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
			SetAABB(aabb_);
			characterState_.isHitTCHighPunch = false;
			characterState_.isDown = false;
		}
	}

	//タックル攻撃
	if (characterState_.isHitTackle && characterState_.direction == Direction::Left)
	{
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;

		if (timerData_.downAnimationTimer > 55)
		{
			effectState_.isShake = true;

			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x - 0.1f,
						worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });
		}
		else
		{
			effectState_.isShake = false;
		}

		if (timerData_.downAnimationTimer > 35 && worldTransform_.translation.x < 4.0f)
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

		animationIndex_ = 6;
		UpdateAnimationTime(animationTime_, false, 30.0f, animationIndex_, animationDuration_, model_);

		aabb_ = { {0.1f,-0.3f,-0.3f},{0.8f,0.0f,0.3f} };
		SetAABB(aabb_);

		if (!player_->GetIsTackle() && hp_ > 0.0f)
		{
			animationIndex_ = 5;
			timerData_.downAnimationTimer = 60;
			animationTime_ = 0.0f;
			model_->SetAnimationTime(animationTime_);
			aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
			SetAABB(aabb_);
			characterState_.isHitTackle = false;
			characterState_.isDown = false;
		}
	}

	if (characterState_.isHitTackle && characterState_.direction == Direction::Right)
	{
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;

		if (timerData_.downAnimationTimer > 55)
		{
			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x + 0.1f,
						worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });

			effectState_.isShake = true;
		}
		else
		{
			effectState_.isShake = false;
		}

		if (timerData_.downAnimationTimer > 35 && worldTransform_.translation.x > -4.0f)
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

		animationIndex_ = 6;
		UpdateAnimationTime(animationTime_, false, 30.0f, animationIndex_, animationDuration_, model_);

		aabb_ = { {-0.8f,-0.3f,-0.3f},{-0.1f,0.0f,0.3f} };
		SetAABB(aabb_);

		if (!player_->GetIsTackle() && hp_ > 0.0f)
		{
			animationIndex_ = 5;
			timerData_.downAnimationTimer = 60;
			animationTime_ = 0.0f;
			model_->SetAnimationTime(animationTime_);
			aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
			SetAABB(aabb_);
			characterState_.isHitTackle = false;
			characterState_.isDown = false;
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

void Enemy::BulletShoot(const Vector3& startPosition, const Vector3& velocity)
{
	// 弾を生成してリストに追加する
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(bulletModel_.get(), startPosition, velocity);
	bullets_.push_back(newBullet);
}

void Enemy::BulletsUpdate()
{
	// 弾の更新と衝突判定などを行う
	for (auto it = bullets_.begin(); it != bullets_.end();)
	{
		(*it)->Update();
		if ((*it)->GetIsDead())
		{
			delete* it;
			it = bullets_.erase(it);
		}
		else
		{
			++it;
		}
	}
}
