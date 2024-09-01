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

	ICharacter::Initialize();

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
		{979.0f, finisherGaugeBarSpace_},
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
	damageSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/HitPunch1.mp3");
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

	//デバッグ用の処理
	if (isDebug_)
	{
		if (input_->PressKey(DIK_M))
		{
			hp_ -= 1.0f;
		}

		if (input_->PressKey(DIK_N))
		{
			hp_ -= 1.0f;
		}
	}

	//エディタで設定したパラメータをセット
	AttackEditor::GetInstance()->SetAttackParameters(attackType, attackData_.attackStartTime, attackData_.attackEndTime, attackData_.recoveryTime, true);

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

	if (player_->GetFinisherTimer() == 120)
	{
		BulletsUpdate();

		particleEffectPlayer_->Update();
	}

	HitCombo();

	if (!player_->GetIsAttack())
	{
		timerData_.guardAnimationTimer = 60;
	}

	ComboNumberSpriteUpdate();

	model_->GetLight()->SetEnableLighting(false);

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
	ImGui::Text("comboTimer %d", timerData_.comboTimer);

	//ImGui::Checkbox("isDebug_", &isDebug_);

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
	if (!isDebug_ && player_->GetFinisherTimer() == 120)
	{
		if (!characterState_.isDown && comboCount_ == 0)
		{
			//移動
			Move();
		}

		//攻撃
		//突進攻撃
		if (patternCount_ == 3 && !characterState_.isDown)
		{
			attackType = "Tackle";
			AttackStart(attackData_.isTackle);
		}

		//弾攻撃
		if (patternCount_ == 4 && !characterState_.isDown)
		{
			AttackStart(attackData_.isShot);
		}

		//弱攻撃
		if (patternCount_ == 5 && !characterState_.isDown)
		{
			attackType = "LightPunch";
			AttackStart(attackData_.isLightPunch);
		}

	}
}

void Enemy::BehaviorAttackInitialize()
{
	attackData_.attackAnimationFrame = 0;
}

