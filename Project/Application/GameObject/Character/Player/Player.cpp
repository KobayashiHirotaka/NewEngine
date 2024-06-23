#include "Player.h"

Player::~Player()
{
	delete hpBar_.sprite_;
	delete guardGaugeBar_.sprite_;
	delete finisherGaugeBar_.sprite_;
}

void Player::Initialize()
{
	IGame3dObject::SetTag("Player");

	//modelManagerのinstance
	modelManager_ = ModelManager::GetInstance();

	//inputのinstance
	input_ = Input::GetInstance();

	//audioのinstance
	audio_ = Audio::GetInstance();

	//worldTransformの初期化
	worldTransform_.Initialize();

	//リソースの初期化(sprite,se)
	//各ゲージの初期化
	hpBar_ = {
		true,
		TextureManager::LoadTexture("resource/images/HP.png"),
		{60.0f, barSpace},
		0.0f,
		{-barSize  ,7.0f},
		nullptr,
	};

	hpBar_.sprite_ = Sprite::Create(hpBar_.textureHandle_, hpBar_.position_);

	guardGaugeBar_ = {
		true,
		TextureManager::LoadTexture("resource/images/guardGauge.png"),
		{540.0f, guardGaugeBarSpace},
		0.0f,
		{guardGaugeBarSize  ,7.0f},
		nullptr,
	};

	guardGaugeBar_.sprite_ = Sprite::Create(guardGaugeBar_.textureHandle_, guardGaugeBar_.position_);

	finisherGaugeBar_ = {
		true,
		TextureManager::LoadTexture("resource/images/guardGauge.png"),
		{60.0f, finisherGaugeBarSpace},
		0.0f,
		{-finisherGaugeBarSize  ,20.0f},
		nullptr,
	};

	finisherGaugeBar_.sprite_ = Sprite::Create(finisherGaugeBar_.textureHandle_, finisherGaugeBar_.position_);

	//seの初期化
	attackSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/Attack.mp3");
	weaponAttackSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/WeaponAttack.mp3");
	damageSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/Damage.mp3");
	guardSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/Guard.mp3");

	////カーソルの初期化
	//playerCursol_.reset(Model::CreateFromOBJ("resource/playerCursol", "playerCursol.obj"));

	//worldTransformCursol_.Initialize();
	//worldTransformCursol_.translation.x = 0.9f;
	//worldTransformCursol_.translation.y = worldTransform_.translation.y + 2.0f;

	//worldTransformCursol_.parent_ = &worldTransform_;

	//パーティクルの初期化
	particleModel_.reset(ParticleModel::CreateFromOBJ("resource/Particle", "Particle.obj"));
	particleSystem_ = std::make_unique<ParticleSystem>();
	particleSystem_->Initialize();
}

void Player::Update()
{
	//テスト用の処理
	if (input_->PressKey(DIK_A))
	{
		guardGauge_ -= 1.0f;
	}

	if (input_->PressKey(DIK_D))
	{
		finisherGauge_ += 1.0f;
	}
	//ここまでテスト用の処理

	isShake_ = false;

	//0は停止、1は攻撃(振り下ろし),2は歩き
	model_->ApplyAnimation(animationIndex);

	model_->Update();

	//各ゲージの更新処理
	HPBarUpdate();

	if (guardGauge_ <= 50.0f)
	{
		GuardGaugeBarUpdate();
	}

	FinisherGaugeBarUpdate();

	//PlayerのBehavior
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
		BehaviorRootUpdate();
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

	//imGui
	model_->GetLight()->ImGui("DirectionalLight");
	model_->GetPointLight()->ImGui("PointLight");
	model_->GetSpotLight()->ImGui("SpotLight");

	ImGui::Begin("Player");
	ImGui::SliderFloat3("WTFT", &worldTransform_.translation.x, -100.0f, 100.0f);
	ImGui::SliderFloat3("WTFR", &worldTransform_.rotation.x, 0.0f, 16.0f);
	ImGui::Text("isGuard %d", isGuard_);
	ImGui::End();

	//worldTransformの更新
	worldTransform_.UpdateMatrixEuler();
	//worldTransformCursol_.UpdateMatrixEuler();
}

