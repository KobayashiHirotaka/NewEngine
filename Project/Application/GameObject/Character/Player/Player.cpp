#include "Player.h"
#include "Application/GameObject/Character/Enemy/Enemy.h"
#include "Application/Game/Scenes/GamePlayScene.h"

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

	//当たり判定の設定
	SetAABB(aabb_);

	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(kCollisionMaskPlayer);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);

	//リソースの初期化(sprite,se)
	//各ゲージの初期化
	hpBar_ = {
		true,
		TextureManager::LoadTexture("resource/images/HP.png"),
		{537.0f, barSpace_},
		0.0f,
		{-barSize_  ,7.2f},
		nullptr,
	};

	hpBar_.sprite_ = Sprite::Create(hpBar_.textureHandle_, hpBar_.position_);

	guardGaugeBar_ = {
		true,
		TextureManager::LoadTexture("resource/images/guardGauge.png"),
		{537.0f, guardGaugeBarSpace_},
		0.0f,
		{-guardGaugeBarSize_  ,7.0f},
		nullptr,
	};

	guardGaugeBar_.sprite_ = Sprite::Create(guardGaugeBar_.textureHandle_, guardGaugeBar_.position_);

	finisherGaugeBar_ = {
		true,
		TextureManager::LoadTexture("resource/images/guardGauge.png"),
		{299.0f, finisherGaugeBarSpace_},
		0.0f,
		{-finisherGaugeBarSize_  ,20.0f},
		nullptr,
	};

	finisherGaugeBar_.sprite_ = Sprite::Create(finisherGaugeBar_.textureHandle_, finisherGaugeBar_.position_);

	//seの初期化
	attackSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/Attack.mp3");
	weaponAttackSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/WeaponAttack.mp3");
	damageSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/Damage.mp3");
	guardSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/Guard.mp3");

	//カーソルの初期化
	playerCursol_.reset(Model::CreateFromOBJ("resource/playerCursol", "playerCursol.obj"));

	worldTransformCursol_.Initialize();
	worldTransformCursol_.translation = { worldTransform_.translation.x - 0.3f,worldTransform_.translation.y + 0.4f,worldTransform_.translation.z };
	worldTransformCursol_.rotation.y = 1.5f;
	worldTransformCursol_.scale = { 0.3f, 0.3f, 0.3f };

	worldTransformCursol_.parent_ = &worldTransform_;

	//パーティクルの初期化
	particleModel_.reset(ParticleModel::CreateFromOBJ("resource/Particle", "Particle.obj"));
	particleSystem_ = std::make_unique<ParticleSystem>();
	particleSystem_->Initialize();

	//worldTransformの更新
	worldTransform_.UpdateMatrixEuler();
}