void Enemy::BehaviorAttackUpdate()
{
	if (player_->GetFinisherTimer() == 120)
	{
		//弱攻撃
		if (attackData_.isLightPunch)
		{
			animationIndex_ = 12;
			characterState_.isGuard = false;

			if (!characterState_.isDown)
			{
				UpdateAnimationTime(animationTime_, false, 40.0f, animationIndex_, model_);
			}

			if (characterState_.direction == Direction::Right)
			{
				aabb_ = { {-0.3f,-0.3f,-0.3f},{0.5f,0.3f,0.3f} };
				SetAABB(aabb_);
			}
			else if (characterState_.direction == Direction::Left)
			{
				aabb_ = { {-0.5f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
				SetAABB(aabb_);
			}

			EvaluateAttackTiming();

			if (characterState_.isDown || attackData_.attackAnimationFrame > attackData_.recoveryTime)
			{
				AttackEnd(attackData_.isLightPunch);
				ResetCollision();
			}

			//キャンセルの処理(中TC)
			if (!characterState_.isDown && characterState_.isHitCharacter && player_->GetIsDown() &&
				attackData_.attackAnimationFrame > 15 && attackData_.attackAnimationFrame < 30)
			{
				attackType = "TCMiddlePunch";
				attackData_.isAttack = false;
				attackData_.isLightPunch = false;
				attackData_.isTCMiddlePunch = true;
				animationTime_ = 0.0f;
				attackData_.attackAnimationFrame = 0;
				model_->SetAnimationTime(animationTime_);
				ResetCollision();
			}

			attackData_.attackAnimationFrame++;
		}

		//TC用の攻撃(2発目)
		if (attackData_.isTCMiddlePunch)
		{
			animationIndex_ = 11;
			characterState_.isGuard = false;

			if (!characterState_.isDown)
			{
				UpdateAnimationTime(animationTime_, false, 40.0f, animationIndex_, model_);
			}

			attackData_.isAttack = true;

			if (characterState_.direction == Direction::Right)
			{
				aabb_ = { {-0.3f,-0.3f,-0.3f},{0.5f,0.3f,0.3f} };
				SetAABB(aabb_);
			}
			else if (characterState_.direction == Direction::Left)
			{
				aabb_ = { {-0.5f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
				SetAABB(aabb_);
			}

			EvaluateAttackTiming();

			if (characterState_.isDown || attackData_.attackAnimationFrame > attackData_.recoveryTime)
			{
				AttackEnd(attackData_.isTCMiddlePunch);
				ResetCollision();
			}

			//キャンセルの処理(強攻撃)
			if (!characterState_.isDown && characterState_.isHitCharacter && player_->GetIsDown() &&
				attackData_.attackAnimationFrame > 15 && attackData_.attackAnimationFrame < 30)
			{
				attackType = "HighPunch";
				attackData_.isAttack = false;
				attackData_.isTCMiddlePunch = false;
				attackData_.isHighPunch = true;
				animationTime_ = 0.0f;
				attackData_.attackAnimationFrame = 0;
				model_->SetAnimationTime(animationTime_);
				ResetCollision();
			}

			attackData_.attackAnimationFrame++;
		}

		//強攻撃
		if (attackData_.isHighPunch)
		{
			animationIndex_ = 3;
			characterState_.isGuard = false;

			if (!characterState_.isDown)
			{
				UpdateAnimationTime(animationTime_, false, 40.0f, animationIndex_, model_);
			}

			if (characterState_.direction == Direction::Right)
			{
				aabb_ = { {-0.3f,-0.3f,-0.3f},{0.6f,0.3f,0.3f} };
				SetAABB(aabb_);

				if (characterState_.isHitCharacter && attackData_.attackAnimationFrame <= 15)
				{
					worldTransform_.translation.x -= 0.05f;
				}
			}
			else if (characterState_.direction == Direction::Left)
			{
				aabb_ = { {-0.6f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
				SetAABB(aabb_);

				if (characterState_.isHitCharacter && attackData_.attackAnimationFrame <= 15)
				{
					worldTransform_.translation.x += 0.05f;
				}
			}

			EvaluateAttackTiming();

			if (characterState_.isDown || attackData_.attackAnimationFrame > attackData_.recoveryTime)
			{
				AttackEnd(attackData_.isHighPunch);
				ResetCollision();
			}

			//キャンセルの処理(タックル攻撃)
			if (!characterState_.isDown && player_->GetIsDown() && attackData_.attackAnimationFrame > 15 && attackData_.attackAnimationFrame < 30)
			{
				attackType = "Tackle";
				attackData_.isAttack = false;
				attackData_.isHighPunch = false;
				attackData_.isTackle = true;
				animationTime_ = 0.0f;
				attackData_.attackAnimationFrame = 0;
				model_->SetAnimationTime(animationTime_);
				ResetCollision();
			}

			attackData_.attackAnimationFrame++;
		}

		//タックル攻撃
		if (attackData_.isTackle)
		{
			animationIndex_ = 8;
			characterState_.isGuard = false;
			float particlePositionX = 0.0f;
			int particleTime = 60;
			int moveTime = 40;

			if (!characterState_.isDown)
			{
				UpdateAnimationTime(animationTime_, false, 40.0f, animationIndex_, model_);
			}

			if (characterState_.direction == Direction::Right)
			{
				aabb_ = { {-0.3f,-0.3f,-0.3f},{0.6f,0.3f,0.3f} };
				SetAABB(aabb_);

				EvaluateAttackTiming();

				if (attackData_.attackAnimationFrame >= attackData_.attackStartTime && attackData_.attackAnimationFrame < moveTime)
				{
					worldTransform_.translation.x += 0.15f;
				}

				if (attackData_.attackAnimationFrame >= attackData_.attackStartTime && attackData_.attackAnimationFrame < particleTime)
				{
					particlePositionX = 0.1f;
					particlePositionX += 0.3f;

					particleEffectPlayer_->PlayParticle("EnemyRightNackle", { worldTransform_.translation.x + particlePositionX,
						worldTransform_.translation.y + 0.6f,worldTransform_.translation.z });
				}
			}
			else if (characterState_.direction == Direction::Left)
			{
				aabb_ = { {-0.6f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
				SetAABB(aabb_);

				EvaluateAttackTiming();

				if (attackData_.attackAnimationFrame >= attackData_.attackStartTime && attackData_.attackAnimationFrame < moveTime)
				{
					worldTransform_.translation.x -= 0.15f;
				}


				if (attackData_.attackAnimationFrame >= attackData_.attackStartTime && attackData_.attackAnimationFrame < particleTime)
				{
					particlePositionX = 0.1f;
					particlePositionX += 0.3f;

					particleEffectPlayer_->PlayParticle("EnemyLeftNackle", { worldTransform_.translation.x - particlePositionX,
						worldTransform_.translation.y + 0.6f,worldTransform_.translation.z });
				}
			}

			if (attackData_.attackAnimationFrame >= attackData_.attackEndTime)
			{
				attackData_.isAttack = false;
				ResetCollision();
			}

			if (characterState_.isDown || attackData_.attackAnimationFrame >= attackData_.recoveryTime)
			{
				patternCount_ = Random(1, 2);
				AttackEnd(attackData_.isTackle);
				ResetCollision();
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
				UpdateAnimationTime(animationTime_, false, 40.0f, animationIndex_, model_);
			}

			//まだ弾を発射していない場合
			if (!hasShot_)
			{
				if (characterState_.direction == Direction::Right)
				{
					Vector3 bulletStartPosition = { worldTransform_.translation.x + 0.2f, worldTransform_.translation.y + 0.5f, worldTransform_.translation.z };  // 弾の発射位置を敵の位置に設定
					Vector3 bulletVelocity = Vector3{ 0.1f, 0.0f, 0.0f }; 

					BulletShoot(bulletStartPosition, bulletVelocity);
				}
				else if (characterState_.direction == Direction::Left)
				{
					Vector3 bulletStartPosition = { worldTransform_.translation.x - 0.2f, worldTransform_.translation.y + 0.5f, worldTransform_.translation.z };  // 弾の発射位置を敵の位置に設定
					Vector3 bulletVelocity = Vector3{ -0.1f, 0.0f, 0.0f };

					BulletShoot(bulletStartPosition, bulletVelocity);
				}

				hasShot_ = true;  // 弾を発射したことを記録
			}

			if (characterState_.isDown || attackData_.attackAnimationFrame >= 40)
			{
				patternCount_ = Random(1, 2);
				AttackEnd(attackData_.isShot);
				hasShot_ = false;
				ResetCollision();
			}

			attackData_.attackAnimationFrame++;
		}
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
	UpdateAnimationTime(animationTime_, true, 60.0f, animationIndex_, model_);

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
	animationIndex_ = 9;
}

void Enemy::BehaviorStanUpdate()
{
	animationIndex_ = 9;
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

void Enemy::UpdateAnimationTime(float animationTime, bool isLoop, float frameRate, 
	int animationIndex, std::unique_ptr<Model>& modelFighterBody)
{
	ICharacter::UpdateAnimationTime(animationTime, isLoop, frameRate, animationIndex, modelFighterBody);
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
			guardGauge_ += 0.3f;

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
		if (player_->GetIsAttack() && player_->GetIsLightPunch() && !characterState_.isGuard )
		{
			if (!characterState_.isDown)
			{
				audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
				damage = 2.0f;
				hp_ -= damage;
				characterState_.isHitLightPunch = true;

				AdjustFinisherGauge(1.0f);

				HitStop(10);
			}
			else if(firstAttack_ == "JumpAttack")
			{
				audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
				damage = 0.2f;
				hp_ -= damage;
				characterState_.isHitLightPunch = true;

				AdjustFinisherGauge(1.0f);

				HitStop(10);
			}
		}

		//中パンチ
		if (player_->GetIsAttack() && player_->GetIsMiddlePunch() && !characterState_.isDown && !characterState_.isGuard)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 5.0f;
			hp_ -= damage;
			characterState_.isHitMiddlePunch = true;

			AdjustFinisherGauge(2.0f);

			HitStop(10);
		}

		//強パンチ
		if (player_->GetIsHighPunch() && !characterState_.isDown && !characterState_.isGuard)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 7.0f;
			hp_ -= damage;
			characterState_.isHitHighPunch = true;

			AdjustFinisherGauge(2.0f);

			HitStop(10);
		}

		//TC中パンチ
		if (player_->GetIsTCMiddlePunch() && !characterState_.isDown && !characterState_.isGuard)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 2.0f;
			hp_ -= damage;
			characterState_.isHitTCMiddlePunch = true;

			AdjustFinisherGauge(2.0f);

			HitStop(10);
		}

		//TC強パンチ
		if (player_->GetIsTCHighPunch() && !characterState_.isDown && !characterState_.isGuard)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 2.0f;
			hp_ -= damage;
			characterState_.isHitTCHighPunch = true;

			AdjustFinisherGauge(2.0f);

			HitStop(10);
		}

		//ジャンプ攻撃
		if (player_->GetIsAttack() && player_->GetIsJumpAttack() && !characterState_.isDown && !characterState_.isGuard)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 3.0f;
			hp_ -= damage;
			characterState_.isHitJumpAttack = true;

			if (characterState_.direction == Direction::Right)
			{
				worldTransform_.translation.x -= 0.1f;
			}
			else if(characterState_.direction == Direction::Left)
			{
				worldTransform_.translation.x -= 0.1f;
			}

			AdjustFinisherGauge(2.0f);

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

			AdjustFinisherGauge(4.0f);

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

			AdjustFinisherGauge(3.0f);

			HitStop(10);
		}

		//アッパー攻撃
		if (player_->GetIsUppercut() && player_->GetIsAttack() && !characterState_.isGuard &&!characterState_.isDown)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 10.0f;
			hp_ -= damage;
			characterState_.isHitUppercut = true;

			AdjustFinisherGauge(4.0f);

			HitStop(10);
		}

		//超必
		if (player_->GetIsFinisherFirstAttack() && player_->GetIsAttack() && !characterState_.isGuard && !characterState_.isDown)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 10.0f;
			hp_ -= damage;
			characterState_.isHitFinisherFirstAttack = true;

			HitStop(10);
		}

		if (player_->GetIsFinisherSecondAttack() && player_->GetIsAttack() && !characterState_.isGuard && !characterState_.isDown)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 5.0f;
			hp_ -= damage;
			characterState_.isHitFinisherSecondAttack = true;

			HitStop(10);
		}
	}
}

void Enemy::Move()
{
	//移動処理(後ろ歩きスタート)
	if (patternCount_ == 1 && characterState_.isDown == false)
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
			UpdateAnimationTime(animationTime_, true, 30.0f, animationIndex_, model_);

			moveData_.velocity = Normalize(moveData_.velocity);
			moveData_.velocity = Multiply(frontSpeed_, moveData_.velocity);

			worldTransform_.translation = Add(worldTransform_.translation, moveData_.velocity);

			worldTransform_.UpdateMatrixEuler();
		}
		else if (isBackMove_)
		{
			animationIndex_ = 2;
			UpdateAnimationTime(animationTime_, true, 40.0f, animationIndex_, model_);

			moveData_.velocity = Normalize(moveData_.velocity);
			moveData_.velocity = Multiply(backSpeed_, moveData_.velocity);

			worldTransform_.translation = Add(worldTransform_.translation, moveData_.velocity);

			worldTransform_.UpdateMatrixEuler();
		}
		else
		{
			animationIndex_ = 5;
			UpdateAnimationTime(animationTime_, true, 60.0f, animationIndex_, model_);
		}

		if (moveTimer_ <= 0)
		{
			moveTimer_ = Random(30, 60);
			patternCount_ = Random(3, 4);
		}

		if (characterState_.isHitCharacter)
		{
			moveTimer_ = Random(30, 60);
			patternCount_ = 5;
		}
	}

	//移動処理(前歩きスタート)
	if (patternCount_ == 2 && characterState_.isDown == false)
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
			UpdateAnimationTime(animationTime_, true, 30.0f, animationIndex_, model_);

			moveData_.velocity = Normalize(moveData_.velocity);
			moveData_.velocity = Multiply(frontSpeed_, moveData_.velocity);

			// 平行移動
			worldTransform_.translation = Add(worldTransform_.translation, moveData_.velocity);

			worldTransform_.UpdateMatrixEuler();
		}
		else if (isBackMove_)
		{
			animationIndex_ = 2;
			UpdateAnimationTime(animationTime_, true, 40.0f, animationIndex_, model_);

			moveData_.velocity = Normalize(moveData_.velocity);
			moveData_.velocity = Multiply(backSpeed_, moveData_.velocity);

			// 平行移動
			worldTransform_.translation = Add(worldTransform_.translation, moveData_.velocity);

			worldTransform_.UpdateMatrixEuler();
		}
		else
		{
			animationIndex_ = 6;

			UpdateAnimationTime(animationTime_, true, 60.0f, animationIndex_, model_);
		}

		if (moveTimer_ <= 0)
		{
			moveTimer_ = Random(30, 60);
			patternCount_ = Random(3, 4);
		}

		if (characterState_.isHitCharacter)
		{
			moveTimer_ = Random(30, 60);
			patternCount_ = 5;
		}
	}
}