void Player::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, camera, animationIndex);

	/*if (!isDown_)
	{
		playerCursol_->Draw(worldTransformCursol_, camera, 0);
	}*/
}

void Player::BoneDraw(const Camera& camera)
{
	model_->BoneDraw(worldTransform_, camera, animationIndex);
}

void Player::DrawSprite()
{
	if (HP_ >= 0)
	{
		hpBar_.sprite_->Draw();
	}

	guardGaugeBar_.sprite_->Draw();

	finisherGaugeBar_.sprite_->Draw();
}

void Player::DrawParticle(const Camera& camera)
{
	particleModel_->Draw(particleSystem_.get(), camera);
}

void Player::BehaviorRootInitialize()
{

}

void Player::BehaviorRootUpdate()
{
	//コントローラーの移動処理
	if (input_->GetJoystickState())
	{
		const float deadZone = 0.7f;
		bool isFrontMove_ = false;
		bool isBackMove_ = false;

		float animationTime = 0.0f;

		velocity_ = { 0.0f, 0.0f, 0.0f };

		//振り向きの処理
		if (worldTransform_.translation.x > 15.0f)
		{
			playerDirection = Direction::Left;
			worldTransform_.rotation.y = 1.7f;
		}
		else if (worldTransform_.translation.x <= 15.0f)
		{
			playerDirection = Direction::Right;
			worldTransform_.rotation.y = 4.6f;
		}

		/*Vector3 playerWorldPosition = GetWorldPosition();

		Vector3 enemyWorldPosition = enemy_->GetWorldPosition();

		if (enemyWorldPosition.x > playerWorldPosition.x)
		{
			worldTransform_.rotation.y = 1.7f;
		}

		if (enemyWorldPosition.x < playerWorldPosition.x)
		{
			worldTransform_.rotation.y = 4.6f;
		}*/

		//移動処理
		//前方向に移動(左を向いているとき)
		if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT) && playerDirection == Direction::Left && isDown_ == false && !isHit_)
		{
			velocity_.x = -0.3f;
			isFrontMove_ = true;
			isGuard_ = false;
		}

		//前方向に移動(右を向いているとき)
		if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT) && playerDirection == Direction::Right && isDown_ == false && !isHit_)
		{
			velocity_.x = 0.3f;
			isFrontMove_ = true;
			isGuard_ = false;
		}

		//後ろ方向に移動(右を向いているとき)
		if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT) && playerDirection == Direction::Right && isDown_ == false)
		{
			isGuard_ = true;

			//移動しながらガード
			if (!input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN))
			{
				velocity_.x = -0.3f;
				isBackMove_ = true;
			}

			//止まってガード
			if (isGuard_ && input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP))
			{
				velocity_.x = 0.0f;
				isBackMove_ = false;
			}
		}

		//後ろ方向に移動(左を向いているとき)
		if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT) && playerDirection == Direction::Left && isDown_ == false)
		{
			isGuard_ = true;

			//移動しながらガード
			if (!input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN))
			{
				velocity_.x = 0.3f;
				isBackMove_ = true;
			}

			//止まってガード
			if (isGuard_ && input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP))
			{
				velocity_.x = 0.0f;
				isBackMove_ = false;
			}
		}

		//移動していない時
		if (!input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT))
		{
			isGuard_ = false;
		}

		//移動
		if (isFrontMove_)
		{
			animationIndex = 2;

			UpdateAnimationTime(animationTime, true, 30.0f, animationIndex, model_);

			velocity_ = Normalize(velocity_);
			velocity_ = Multiply(characterFrontSpeed_, velocity_);

			// 平行移動
			worldTransform_.translation = Add(worldTransform_.translation, velocity_);

			worldTransform_.UpdateMatrixEuler();
		}
		else if (isBackMove_)
		{
			animationIndex = 2;

			UpdateAnimationTime(animationTime, true, 60.0f, animationIndex, model_);

			velocity_ = Normalize(velocity_);
			velocity_ = Multiply(characterBackSpeed_, velocity_);

			// 平行移動
			worldTransform_.translation = Add(worldTransform_.translation, velocity_);

			worldTransform_.UpdateMatrixEuler();
		}
		else
		{
			animationIndex = 0;

			UpdateAnimationTime(animationTime, true, 60.0f, animationIndex, model_);
		}

		//ジャンプ
		if (input_->GetJoystickState())
		{
			if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP) && !input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) && isDown_ == false)
			{
				behaviorRequest_ = Behavior::kJump;
			}
		}
	}
}