void Player::Update()
{
	//テスト用の処理
	//ゲージテスト用
	if (input_->PressKey(DIK_A))
	{
		guardGauge_ -= 1.0f;
	}

	//パーティクルテスト用
	if (input_->PressKey(DIK_P))
	{
		ParticleEmitter* newParticleEmitter = EmitterBuilder()
			.SetParticleType(ParticleEmitter::ParticleType::kNormal)
			.SetTranslation({ worldTransform_.translation.x + 0.1f,
					worldTransform_.translation.y + 0.6f,  worldTransform_.translation.z })
			.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetScale({ 0.1f, 0.1f, 0.1f }, { 0.4f ,0.4f ,0.4f })
			.SetAzimuth(172.0f, 188.0f)
			.SetElevation(0.0f, 0.0f)
			.SetVelocity({ 0.08f ,0.08f ,0.08f }, { 0.1f ,0.1f ,0.1f })
			.SetColor({ 1.0f ,0.0f ,0.0f ,1.0f }, { 1.0f ,0.5f ,0.0f ,1.0f })
			.SetLifeTime(0.1f, 0.6f)
			.SetCount(100)
			.SetFrequency(4.0f)
			.SetDeleteTime(2.0f)
			.Build();
		particleSystem_->AddParticleEmitter(newParticleEmitter);
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
	if (input_->PressKey(DIK_W))
	{
		animationIndex_ = 11;
	}
	if (input_->PressKey(DIK_E))
	{
		animationIndex_ = 12;
	}
	//ここまでテスト用の処理

	isShake_ = false;

	//0は後ろ歩き, 1は前歩き, 2はライトリアクション,3は停止,4はノックダウン,5はTC強P,6は弱P,7はTC中P
	model_->ApplyAnimation(animationIndex_);

	model_->Update();

	//リセットのときの処理
	if (isReset_)
	{
		Reset();
	}

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
	Vector3 playerWorldPosition = GetWorldPosition();

	Vector3 enemyWorldPosition = enemy_->GetWorldPosition();

	if (enemyWorldPosition.x > playerWorldPosition.x && behavior_ != Behavior::kJump
		&& behavior_ != Behavior::kAttack && !isDown_)
	{
		playerDirection_ = Direction::Right;
		worldTransform_.rotation.y = 1.7f;
	}

	if (enemyWorldPosition.x < playerWorldPosition.x && behavior_ != Behavior::kJump
		&& behavior_ != Behavior::kAttack && !isDown_)
	{
		playerDirection_ = Direction::Left;
		worldTransform_.rotation.y = 4.6f;
	}

	if (!workAttack_.isAttack && worldTransform_.translation.x >= 3.5f && playerDirection_ == Direction::Right)
	{
		worldTransform_.translation.x = 3.5f;
	}

	if (!workAttack_.isAttack && worldTransform_.translation.x <= -3.5f && playerDirection_ == Direction::Left)
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

	//ジャンプ中に敵と当たったときの処理
	if (behaviorRequest_ == Behavior::kJump && isHit_)
	{
		worldTransform_.translation.y = 0.0f;
	}

	DownAnimation();

	//パーティクルの更新
	particleSystem_->Update();

	isHit_ = false;

	//各ゲージの更新処理
	HPBarUpdate();

	GuardGaugeBarUpdate();

	FinisherGaugeBarUpdate();

	if (!enemy_->GetIsAttack())
	{
		guardAnimationTimer_ = 60;
	}

	//imGui
	model_->GetLight()->ImGui("DirectionalLight");
	model_->GetPointLight()->ImGui("PointLight");
	model_->GetSpotLight()->ImGui("SpotLight");

	ImGui::Begin("Player");
	ImGui::SliderFloat3("WTFT", &worldTransform_.translation.x, -100.0f, 100.0f);
	ImGui::SliderFloat3("WTFR", &worldTransform_.rotation.x, 0.0f, 16.0f);
	ImGui::Text("isGuard %d", isGuard_);
	ImGui::Text("attackAnimationFrame %d", attackAnimationFrame_);
	ImGui::Text("isAttack %d", workAttack_.isAttack);
	ImGui::End();

	ImGui::Begin("Cursol");
	ImGui::SliderFloat3("WTFT", &worldTransformCursol_.translation.x, -100.0f, 100.0f);
	ImGui::SliderFloat3("WTFR", &worldTransformCursol_.rotation.x, 0.0f, 16.0f);
	ImGui::SliderFloat3("WTFS", &worldTransformCursol_.scale.x, 0.0f, 16.0f);
	ImGui::End();

	//worldTransformの更新
	worldTransform_.UpdateMatrixEuler();
	worldTransformCursol_.UpdateMatrixEuler();
}

void Player::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, camera, animationIndex_);

	if (!isDown_)
	{
		playerCursol_->Draw(worldTransformCursol_, camera, 0);
	}
}

void Player::BoneDraw(const Camera& camera)
{
	model_->BoneDraw(worldTransform_, camera, animationIndex_);
}