void Enemy::AttackStart(bool& isAttackType)
{
	ICharacter::AttackStart(isAttackType);
}

void Enemy::AttackEnd(bool& isAttackType)
{
	ICharacter::AttackEnd(isAttackType);
}

void Enemy::EvaluateAttackTiming()
{
	ICharacter::EvaluateAttackTiming();
}

void Enemy::ResetCollision()
{
	aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
	SetAABB(aabb_);
}

void Enemy::ConfigureCollision(Vector3 min, Vector3 max)
{
	aabb_ = { {min.x, min.y, min.z},{max.x, max.y, max.z} };
	SetAABB(aabb_);
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

void Enemy::AdjustFinisherGauge(float value)
{
	if (finisherGauge_ < 50.0f)
	{
		finisherGauge_ += value;

		if (finisherGauge_ > 50.0f)
		{
			finisherGauge_ = 50.0f;
		}
	}

	float finisherGaugePlayer = player_->GetFinisherGauge();

	if (finisherGaugePlayer > -50.0f)
	{
		finisherGaugePlayer -= value;

		if (finisherGaugePlayer < -50.0f)
		{
			finisherGaugePlayer = -50.0f;
		}
	}

	player_->SetFinisherGauge(finisherGaugePlayer);
}

void Enemy::Reset()
{
	ICharacter::Reset();

	hp_ = 100.0f;

	animationIndex_ = 5;
	animationTime_ = 0.0f;
	model_->SetAnimationTime(animationTime_);
	model_->ApplyAnimation(animationIndex_);
	model_->Update();
	UpdateAnimationTime(animationTime_, false, 40.0f, animationIndex_, model_);

	worldTransform_.translation = { 3.0f,0.0f,0.0f };
	worldTransform_.rotation = { 0.0f,4.6f,0.0f };
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
	if (characterState_.isHitLightPunch)
	{
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;

		if (timerData_.downAnimationTimer > 55)
		{
			float particlePosX = (characterState_.direction == Direction::Right) ? -0.1f : 0.1f;

			particleEffectPlayer_->PlayParticle("Hit",{ worldTransform_.translation.x + particlePosX,
				 worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });
		}

		animationIndex_ = 4;
		UpdateAnimationTime(animationTime_, false, 30.0f, animationIndex_, model_);

		//次の入力を受け取ったらこの処理にする
		if (!player_->GetIsLightPunch() && hp_ > 0.0f)
		{
			patternCount_ = Random(1, 2);
			DownAnimationEnd(5, characterState_.isHitLightPunch);
		}
	}

	//中攻撃
	if (characterState_.isHitMiddlePunch)
	{
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;

		if (timerData_.downAnimationTimer > 55)
		{
			float particlePosX = (characterState_.direction == Direction::Right) ? -0.1f : 0.1f;

			particleEffectPlayer_->PlayParticle("Hit",{ worldTransform_.translation.x + particlePosX,
				 worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });
		}

		animationIndex_ = 4;
		UpdateAnimationTime(animationTime_, false, 30.0f, animationIndex_, model_);

		if (!player_->GetIsMiddlePunch() && hp_ > 0.0f)
		{
			DownAnimationEnd(5, characterState_.isHitMiddlePunch);
		}
	}

	//強攻撃
	if (characterState_.isHitHighPunch)
	{
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;

		if (timerData_.downAnimationTimer > 55)
		{
			float particlePosX = (characterState_.direction == Direction::Right) ? 0.1f : -0.1f;

			particleEffectPlayer_->PlayParticle("Hit",{ worldTransform_.translation.x + particlePosX,
				 worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });

			const float kJumpFirstSpeed_ = 0.15f;

			moveData_.velocity.x = (characterState_.direction == Direction::Right) ? -0.025f : 0.025f;
			moveData_.velocity.y = kJumpFirstSpeed_;
		}
		else if (timerData_.downAnimationTimer <= 55 && timerData_.downAnimationTimer > -30)
		{
			worldTransform_.translation = Add(worldTransform_.translation, moveData_.velocity);

			const float kGravityAcceleration_ = 0.005f;
			Vector3 accelerationVector_ = { 0.0f, -kGravityAcceleration_, 0.0f };

			moveData_.velocity = Add(moveData_.velocity, accelerationVector_);

			if (worldTransform_.translation.y <= 0.0f)
			{
				moveData_.velocity.x = 0.0f;
				worldTransform_.translation.y = 0.0f;
			}
		}

		animationIndex_ = 6;
		UpdateAnimationTime(animationTime_, false, 30.0f, animationIndex_, model_);

		if (!player_->GetIsHighPunch() && worldTransform_.translation.y <= 0.0f && hp_ > 0.0f)
		{
			DownAnimationEnd(5, characterState_.isHitHighPunch);
		}
	}

	//TC中攻撃
	if (characterState_.isHitTCMiddlePunch)
	{
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;

		if (timerData_.downAnimationTimer > 55)
		{
			float particlePosX = (characterState_.direction == Direction::Right) ? 0.1f : -0.1f;

			particleEffectPlayer_->PlayParticle("Hit",{ worldTransform_.translation.x + particlePosX,
				 worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });
		}

		animationIndex_ = 4;
		UpdateAnimationTime(animationTime_, false, 30.0f, animationIndex_, model_);

		if (!player_->GetIsTCMiddlePunch() && hp_ > 0.0f)
		{
			DownAnimationEnd(5, characterState_.isHitTCMiddlePunch);
		}
	}

	//TC強攻撃
	if (characterState_.isHitTCHighPunch)
	{
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;

		float particlePosX = (characterState_.direction == Direction::Right) ? 0.1f : -0.1f;
		float moveX = (characterState_.direction == Direction::Right) ? -0.02f : 0.02f;

		aabb_ = (characterState_.direction == Direction::Right) ? AABB{ {-0.8f, -0.3f, -0.3f}, {-0.1f, 0.0f, 0.3f} } :
			AABB{ {0.1f, -0.3f, -0.3f}, {0.8f, 0.0f, 0.3f} };

		if (timerData_.downAnimationTimer > 55)
		{
			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x + particlePosX,
						worldTransform_.translation.y + 0.5f, worldTransform_.translation.z });
		}

		if (timerData_.downAnimationTimer > 35 && ((characterState_.direction == Direction::Left && worldTransform_.translation.x < rightEdge_) ||
			(characterState_.direction == Direction::Right && worldTransform_.translation.x > leftEdge_)))
		{
			worldTransform_.translation.x += moveX;
		}

		animationIndex_ = 6;
		UpdateAnimationTime(animationTime_, false, 30.0f, animationIndex_, model_);

		SetAABB(aabb_);

		if (!player_->GetIsTCHighPunch() && hp_ > 0.0f)
		{
			DownAnimationEnd(5, characterState_.isHitTCHighPunch);
			ResetCollision();
		}
	}

	//ジャンプ攻撃
	if (characterState_.isHitJumpAttack)
	{
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;

		if (timerData_.downAnimationTimer > 55)
		{
			float particlePosX = (characterState_.direction == Direction::Right) ? -0.1f : 0.1f;

			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x + particlePosX,
				 worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });
		}

		animationIndex_ = 4;
		UpdateAnimationTime(animationTime_, false, 40.0f, animationIndex_, model_);

		if (timerData_.downAnimationTimer < 38 && hp_ > 0.0f)
		{
			DownAnimationEnd(5, characterState_.isHitJumpAttack);
		}
	}

	//タックル攻撃
	if (characterState_.isHitTackle)
	{
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;


		float particlePosX = (characterState_.direction == Direction::Right) ? 0.1f : -0.1f;
		float moveX = (characterState_.direction == Direction::Right) ? -0.08f : 0.08f;

		aabb_ = (characterState_.direction == Direction::Right) ? AABB{ {-0.8f, -0.3f, -0.3f}, {-0.1f, 0.0f, 0.3f} } :
			AABB{ {0.1f, -0.3f, -0.3f}, {0.8f, 0.0f, 0.3f} };

		if (timerData_.downAnimationTimer > 55)
		{
			effectState_.isShake = true;

			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x + particlePosX,
						worldTransform_.translation.y + 0.5f, worldTransform_.translation.z });
		}
		else
		{
			effectState_.isShake = false;
		}

		if (timerData_.downAnimationTimer > 35 && ((characterState_.direction == Direction::Left && worldTransform_.translation.x < rightEdge_) ||
				(characterState_.direction == Direction::Right && worldTransform_.translation.x > leftEdge_)))
		{
			worldTransform_.translation.x += moveX;
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
		UpdateAnimationTime(animationTime_, false, 30.0f, animationIndex_, model_);

		SetAABB(aabb_);

		if (!player_->GetIsTackle() && hp_ > 0.0f)
		{
			DownAnimationEnd(5, characterState_.isHitTackle);
			ResetCollision();
		}
	}

	//アッパー攻撃
	if (characterState_.isHitUppercut)
	{
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;

		if (timerData_.downAnimationTimer > 55)
		{
			float particlePosX = (characterState_.direction == Direction::Right) ? 0.1f : -0.1f;

			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x + particlePosX,
				 worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });
		}

		animationIndex_ = 4;
		UpdateAnimationTime(animationTime_, false, 40.0f, animationIndex_, model_);

		if (!player_->GetIsUppercut() && hp_ > 0.0f)
		{
			DownAnimationEnd(5, characterState_.isHitUppercut);
		}
	}

	//超必殺技(1段目)
	if (characterState_.isHitFinisherFirstAttack)
	{
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;

		if (timerData_.downAnimationTimer > 55)
		{
			float particlePosX = (characterState_.direction == Direction::Right) ? 0.1f : -0.1f;

			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x + particlePosX,
				 worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });
		}

		animationIndex_ = 4;
		UpdateAnimationTime(animationTime_, false, 40.0f, animationIndex_, model_);

		if (!player_->GetIsFinisherFirstAttack())
		{
			DownAnimationEnd(5, characterState_.isHitFinisherFirstAttack);
		}
	}

	if (characterState_.isHitFinisherSecondAttack)
	{
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;

		if (timerData_.downAnimationTimer > 55)
		{
			float particlePosX = (characterState_.direction == Direction::Right) ? 0.1f : -0.1f;

			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x + particlePosX,
				 worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });
		}

		if (timerData_.downAnimationTimer < 50 && timerData_.downAnimationTimer > 45)
		{
			float particlePosX = (characterState_.direction == Direction::Right) ? 0.1f : -0.1f;

			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x + particlePosX,
				 worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });

			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			hp_ -= 1.0f;
		}

		if (timerData_.downAnimationTimer < 40 && timerData_.downAnimationTimer > 35)
		{
			float particlePosX = (characterState_.direction == Direction::Right) ? 0.1f : -0.1f;

			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x + particlePosX,
				 worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });

			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			hp_ -= 1.0f;
		}

		animationIndex_ = 4;
		UpdateAnimationTime(animationTime_, false, 40.0f, animationIndex_, model_);

		if (!player_->GetIsFinisherSecondAttack())
		{
			DownAnimationEnd(5, characterState_.isHitFinisherSecondAttack);
		}
	}
}