void Player::BehaviorAttackInitialize()
{

}

void Player::BehaviorAttackUpdate()
{

}

void Player::BehaviorJumpInitialize()
{
	worldTransform_.translation.y = 0.0f;

	const float kJumpFirstSpeed_ = 0.6f;

	velocity_.y = kJumpFirstSpeed_;
}

void Player::BehaviorJumpUpdate()
{
	float animationTime = 0.0f;
	animationIndex = 1;

	UpdateAnimationTime(animationTime, true, 60.0f, animationIndex, model_);

	worldTransform_.translation = Add(worldTransform_.translation, velocity_);

	const float kGravityAcceleration_ = 0.03f;

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

void Player::BehaviorThrowInitialize()
{

}

void Player::BehaviorThrowUpdate()
{

}

void Player::BehaviorStanInitialize()
{

}

void Player::BehaviorStanUpdate()
{

}

void Player::UpdateAnimationTime(float animationTime, bool isLoop, float frameRate, int animationIndex, std::unique_ptr<Model>& modelFighterBody)
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

void Player::OnCollision(Collider* collider, float damage)
{
	/*if (collider->GetCollisionAttribute() & kCollisionAttributeEnemy)
	{
		isHit_ = true;

		if (enemy_->GetIsPunch() == true && isDown_ == false)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 15.0f;
			HP_ -= damage;
			isHitPunch_ = true;

			HitStop(20);
		}

		if (enemy_->GetIsThrow() == true && isDown_ == false)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 50.0f;
			HP_ -= damage;
			isEnemyHit_ = true;
			isHitThrow_ = true;
		}
	}

	if (collider->GetCollisionAttribute() & kCollisionAttributeEnemyWeapon)
	{
		if (isGuard_ && worldTransform_.rotation.y == 1.7f)
		{
			audio_->SoundPlayMP3(guardSoundHandle_, false, 1.0f);
			worldTransform_.translation.x -= 0.3f;
			guardGauge_ += 2.0f;

			ParticleEmitter* newParticleEmitter = EmitterBuilder()
				.SetParticleType(ParticleEmitter::ParticleType::kNormal)
				.SetTranslation(worldTransform_.translation)
				.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetScale({ 0.2f, 0.2f,0.2f }, { 0.6f ,0.6f ,0.6f })
				.SetAzimuth(0.0f, 360.0f)
				.SetElevation(0.0f, 0.0f)
				.SetVelocity({ 0.06f ,0.06f ,0.06f }, { 0.1f ,0.1f ,0.1f })
				.SetColor({ 1.0f ,1.0f ,1.0f ,1.0f }, { 1.0f ,1.0f ,1.0f ,1.0f })
				.SetLifeTime(0.1f, 1.0f)
				.SetCount(100)
				.SetFrequency(4.0f)
				.SetDeleteTime(2.0f)
				.Build();
			particleSystem_->AddParticleEmitter(newParticleEmitter);
		}

		if (isGuard_ && worldTransform_.rotation.y == 4.6f)
		{
			audio_->SoundPlayMP3(guardSoundHandle_, false, 1.0f);
			worldTransform_.translation.x += 0.3f;
			guardGauge_ += 2.0f;

			ParticleEmitter* newParticleEmitter = EmitterBuilder()
				.SetParticleType(ParticleEmitter::ParticleType::kNormal)
				.SetTranslation(worldTransform_.translation)
				.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetScale({ 0.2f, 0.2f,0.2f }, { 0.6f ,0.6f ,0.6f })
				.SetAzimuth(0.0f, 360.0f)
				.SetElevation(0.0f, 0.0f)
				.SetVelocity({ 0.06f ,0.06f ,0.06f }, { 0.1f ,0.1f ,0.1f })
				.SetColor({ 1.0f ,1.0f ,1.0f ,1.0f }, { 1.0f ,1.0f ,1.0f ,1.0f })
				.SetLifeTime(0.1f, 1.0f)
				.SetCount(100)
				.SetFrequency(4.0f)
				.SetDeleteTime(2.0f)
				.Build();
			particleSystem_->AddParticleEmitter(newParticleEmitter);
		}

		if (enemy_->GetIsAttack() == true && enemy_->GetIsSwingDown() == true && isDown_ == false
			&& isGuard_ == false)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 30.0f;
			HP_ -= damage;
			isHitSwingDown_ = true;
			isShake_ = true;

			HitStop(100);
		}

		if (enemy_->GetIsAttack() == true && enemy_->GetIsPoke() == true && isDown_ == false
			&& isGuard_ == false)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 20.0f;
			HP_ -= damage;
			isHitPoke_ = true;
			isShake_ = true;

			HitStop(100);
		}

		if (enemy_->GetIsAttack() == true && enemy_->GetIsMowDown() == true && isDown_ == false
			&& isGuard_ == false)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 30.0f;
			HP_ -= damage;
			isHitMowDown_ = true;
			isShake_ = true;

			HitStop(100);
		}
	}*/
}

void Player::HPBarUpdate()
{
	hpBar_.size_ = { (HP_ / maxHP_) * barSize,7.0f };

	hpBar_.sprite_->SetSize(hpBar_.size_);

	if (HP_ > 50)
	{
		hpBar_.sprite_->SetColor({ 0.0f, 1.0f, 0.0f, 1.0f });
	}

	if (HP_ <= 50 && HP_ > 25)
	{
		hpBar_.sprite_->SetColor({ 1.0f, 0.8f, 0.0f, 1.0f });
	}
	else if (HP_ <= 25)
	{
		hpBar_.sprite_->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });
	}
}