void Player::DrawSprite()
{
	if (hp_ <= 0)
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

		//敵の位置を取得する（例: enemyPosition という変数）
		Vector3 enemyPosition = enemy_->GetWorldPosition();

		if (isHit_)
		{
			if (playerDirection_ == Direction::Right && input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT))
			{
				// 敵を右方向に押す
				PushEnemy(enemyPosition, 0.04f);
			}
			else if (playerDirection_ == Direction::Left && input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT))
			{
				// 敵を左方向に押す
				PushEnemy(enemyPosition, -0.04f);
			}
		}

		//移動処理
		//前方向に移動(左を向いているとき)
		if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT) && playerDirection_ == Direction::Left && !isDown_)
		{
			velocity_.x = -0.01f;
			isFrontMove_ = true;
			isGuard_ = false;
		}

		//前方向に移動(右を向いているとき)
		if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT) && playerDirection_ == Direction::Right && !isDown_)
		{
			velocity_.x = 0.01f;
			isFrontMove_ = true;
			isGuard_ = false;
		}

		//後ろ方向に移動(右を向いているとき)
		if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT) && playerDirection_ == Direction::Right && !isDown_)
		{
			isGuard_ = true;

			//移動しながらガード
			if (!input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN))
			{
				velocity_.x = -0.01f;
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
		if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT) && playerDirection_ == Direction::Left && !isDown_)
		{
			isGuard_ = true;

			//移動しながらガード
			if (!input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN))
			{
				velocity_.x = 0.01f;
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
			animationIndex_ = 1;

			UpdateAnimationTime(animationTime, true, 30.0f, animationIndex_, model_);

			velocity_ = Normalize(velocity_);
			velocity_ = Multiply(frontSpeed_, velocity_);

			// 平行移動
			worldTransform_.translation = Add(worldTransform_.translation, velocity_);

			worldTransform_.UpdateMatrixEuler();
		}
		else if (isBackMove_)
		{
			animationIndex_ = 0;

			UpdateAnimationTime(animationTime, true, 30.0f, animationIndex_, model_);

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

		//ジャンプ
		if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP) && !input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) && !isDown_)
		{
			behaviorRequest_ = Behavior::kJump;
		}

		//攻撃
		//弱攻撃
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X) && !isDown_)
		{
			behaviorRequest_ = Behavior::kAttack;
			animationTime = 0.0f;
			model_->SetAnimationTime(animationTime);
			workAttack_.isLightPunch = true;
		}

		//中攻撃
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_Y) && !isDown_)
		{
			behaviorRequest_ = Behavior::kAttack;
			animationTime = 0.0f;
			model_->SetAnimationTime(animationTime);
			workAttack_.isMiddlePunch = true;
		}

		//強攻撃
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B) && !isDown_)
		{
			behaviorRequest_ = Behavior::kAttack;
			animationTime = 0.0f;
			model_->SetAnimationTime(animationTime);
			workAttack_.isHighPunch = true;
		}
		
		//タックル攻撃
		//右向きのとき
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) && input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT)
			&& playerDirection_ == Direction::Right && !isDown_)
		{
			behaviorRequest_ = Behavior::kAttack;
			animationTime = 0.0f;
			model_->SetAnimationTime(animationTime);
			workAttack_.isTackle = true;
		}

		//タックル攻撃
		//左向きのとき
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) && input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT)
			&& playerDirection_ == Direction::Left && !isDown_)
		{
			behaviorRequest_ = Behavior::kAttack;
			animationTime = 0.0f;
			model_->SetAnimationTime(animationTime);
			workAttack_.isTackle = true;
		}
	}
}

void Player::BehaviorAttackInitialize()
{
	attackAnimationFrame_ = 0;
}