void Enemy::DownAnimationEnd(int animationIndex, bool& isHitAttackType)
{
	ICharacter::DownAnimationEnd(animationIndex, isHitAttackType);
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

void Enemy::HitCombo()
{
	//コンボを食らっているとき
	if (characterState_.isHitJumpAttack && comboCount_ == 0)
	{
		firstAttack_ = "JumpAttack";
		comboCount_ = 1;
		timerData_.comboTimer = 40;
		timerData_.comboTimer--;
	}

	if (characterState_.isHitLightPunch && comboCount_ == 0)
	{
		firstAttack_ = "LightPunch";
		comboCount_ = 1;
		timerData_.comboTimer = 10;
		timerData_.comboTimer--;
	}

	if (characterState_.isHitHighPunch && comboCount_ == 0)
	{
		firstAttack_ = "HighPunch";
		comboCount_ = 1;
		timerData_.comboTimer = 30;
		timerData_.comboTimer--;
	}

	if (characterState_.isHitFinisherFirstAttack && comboCount_ == 0)
	{
		firstAttack_ = "FinisherFirstAttack";
		comboCount_ = 1;
		timerData_.comboTimer = 120;
		timerData_.comboTimer--;
	}

	if (firstAttack_ == "JumpAttack")
	{
		if (characterState_.isHitLightPunch && comboCount_ == 1)
		{
			comboCount_ = 2;
			timerData_.comboTimer = 20;
			timerData_.comboTimer--;
		}

		if (characterState_.isHitTCMiddlePunch && comboCount_ == 2)
		{
			comboCount_ = 3;
			timerData_.comboTimer = 30;
			timerData_.comboTimer--;
		}

		if (characterState_.isHitUppercut && comboCount_ == 3)
		{
			comboCount_ = 4;
			timerData_.comboTimer = 40;
			timerData_.comboTimer--;
		}

		if (characterState_.isHitFinisherFirstAttack && comboCount_ == 4)
		{
			comboCount_ = 5;
			timerData_.comboTimer = 120;
			timerData_.comboTimer--;
		}

		if (characterState_.isHitFinisherSecondAttack)
		{
			if (comboCount_ == 5)
			{
				timerData_.comboTimer = 240;
			}

			if (timerData_.comboTimer > 0)
			{
				timerData_.comboTimer--;

				if (timerData_.comboTimer > 230)
				{
					comboCount_ = 6;
				}
				else if (timerData_.comboTimer > 220)
				{
					comboCount_ = 7;
				}
				else if (timerData_.comboTimer > 210)
				{
					comboCount_ = 8;
				}
			}
		}

		if (characterState_.isHitTackle && comboCount_ == 8)
		{
			comboCount_ = 9;
			timerData_.comboTimer = 40;
			timerData_.comboTimer--;
		}

		if (characterState_.isHitTCHighPunch && comboCount_ == 3)
		{
			comboCount_ = 4;
			timerData_.comboTimer = 50;
			timerData_.comboTimer--;
		}

		if (characterState_.isHitHighPunch && comboCount_ == 3)
		{
			comboCount_ = 4;
			timerData_.comboTimer = 65;
			timerData_.comboTimer--;
		}

		if (characterState_.isHitTackle && comboCount_ == 4)
		{
			comboCount_ = 5;
			timerData_.comboTimer = 60;
			timerData_.comboTimer--;
		}
	}

	if (firstAttack_ == "LightPunch")
	{
		if (characterState_.isHitTCMiddlePunch && comboCount_ == 1)
		{
			comboCount_ = 2;
			timerData_.comboTimer = 30;
			timerData_.comboTimer--;
		}

		if (characterState_.isHitTCHighPunch && comboCount_ == 2)
		{
			comboCount_ = 3;
			timerData_.comboTimer = 50;
			timerData_.comboTimer--;
		}

		if (characterState_.isHitUppercut && comboCount_ == 2)
		{
			comboCount_ = 3;
			timerData_.comboTimer = 40;
			timerData_.comboTimer--;
		}

		if (characterState_.isHitFinisherFirstAttack && comboCount_ == 3)
		{
			comboCount_ = 4;
			timerData_.comboTimer = 120;
			timerData_.comboTimer--;
		}

		if (characterState_.isHitFinisherSecondAttack)
		{
			if (comboCount_ == 4)
			{
				timerData_.comboTimer = 240;
			}

			if (timerData_.comboTimer > 0)
			{
				timerData_.comboTimer--;

				if (timerData_.comboTimer > 230)
				{
					comboCount_ = 5;
				}
				else if (timerData_.comboTimer > 220)
				{
					comboCount_ = 6;
				}
				else if (timerData_.comboTimer > 210)
				{
					comboCount_ = 7;
				}
			}
		}

		if (characterState_.isHitTackle && comboCount_ == 7)
		{
			comboCount_ = 8;
			timerData_.comboTimer = 40;
			timerData_.comboTimer--;
		}

		if (characterState_.isHitHighPunch && comboCount_ == 2)
		{
			comboCount_ = 3;
			timerData_.comboTimer = 65;
			timerData_.comboTimer--;
		}

		if (characterState_.isHitTackle && comboCount_ == 3)
		{
			comboCount_ = 4;
			timerData_.comboTimer = 40;
			timerData_.comboTimer--;
		}
	}

	if (firstAttack_ == "HighPunch")
	{
		if (characterState_.isHitTackle && comboCount_ == 1)
		{
			comboCount_ = 2;
			timerData_.comboTimer = 30;
			timerData_.comboTimer--;
		}
	}

	if (firstAttack_ == "FinisherFirstAttack")
	{
		if (characterState_.isHitFinisherSecondAttack)
		{
			if (comboCount_ == 1)
			{
				timerData_.comboTimer = 240;
			}

			if (timerData_.comboTimer > 0)
			{
				timerData_.comboTimer--;

				if (timerData_.comboTimer > 230)
				{
					comboCount_ = 2;
				}
				else if (timerData_.comboTimer > 220)
				{
					comboCount_ = 3;
				}
				else if (timerData_.comboTimer > 210)
				{
					comboCount_ = 4;
				}
			}
		}

		if (characterState_.isHitTackle && comboCount_ == 4)
		{
			comboCount_ = 5;
			timerData_.comboTimer = 40;
			timerData_.comboTimer--;
		}
	}

	if (comboCount_ >= 3)
	{
		if (characterState_.isHitJumpAttack)
		{
			firstAttack_ = "JumpAttack";
			comboCount_ = 1;
			timerData_.comboTimer = 40;
			timerData_.comboTimer--;
		}

		if (characterState_.isHitLightPunch)
		{
			firstAttack_ = "LightPunch";
			comboCount_ = 1;
			timerData_.comboTimer = 10;
			timerData_.comboTimer--;
		}
	}

	if (timerData_.comboTimer >= 0 && !player_->GetIsFinisher())
	{
		timerData_.comboTimer--;
	}

	if (timerData_.comboTimer < 0 || player_->GetIsDown())
	{
		timerData_.comboTimer = 0;
		comboCount_ = 0;
		firstAttack_ = "";
	}
}