void Player::GuardGaugeBarUpdate()
{
	if (guardGauge_ < 0 && guardGauge_ > -50.0f)
	{
		guardGauge_ += 0.03f;
	}

	guardGaugeBar_.size_ = { (guardGauge_ / maxGuardGauge_) * guardGaugeBarSize,7.0f };

	guardGaugeBar_.sprite_->SetSize(guardGaugeBar_.size_);

	guardGaugeBar_.sprite_->SetColor({ 0.0f, 0.5f, 1.0f, 1.0f });

	if (guardGauge_ <= -50.0f)
	{
		guardGauge_ = -50.0f;
		behaviorRequest_ = Behavior::kStan;
	}
}

void Player::FinisherGaugeBarUpdate()
{

	/*if (enemy_->GetIsDown() == false && enemy_->GetIsHitPunch())
	{
		finisherGauge_ += 3.0f;
	}

	if (enemy_->GetIsDown() == false && enemy_->GetIsHitSwingDown())
	{
		finisherGauge_ += 8.0f;
	}

	if (enemy_->GetIsDown() == false && enemy_->GetIsHitPoke())
	{
		finisherGauge_ += 6.0f;
	}

	if (enemy_->GetIsDown() == false && enemy_->GetIsHitMowDown())
	{
		finisherGauge_ += 8.0f;
	}

	if (enemy_->GetIsDown() == false && enemy_->GetIsHitThrow())
	{
		finisherGauge_ += 5.0f;
	}*/

	finisherGaugeBar_.size_ = { (finisherGauge_ / maxFinisherGauge_) * finisherGaugeBarSize,20.0f };

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

Vector3 Player::GetWorldPosition()
{
	Vector3 pos{};
	pos.x = worldTransform_.matWorld.m[3][0];
	pos.y = worldTransform_.matWorld.m[3][1];
	pos.z = worldTransform_.matWorld.m[3][2];
	return pos;
}