void Player::BehaviorAttackUpdate()
{
	//弱攻撃
	if (workAttack_.isLightPunch)
	{
		animationIndex_ = 12;
		isGuard_ = false;
		float animationTime = 0.0f;
		float animationDuration;
		animationTime = model_->GetAnimationTime();
		animationDuration = model_->GetAnimation()[animationIndex_].duration;

		if (!isDown_)
		{
			animationTime += 1.0f / 40.0f;
		}

		model_->SetAnimationTime(animationTime);
		model_->ApplyAnimation(animationIndex_);

		workAttack_.isAttack = true;

		if (playerDirection_ == Direction::Right)
		{
			aabb_ = { {-0.3f,-0.3f,-0.3f},{0.5f,0.0f,0.3f} };
			SetAABB(aabb_);
		}
		else if (playerDirection_ == Direction::Left)
		{
			aabb_ = { {-0.5f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
			SetAABB(aabb_);
		}

		if (isDown_ || animationTime >= animationDuration)
		{
			behaviorRequest_ = Behavior::kRoot;
			workAttack_.isAttack = false;
			workAttack_.isLightPunch = false;
			animationTime = 0.0f;
			attackAnimationFrame_ = 0;
			model_->SetAnimationTime(animationTime);
			aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
			SetAABB(aabb_);
		}

		//キャンセルの処理(中TC)
		if (input_->GetJoystickState())
		{
			if (!isDown_ && attackAnimationFrame_ > 15 && attackAnimationFrame_ < 30 && animationTime < animationDuration
				&& input_->IsPressButtonEnter(XINPUT_GAMEPAD_X) && input_->IsPressButton(XINPUT_GAMEPAD_RIGHT_SHOULDER) 
				&& isHit_)
			{
				workAttack_.isAttack = false;
				workAttack_.isLightPunch = false;
				workAttack_.isTCMiddlePunch = true;
				animationTime = 0.0f;
				attackAnimationFrame_ = 0;
				model_->SetAnimationTime(animationTime);
				aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
				SetAABB(aabb_);
			}
		}

		attackAnimationFrame_++;
	}

	//TC用の攻撃(2発目)
	if (workAttack_.isTCMiddlePunch)
	{
		animationIndex_ = 11;
		isGuard_ = false;
		float animationTime = 0.0f;
		float animationDuration;
		animationTime = model_->GetAnimationTime();
		animationDuration = model_->GetAnimation()[animationIndex_].duration;

		if (!isDown_)
		{
			animationTime += 1.0f / 40.0f;
		}

		model_->SetAnimationTime(animationTime);
		model_->ApplyAnimation(animationIndex_);

		workAttack_.isAttack = true;

		if (playerDirection_ == Direction::Right)
		{
			aabb_ = { {-0.3f,-0.3f,-0.3f},{0.5f,0.0f,0.3f} };
			SetAABB(aabb_);
		}
		else if (playerDirection_ == Direction::Left)
		{
			aabb_ = { {-0.5f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
			SetAABB(aabb_);
		}

		if (isDown_ || animationTime >= animationDuration)
		{
			behaviorRequest_ = Behavior::kRoot;
			workAttack_.isAttack = false;
			workAttack_.isTCMiddlePunch = false;
			animationTime = 0.0f;
			attackAnimationFrame_ = 0;
			model_->SetAnimationTime(animationTime);
			aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
			SetAABB(aabb_);
		}

		//キャンセルの処理(強TC)
		if (input_->GetJoystickState())
		{
			if (!isDown_ && attackAnimationFrame_ > 15 && attackAnimationFrame_ < 30 && animationTime < animationDuration
				&& input_->IsPressButtonEnter(XINPUT_GAMEPAD_X) && input_->IsPressButton(XINPUT_GAMEPAD_RIGHT_SHOULDER)
				&& isHit_)
			{
				workAttack_.isAttack = false;
				workAttack_.isTCMiddlePunch = false;
				workAttack_.isHighPunch = true;
				animationTime = 0.0f;
				attackAnimationFrame_ = 0;
				model_->SetAnimationTime(animationTime);
				aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
				SetAABB(aabb_);
			}
		}

		////キャンセルの処理(強TC)
		//if (input_->GetJoystickState())
		//{
		//	if (!isDown_ && attackAnimationFrame > 20 && animationTime < animationDuration
		//		&& input_->IsPressButton(XINPUT_GAMEPAD_RIGHT_SHOULDER) && isHit_)
		//	{
		//		workAttack_.isAttack = false;
		//		workAttack_.isTCMiddlePunch = false;
		//		workAttack_.isTCHighPunch = true;
		//		animationTime = 0.0f;
		//		attackAnimationFrame = 0;
		//		model_->SetAnimationTime(animationTime);
		//		aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
		//		SetAABB(aabb_);
		//	}
		//}

		attackAnimationFrame_++;
	}

	//TC用の攻撃(3発目)
	if (workAttack_.isTCHighPunch)
	{
		animationIndex_ = 10;
		isGuard_ = false;
		float animationTime = 0.0f;
		float animationDuration;
		animationTime = model_->GetAnimationTime();
		animationDuration = model_->GetAnimation()[animationIndex_].duration;

		if (!isDown_)
		{
			animationTime += 1.0f / 40.0f;
		}

		model_->SetAnimationTime(animationTime);
		model_->ApplyAnimation(animationIndex_);

		workAttack_.isAttack = true;

		if (playerDirection_ == Direction::Right)
		{
			aabb_ = { {-0.3f,-0.3f,-0.3f},{0.5f,0.0f,0.3f} };
			SetAABB(aabb_);
		}
		else if (playerDirection_ == Direction::Left)
		{
			aabb_ = { {-0.5f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
			SetAABB(aabb_);
		}

		if (isDown_ || animationTime >= animationDuration)
		{
			behaviorRequest_ = Behavior::kRoot;
			workAttack_.isAttack = false;
			workAttack_.isTCHighPunch = false;
			animationTime = 0.0f;
			attackAnimationFrame_ = 0;
			model_->SetAnimationTime(animationTime);
			aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
			SetAABB(aabb_);
		}

		attackAnimationFrame_++;
	}

	//中攻撃
	if (workAttack_.isMiddlePunch)
	{
		animationIndex_ = 7;
		isGuard_ = false;
		float animationTime = 0.0f;
		float animationDuration;
		animationTime = model_->GetAnimationTime();
		animationDuration = model_->GetAnimation()[animationIndex_].duration;

		if (!isDown_)
		{
			animationTime += 1.0f / 40.0f;
		}

		model_->SetAnimationTime(animationTime);
		model_->ApplyAnimation(animationIndex_);

		workAttack_.isAttack = true;

		if (playerDirection_ == Direction::Right)
		{
			aabb_ = { {-0.3f,-0.3f,-0.3f},{0.6f,0.0f,0.3f} };
			SetAABB(aabb_);
		}
		else if (playerDirection_ == Direction::Left)
		{
			aabb_ = { {-0.6f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
			SetAABB(aabb_);
		}

		if (isDown_ || animationTime >= animationDuration)
		{
			behaviorRequest_ = Behavior::kRoot;
			workAttack_.isAttack = false;
			workAttack_.isMiddlePunch = false;
			animationTime = 0.0f;
			attackAnimationFrame_ = 0;
			model_->SetAnimationTime(animationTime);
			aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
			SetAABB(aabb_);
		}

		attackAnimationFrame_++;
	}

	//強攻撃
	if (workAttack_.isHighPunch)
	{
		animationIndex_ = 2;
		isGuard_ = false;
		float animationTime = 0.0f;
		float animationDuration;
		animationTime = model_->GetAnimationTime();
		animationDuration = model_->GetAnimation()[animationIndex_].duration;

		if (!isDown_)
		{
			animationTime += 1.0f / 40.0f;
		}

		model_->SetAnimationTime(animationTime);
		model_->ApplyAnimation(animationIndex_);

		workAttack_.isAttack = true;

		if (playerDirection_ == Direction::Right)
		{
			aabb_ = { {-0.3f,-0.3f,-0.3f},{0.9f,0.3f,0.3f} };
			SetAABB(aabb_);

			if (isHit_ && attackAnimationFrame_ <= 15)
			{
				worldTransform_.translation.x -= 0.05f;
			}
		}
		else if (playerDirection_ == Direction::Left)
		{
			aabb_ = { {-0.9f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
			SetAABB(aabb_);

			if (isHit_ && attackAnimationFrame_ <= 15)
			{
				worldTransform_.translation.x += 0.05f;
			}
		}

		if (isDown_ || animationTime >= animationDuration)
		{
			behaviorRequest_ = Behavior::kRoot;
			workAttack_.isAttack = false;
			workAttack_.isHighPunch = false;
			animationTime = 0.0f;
			attackAnimationFrame_ = 0;
			model_->SetAnimationTime(animationTime);
			aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
			SetAABB(aabb_);
		}

		//キャンセルの処理(横A)
		if (input_->GetJoystickState())
		{
			//タックル攻撃
			//右向きのとき
			if (!isDown_ && attackAnimationFrame_ > 15 && attackAnimationFrame_ < 30 && animationTime < animationDuration
				&& input_->IsPressButtonEnter(XINPUT_GAMEPAD_X) && input_->IsPressButton(XINPUT_GAMEPAD_RIGHT_SHOULDER)
				&& playerDirection_ == Direction::Right)
			{
				workAttack_.isAttack = false;
				workAttack_.isHighPunch = false;
				workAttack_.isTackle = true;
				animationTime = 0.0f;
				attackAnimationFrame_ = 0;
				model_->SetAnimationTime(animationTime);
				aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
				SetAABB(aabb_);
			}

			//タックル攻撃
			//左向きのとき
			if (!isDown_ && attackAnimationFrame_ > 10 && animationTime < animationDuration
				&& input_->IsPressButtonEnter(XINPUT_GAMEPAD_X) && input_->IsPressButton(XINPUT_GAMEPAD_RIGHT_SHOULDER)
				&& playerDirection_ == Direction::Left)
			{
				workAttack_.isAttack = false;
				workAttack_.isHighPunch = false;
				workAttack_.isTackle = true;
				animationTime = 0.0f;
				attackAnimationFrame_ = 0;
				model_->SetAnimationTime(animationTime);
				aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
				SetAABB(aabb_);
			}
		}

		attackAnimationFrame_++;
	}

	//タックル攻撃
	if (workAttack_.isTackle)
	{
		animationIndex_ = 8;
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

		if (playerDirection_ == Direction::Right)
		{
			aabb_ = { {-0.3f,-0.3f,-0.3f},{0.6f,0.3f,0.3f} };
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
					.SetColor({ 1.0f ,0.0f ,0.0f ,1.0f }, { 1.0f ,0.5f ,0.0f ,1.0f })
					.SetLifeTime(0.1f, 0.6f)
					.SetCount(100)
					.SetFrequency(4.0f)
					.SetDeleteTime(2.0f)
					.Build();
				particleSystem_->AddParticleEmitter(newParticleEmitter);
			}
		}
		else if (playerDirection_ == Direction::Left)
		{
			aabb_ = { {-0.6f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
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
					.SetColor({ 1.0f ,0.0f ,0.0f ,1.0f }, { 1.0f ,0.5f ,0.0f ,1.0f })
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

void Player::BehaviorJumpInitialize()
{
	worldTransform_.translation.y = 0.0f;

	const float kJumpFirstSpeed_ = 0.3f;

	velocity_.y = kJumpFirstSpeed_;
}

void Player::BehaviorJumpUpdate()
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

void Player::BehaviorThrowInitialize()
{
	
}

void Player::BehaviorThrowUpdate()
{

}

void Player::BehaviorStanInitialize()
{
	animationIndex_ = 9;
}

void Player::BehaviorStanUpdate()
{
	stanTimer_--;
	if (stanTimer_ > 55)
	{
		isShake_ = true;
	}

	animationIndex_ = 9;
	float animationTime = 0.0f;
	float animationDuration;
	animationTime = model_->GetAnimationTime();
	animationDuration = model_->GetAnimation()[animationIndex_].duration;

	if (playerDirection_ == Direction::Left)
	{
		aabb_ = { {-0.6f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
		SetAABB(aabb_);
	}
	else if (playerDirection_ == Direction::Right)
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
		stanTimer_ = 60;
		model_->SetAnimationTime(animationTime);
		aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
		SetAABB(aabb_);
	}
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
}

void Player::OnCollision(Collider* collider, float damage)
{
	if (collider->GetCollisionAttribute() & kCollisionAttributeEnemyBullet)
	{
		if (enemy_->GetIsShot() && !isDown_ && !isGuard_)
		{
			damage = 5.0f;
			hp_ += damage;
			isHitBullet_ = true;
		}

		if (enemy_->GetIsShot() && isGuard_ && playerDirection_ == Direction::Right)
		{
			guardAnimationTimer_--;

			audio_->SoundPlayMP3(guardSoundHandle_, false, 1.0f);
			worldTransform_.translation.x -= 0.1f;
			guardGauge_ -= 6.0f;

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
		else if (enemy_->GetIsShot() && isGuard_ && playerDirection_ == Direction::Left)
		{
			guardAnimationTimer_--;

			audio_->SoundPlayMP3(guardSoundHandle_, false, 1.0f);
			worldTransform_.translation.x += 0.1f;
			guardGauge_ -= 6.0f;

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
	}

	//敵との当たり判定
	if (collider->GetCollisionAttribute() & kCollisionAttributeEnemy)
	{
		isHit_ = true;

		if (enemy_->GetIsAttack() && !enemy_->GetIsTackle() && isGuard_ && playerDirection_ == Direction::Right)
		{
			guardAnimationTimer_--;

			audio_->SoundPlayMP3(guardSoundHandle_, false, 1.0f);
			worldTransform_.translation.x -= 0.3f;
			guardGauge_ -= 1.0f;

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

		if (enemy_->GetIsAttack() && !enemy_->GetIsTackle() && isGuard_ && playerDirection_ == Direction::Left)
		{
			guardAnimationTimer_--;

			audio_->SoundPlayMP3(guardSoundHandle_, false, 1.0f);
			worldTransform_.translation.x += 0.3f;
			guardGauge_ -= 1.0f;

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

		if (enemy_->GetIsAttack() && enemy_->GetIsTackle() && isGuard_ && playerDirection_ == Direction::Right)
		{
			guardAnimationTimer_--;

			audio_->SoundPlayMP3(guardSoundHandle_, false, 1.0f);
			worldTransform_.translation.x -= 0.2f;
			guardGauge_ -= 1.0f;

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

		if (enemy_->GetIsAttack() && enemy_->GetIsTackle() && isGuard_ && playerDirection_ == Direction::Left)
		{
			guardAnimationTimer_--;

			audio_->SoundPlayMP3(guardSoundHandle_, false, 1.0f);
			worldTransform_.translation.x += 0.2f;
			guardGauge_ -= 1.0f;

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

		//タックル
		//キャンセルじゃないとき
		if (enemy_->GetIsTackle() && enemy_->GetIsAttack() && !isDown_ && !isGuard_)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 15.0f;
			hp_ += damage;
			isHitTackle_ = true;

			HitStop(30);
		}
	}
}

void Player::HPBarUpdate()
{
	hpBar_.size_ = { (hp_ / maxHp_) * barSize_,7.0f };

	hpBar_.sprite_->SetSize(hpBar_.size_);

	if (hp_ < -50)
	{
		hpBar_.sprite_->SetColor({ 0.0f, 1.0f, 0.0f, 1.0f });
	}

	if (hp_ >= -50 && hp_ < -25)
	{
		hpBar_.sprite_->SetColor({ 1.0f, 0.8f, 0.0f, 1.0f });
	}
	else if (hp_ >= -25)
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

	guardGaugeBar_.size_ = { (guardGauge_ / maxGuardGauge_) * guardGaugeBarSize_,7.0f };

	guardGaugeBar_.sprite_->SetSize(guardGaugeBar_.size_);

	guardGaugeBar_.sprite_->SetColor({ 0.0f, 0.5f, 1.0f, 1.0f });

	if (guardGauge_ <= -50.0f)
	{
		guardGauge_ = -50.0f;
		isGuard_ = false;
		workAttack_.isAttack = false;
		behaviorRequest_ = Behavior::kStan;
	}
}

void Player::FinisherGaugeBarUpdate()
{
	finisherGaugeBar_.size_ = { (finisherGauge_ / maxFinisherGauge_) * finisherGaugeBarSize_,20.0f };

	finisherGaugeBar_.sprite_->SetSize(finisherGaugeBar_.size_);

	if (finisherGauge_ > -50.0f)
	{
		finisherGaugeBar_.sprite_->SetColor({ 0.0f, 0.5f, 1.0f, 1.0f });
	}
	else
	{
		finisherGaugeBar_.sprite_->SetColor({ 1.0f, 0.5f, 0.0f, 1.0f });
	}

	if (finisherGauge_ <= -50.0f)
	{
		finisherGauge_ = -50.0f;
	}
}

void Player::Reset()
{
	hp_ = -100.0f;

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

	//isThrow_ = false;

	finisherEffectTimer_ = 90;
	isFinisherEffect_ = false;
	finisherCount_ = 0;

	animationIndex_ = 4;

	attackAnimationFrame_ = 0;

	behavior_ = Behavior::kRoot;

	worldTransform_.translation = { -3.0f,0.0f,0.0f };
	playerDirection_ = Direction::Right;

	worldTransform_.UpdateMatrixEuler();

	isReset_ = false;
}

void Player::HitStop(int milliseconds)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void Player::DownAnimation()
{
	//タックル攻撃
	if (isHitTackle_ && playerDirection_ == Direction::Left)
	{
		isDown_ = true;
		downAnimationTimer_--;

		if (downAnimationTimer_ > 55)
		{
			isShake_ = true;

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

		animationIndex_ = 6;
		float animationTime = 0.0f;
		float animationDuration;
		animationTime = model_->GetAnimationTime();
		animationDuration = model_->GetAnimation()[animationIndex_].duration;

		animationTime += 1.0f / 30.0f;

		model_->SetAnimationTime(animationTime);
		model_->ApplyAnimation(animationIndex_);

		aabb_ = { {0.1f,-0.3f,-0.3f},{0.8f,0.0f,0.3f} };
		SetAABB(aabb_);

		if (!enemy_->GetIsTackle() && hp_ < 0.0f)
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

	if (isHitTackle_ && playerDirection_ == Direction::Right)
	{
		isDown_ = true;
		downAnimationTimer_--;

		if (downAnimationTimer_ > 55)
		{
			isShake_ = true;

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

		animationIndex_ = 6;
		float animationTime = 0.0f;
		float animationDuration;
		animationTime = model_->GetAnimationTime();
		animationDuration = model_->GetAnimation()[animationIndex_].duration;

		animationTime += 1.0f / 30.0f;

		model_->SetAnimationTime(animationTime);
		model_->ApplyAnimation(animationIndex_);

		aabb_ = { {-0.8f,-0.3f,-0.3f},{-0.1f,0.0f,0.3f} };
		SetAABB(aabb_);

		if (!enemy_->GetIsTackle() && hp_ < 0.0f)
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

	//弾攻撃
	if (isHitBullet_)
	{
		isDown_ = true;
		downAnimationTimer_--;

		if (downAnimationTimer_ > 55)
		{
			ParticleEmitter* newParticleEmitter = EmitterBuilder()
				.SetParticleType(ParticleEmitter::ParticleType::kNormal)
				.SetTranslation({ worldTransform_.translation.x + (playerDirection_ == Direction::Left ? -0.1f : 0.1f), worldTransform_.translation.y + 0.5f, worldTransform_.translation.z })
				.SetArea({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f })
				.SetRotation({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f })
				.SetScale({ 0.1f, 0.1f, 0.1f }, { 0.2f, 0.2f, 0.2f })
				.SetAzimuth(0.0f, 360.0f)
				.SetElevation(0.0f, 0.0f)
				.SetVelocity({ 0.03f, 0.03f, 0.03f }, { 0.06f, 0.06f, 0.06f })
				.SetColor({ 1.0f, 0.5f, 0.0f, 1.0f }, { 1.0f, 0.5f, 0.0f, 1.0f })
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

		if (animationTime >= animationDuration && hp_ < 0.0f)
		{
			animationIndex_ = 4;
			downAnimationTimer_ = 60;
			animationTime = 0.0f;
			model_->SetAnimationTime(animationTime);
			isHitBullet_ = false;
			isDown_ = false;
		}
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

void Player::PushEnemy(Vector3& enemyPosition, float pushSpeed)
{
	//敵の位置を押す速度分だけ更新
	enemyPosition.x += pushSpeed;

	//敵のワールドトランスフォームを更新
	enemy_->GetWorldTransform().translation = enemyPosition;
	enemy_->GetWorldTransform().UpdateMatrixEuler();
}