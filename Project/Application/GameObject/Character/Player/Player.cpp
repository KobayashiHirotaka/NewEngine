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

	ICharacter::Initialize();

	//Inputのinstance
	input_ = Input::GetInstance();

	//Audioのinstance
	audio_ = Audio::GetInstance();

	//WorldTransformの初期化
	worldTransform_.Initialize();

	//当たり判定の設定
	SetAABB(aabb_);

	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(kCollisionMaskPlayer);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);

	//リソース
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

	hitTextureHandle_ = TextureManager::LoadTexture("resource/images/Hit.png");
	hitSprite_.reset(Sprite::Create(hitTextureHandle_, { 1090.0f, 180.0f }));

	comboNumTextureHandle_ = TextureManager::LoadTexture("resource/number/0.png");
	comboNumSprite_.reset(Sprite::Create(comboNumTextureHandle_, { 1060.0f, 290.0f }));

	finisherGaugeBar_.sprite_ = Sprite::Create(finisherGaugeBar_.textureHandle_, finisherGaugeBar_.position_);

	//SEの初期化
	attackSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/Attack.mp3");
	weaponAttackSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/WeaponAttack.mp3");
	damageSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/HitPunch1.mp3");
	guardSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/Guard.mp3");

	//カーソルの初期化
	playerCursol_.reset(Model::CreateFromOBJ("resource/playerCursol", "playerCursol.obj"));

	worldTransformCursol_.Initialize();
	worldTransformCursol_.translation = { worldTransform_.translation.x, worldTransform_.translation.y + 0.4f, worldTransform_.translation.z };
	worldTransformCursol_.rotation.y = 1.5f;
	worldTransformCursol_.scale = { 0.3f, 0.3f, 0.3f };

	worldTransformCursol_.parent = &worldTransform_;

	//パーティクル
	particleEffectPlayer_ = std::make_unique<ParticleEffectPlayer>();
	particleEffectPlayer_->Initialize();

	//WorldTransformの更新
	worldTransform_.UpdateMatrixEuler();
}

void Player::Update()
{
	ICharacter::Update();

	//エディタで設定したパラメータをセット
	AttackEditor::GetInstance()->SetAttackParameters(attackType, attackData_.attackStartTime, attackData_.attackEndTime,
		attackData_.recoveryTime, attackData_.damage, true);

	//デバッグ用の処理
	if (isDebug_)
	{
		if (attackData_.isAttack)
		{
			//攻撃中(攻撃判定あり)にモデルの色を変える
			model_->GetMaterial()->SetColor({ 1.0f,0.0f,0.0f,1.0f });
		}
	    else if (attackData_.isRecovery)
	    {
		    //硬直中にモデルの色を変える
		    model_->GetMaterial()->SetColor({ 0.0f,0.0f,1.0f,1.0f });
	    }
		else
		{
			model_->GetMaterial()->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		}
	}

	model_->GetMaterial()->ImGui();

	//振り向きの処理
	Vector3 playerWorldPosition = GetWorldPosition();
	Vector3 enemyWorldPosition = enemy_->GetWorldPosition();

	if (enemyWorldPosition.x > playerWorldPosition.x && characterState_.behavior != Behavior::kJump
		&& characterState_.behavior != Behavior::kAttack && !characterState_.isDown)
	{
		characterState_.direction = Direction::Right;
		worldTransform_.rotation.y = 1.7f;
		isDirectionRight_ = true;
	}

	if (enemyWorldPosition.x < playerWorldPosition.x && characterState_.behavior != Behavior::kJump
		&& characterState_.behavior != Behavior::kAttack && !characterState_.isDown)
	{
		characterState_.direction = Direction::Left;
		worldTransform_.rotation.y = 4.6f;
		isDirectionRight_ = false;
	}

	HitCombo();

	ComboNumberSpriteUpdate();

	if (!enemy_->GetIsAttack())
	{
		timerData_.guardAnimationTimer = 60;
	}

	particleEffectPlayer_->Update();

	//worldTransformの更新
	worldTransform_.UpdateMatrixEuler();
	worldTransformCursol_.UpdateMatrixEuler();
}

void Player::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, camera, animationIndex_);

	if (!characterState_.isDown && timerData_.finisherTimer == 120)
	{
		playerCursol_->Draw(worldTransformCursol_, camera, 0);
	}
}

void Player::BoneDraw(const Camera& camera)
{
	model_->BoneDraw(worldTransform_, camera, animationIndex_);
}

void Player::SpriteDraw()
{
	if (hp_ <= 0)
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

void Player::ParticleDraw(const Camera& camera)
{
	particleEffectPlayer_->Draw(camera);
}

void Player::ImGui(const char* title)
{
	ImGui::Begin(title);
	ImGui::DragFloat3("WTFT", &worldTransform_.translation.x, -14.0f, 14.0f);
	ImGui::DragFloat3("WTFR", &worldTransform_.rotation.x, 0.0f, 150.0f);
	ImGui::DragFloat3("WTFS", &worldTransform_.scale.x, 0.0f, 300.0f);

	ImGui::SliderFloat3("WTFT", &worldTransform_.translation.x, -100.0f, 100.0f);
	ImGui::SliderFloat3("WTFR", &worldTransform_.rotation.x, 0.0f, 16.0f);

	ImGui::Text("isGuard %d", characterState_.isGuard);
	ImGui::Text("attackAnimationFrame %d", attackData_.attackAnimationFrame);
	ImGui::Text("isAttack %d", attackData_.isAttack);
	ImGui::Text("isRecovery %d",attackData_.isRecovery);
	ImGui::DragFloat("animationTime", &animationTime_, 0.0001f);

	ImGui::Text("finisherTimer %d", timerData_.finisherTimer);

	ImGui::Text("attackStartTime %d", attackData_.attackStartTime);
	ImGui::Text("swingTime %d", attackData_.attackEndTime);
	ImGui::Text("recoveryTime %d", attackData_.recoveryTime);

	ImGui::Text("downAnimationTimer %d", timerData_.downAnimationTimer);

	model_->GetLight()->ImGui("DirectionalLight");
	model_->GetPointLight()->ImGui("PointLight");
	model_->GetSpotLight()->ImGui("SpotLight");

	ImGui::SliderFloat3("WTFT", &worldTransformCursol_.translation.x, -100.0f, 100.0f);
	ImGui::SliderFloat3("WTFR", &worldTransformCursol_.rotation.x, 0.0f, 16.0f);
	ImGui::SliderFloat3("WTFS", &worldTransformCursol_.scale.x, 0.0f, 16.0f);
	ImGui::End();
}

void Player::BehaviorRootInitialize()
{
	animationIndex_ = 5;
}

void Player::BehaviorRootUpdate()
{
	//コントローラーの取得
	if (input_->GetJoystickState())
	{
		if (!characterState_.isDown && comboCount_ <= 1)
		{
			//移動
			Move();
		}

		//ジャンプ
		if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP) && worldTransform_.translation.y == 0.0f && !characterState_.isDown)
		{
			characterState_.behaviorRequest = Behavior::kJump;
		}

		//攻撃
		//弱攻撃
		if ((input_->IsPressButtonEnter(XINPUT_GAMEPAD_X) || input_->IsPressButtonEnter(XINPUT_GAMEPAD_Y) || (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B) &&
			input_->IsPressButton(XINPUT_GAMEPAD_RIGHT_SHOULDER))) && !characterState_.isDown)
		{
			attackType = "LightPunch";
			AttackStart(attackData_.isLightPunch);
		}

		////中攻撃
		//if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_Y) && !characterState_.isDown)
		//{
		//	attackType = "MiddlePunch";
		//	AttackStart(attackData_.isMiddlePunch);
		//}

		//強攻撃
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B) && !input_->IsPressButton(XINPUT_GAMEPAD_RIGHT_SHOULDER) && !characterState_.isDown)
		{
			attackType = "HighPunch";
			AttackStart(attackData_.isHighPunch);
		}
		
		//タックル攻撃
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) && !characterState_.isDown && (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT) || input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT)))
		{
			attackType = "Tackle";
			AttackStart(attackData_.isTackle);
		}

		//アッパー攻撃
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT)
			&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT) && !characterState_.isDown)
		{
			attackType = "Uppercut";
			AttackStart(attackData_.isUppercut);
		}

		//必殺技
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_LEFT_SHOULDER) && isFinisherCharge_ && !characterState_.isDown)
		{
			attackType = "Finisher";
			AttackStart(attackData_.isFinisher);
			finisherGauge_ = 0.0f;
			timerData_.finisherTimer = 120;
			attackData_.isFinisherFirstAttack = false;
		}
	}
}

void Player::BehaviorAttackInitialize()
{
	attackData_.attackAnimationFrame = 0;
}

void Player::BehaviorAttackUpdate()
{
	//弱攻撃
	if (attackData_.isLightPunch)
	{
		animationIndex_ = 13;
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
		if (input_->GetJoystickState())
		{
			if (!characterState_.isDown && attackData_.attackAnimationFrame >= 10 && attackData_.attackAnimationFrame < 20 && (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X) 
				|| input_->IsPressButtonEnter(XINPUT_GAMEPAD_Y) || input_->IsPressButtonEnter(XINPUT_GAMEPAD_B)) && input_->IsPressButton(XINPUT_GAMEPAD_RIGHT_SHOULDER) && 
				characterState_.isHitCharacter)
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
		}

		attackData_.attackAnimationFrame++;
	}

	//中攻撃
	if (attackData_.isMiddlePunch)
	{
		animationIndex_ = 8;
		characterState_.isGuard = false;

		if (!characterState_.isDown)
		{
			UpdateAnimationTime(animationTime_, false, 40.0f, animationIndex_, model_);
		}

		if (characterState_.direction == Direction::Right)
		{
			aabb_ = { {-0.3f,-0.3f,-0.3f},{0.6f,0.3f,0.3f} };
			SetAABB(aabb_);
		}
		else if (characterState_.direction == Direction::Left)
		{
			aabb_ = { {-0.6f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
			SetAABB(aabb_);
		}

		EvaluateAttackTiming();

		if (characterState_.isDown || attackData_.attackAnimationFrame > attackData_.recoveryTime)
		{
			AttackEnd(attackData_.isMiddlePunch);
			ResetCollision();
		}

		//キャンセルの処理(中TC)
		if (input_->GetJoystickState())
		{
			if (!characterState_.isDown && attackData_.attackAnimationFrame >= 8 && attackData_.attackAnimationFrame < 25
				&& input_->IsPressButtonEnter(XINPUT_GAMEPAD_Y) && input_->IsPressButton(XINPUT_GAMEPAD_RIGHT_SHOULDER)
				&& characterState_.isHitCharacter)
			{
				attackType = "TCMiddlePunch";
				attackData_.isAttack = false;
				attackData_.isMiddlePunch = false;
				attackData_.isTCMiddlePunch = true;
				animationTime_ = 0.0f;
				attackData_.attackAnimationFrame = 0;
				model_->SetAnimationTime(animationTime_);
				ResetCollision();
			}
		}

		attackData_.attackAnimationFrame++;
	}

	//TC用の攻撃(2発目)
	if (attackData_.isTCMiddlePunch)
	{
		animationIndex_ = 12;
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

		//キャンセルの処理
		if (input_->GetJoystickState())
		{
			//弱コンボ
			if (!characterState_.isDown && attackData_.attackAnimationFrame > 15 && attackData_.attackAnimationFrame < 30
				&& input_->IsPressButtonEnter(XINPUT_GAMEPAD_X) && input_->IsPressButton(XINPUT_GAMEPAD_RIGHT_SHOULDER)
				&& characterState_.isHitCharacter)
			{
				attackType = "TCHighPunch";
				attackData_.isAttack = false;
				attackData_.isTCMiddlePunch = false;
				attackData_.isTCHighPunch = true;
				animationTime_ = 0.0f;
				attackData_.attackAnimationFrame = 0;
				model_->SetAnimationTime(animationTime_);
				ResetCollision();
			}

			//中コンボ
			if (!characterState_.isDown && attackData_.attackAnimationFrame > 15 && attackData_.attackAnimationFrame < 30
				&& input_->IsPressButtonEnter(XINPUT_GAMEPAD_Y) && input_->IsPressButton(XINPUT_GAMEPAD_RIGHT_SHOULDER)
				&& characterState_.isHitCharacter)
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

			//強コンボ
			if (!characterState_.isDown && attackData_.attackAnimationFrame > 15 && attackData_.attackAnimationFrame < 30
				&& input_->IsPressButtonEnter(XINPUT_GAMEPAD_B) && input_->IsPressButton(XINPUT_GAMEPAD_RIGHT_SHOULDER)
				&& characterState_.isHitCharacter)
			{
				attackType = "Uppercut";
				attackData_.isAttack = false;
				attackData_.isTCMiddlePunch = false;
				attackData_.isUppercut = true;
				animationTime_ = 0.0f;
				attackData_.attackAnimationFrame = 0;
				model_->SetAnimationTime(animationTime_);
				ResetCollision();
			}
		}

		attackData_.attackAnimationFrame++;
	}

	//TC用の攻撃(3発目)
	if (attackData_.isTCHighPunch)
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
			AttackEnd(attackData_.isTCHighPunch);
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

		//キャンセルの処理(横A)
		if (input_->GetJoystickState())
		{
			//タックル攻撃
			if (!characterState_.isDown && attackData_.attackAnimationFrame > 15 && attackData_.attackAnimationFrame < 30
				&& (input_->IsPressButtonEnter(XINPUT_GAMEPAD_Y) && input_->IsPressButton(XINPUT_GAMEPAD_RIGHT_SHOULDER) || 
				input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) && (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT) || 
				input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT))))
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
		}

		attackData_.attackAnimationFrame++;
	}

	//タックル攻撃
	if (attackData_.isTackle)
	{
		animationIndex_ = 9;
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

				particleEffectPlayer_->PlayParticle("PlayerRightNackle", { worldTransform_.translation.x + particlePositionX,
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

				particleEffectPlayer_->PlayParticle("PlayerLeftNackle", { worldTransform_.translation.x - particlePositionX,
					worldTransform_.translation.y + 0.6f,worldTransform_.translation.z });
			}
		}

		if (characterState_.isDown || attackData_.attackAnimationFrame > attackData_.recoveryTime)
		{
			AttackEnd(attackData_.isTackle);
			ResetCollision();
		}

		attackData_.attackAnimationFrame++;
	}

	//アッパー攻撃
	if (attackData_.isUppercut)
	{
		animationIndex_ = 14;
		characterState_.isGuard = false;
		//Vector3 particlePosition = GetRightHandJointWorldPosition();

		if (!characterState_.isDown)
		{
			UpdateAnimationTime(animationTime_, false, 40.0f, animationIndex_, model_);
		}

		if (characterState_.direction == Direction::Right)
		{
			aabb_ = { {-0.3f,-0.3f,-0.3f},{0.6f,0.3f,0.3f} };
			SetAABB(aabb_);

			EvaluateAttackTiming();
		}
		else if (characterState_.direction == Direction::Left)
		{
			aabb_ = { {-0.6f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
			SetAABB(aabb_);

			EvaluateAttackTiming();
		}

		if (characterState_.isDown || attackData_.attackAnimationFrame > attackData_.recoveryTime)
		{
			AttackEnd(attackData_.isUppercut);
			ResetCollision();
		}

		//キャンセルの処理(横A)
		if (input_->GetJoystickState())
		{
			//強コンボ
			if (!characterState_.isDown && attackData_.attackAnimationFrame > 10 && attackData_.attackAnimationFrame < 40
				&& input_->IsPressButtonEnter(XINPUT_GAMEPAD_B) && input_->IsPressButton(XINPUT_GAMEPAD_RIGHT_SHOULDER)
				&& isFinisherCharge_)
			{
				attackData_.isAttack = false;
				attackData_.isUppercut = false;
				attackData_.isFinisher = true;
				animationTime_ = 0.0f;
				attackData_.attackAnimationFrame = 0;
				model_->SetAnimationTime(animationTime_);
				ResetCollision();
				finisherGauge_ = 0.0f;
			}
		}

		attackData_.attackAnimationFrame++;
	}

	//超必
	if (attackData_.isFinisher)
	{
		bool isFinisherEffect = false;
		characterState_.isGuard = false;

		if (timerData_.finisherTimer > 40 && attackData_.attackAnimationFrame < 80 && !attackData_.isFinisherFirstAttack && !attackData_.isFinisherSecondAttack)
		{
			isFinisherEffect = true;
			timerData_.finisherTimer--;
			UpdateAnimationTime(animationTime_, false, 40.0f, animationIndex_, model_);
		}
		else
		{
			isFinisherEffect = false;
		}

		if (isFinisherEffect)
		{
			if (isDirectionRight_)
			{
				animationIndex_ = 15;
			}
			else 
			{
				animationIndex_ = 18;
			}
		}
		else if(!attackData_.isFinisherFirstAttack && !attackData_.isFinisherSecondAttack)
		{
			attackType = "FinisherFirstAttack";
			timerData_.finisherTimer = 120;
			attackData_.isFinisherFirstAttack = true;
			animationTime_ = 0.0f;
			attackData_.attackAnimationFrame = 0;
			model_->SetAnimationTime(animationTime_);
		}

		if (attackData_.isFinisherFirstAttack && !characterState_.isDown)
		{
			animationIndex_ = 16;
			UpdateAnimationTime(animationTime_, false, 40.0f, animationIndex_, model_);

			if (characterState_.direction == Direction::Right)
			{
				aabb_ = { {-0.3f,-0.3f,-0.3f},{0.4f,0.3f,0.3f} };
				SetAABB(aabb_);
			}
			else if (characterState_.direction == Direction::Left)
			{
				aabb_ = { {-0.4f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
				SetAABB(aabb_);
			}

			EvaluateAttackTiming();

			if (attackData_.isAttack)
			{
				isFinisherInvincible_ = true;
			}
			else
			{
				isFinisherInvincible_ = false;
			}

			if (enemy_->GetIsDown() && attackData_.attackAnimationFrame > 10 && attackData_.attackAnimationFrame < 30)
			{
				attackType = "FinisherSecondAttack";
				timerData_.finisherTimer = 120;
				attackData_.isAttack = false;
				attackData_.isFinisherFirstAttack = false;
				isFinisherInvincible_ = false;
				attackData_.isFinisherSecondAttack = true;
				animationTime_ = 0.0f;
				attackData_.attackAnimationFrame = 0;
				model_->SetAnimationTime(animationTime_);
				ResetCollision();
			}

			if ((characterState_.isDown || attackData_.attackAnimationFrame > attackData_.recoveryTime) && !enemy_->GetIsDown())
			{
				timerData_.finisherTimer = 120;
				AttackEnd(attackData_.isFinisher);
				ResetCollision();
				attackData_.isFinisherFirstAttack = false;
				isFinisherInvincible_ = false;
			}
		}

		if (attackData_.isFinisherSecondAttack && !characterState_.isDown)
		{
			animationIndex_ = 17;
			UpdateAnimationTime(animationTime_, false, 30.0f, animationIndex_, model_);

			if (characterState_.direction == Direction::Right)
			{
				aabb_ = { {-0.3f,-0.3f,-0.3f},{0.6f,0.3f,0.3f} };
				SetAABB(aabb_);

				if (!characterState_.isHitCharacter)
				{
					worldTransform_.translation.x += 0.01f;
				}
			}
			else if (characterState_.direction == Direction::Left)
			{
				aabb_ = { {-0.6f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
				SetAABB(aabb_);

				if (!characterState_.isHitCharacter)
				{
					worldTransform_.translation.x -= 0.01f;
				}
			}

			EvaluateAttackTiming();

			if (attackData_.attackAnimationFrame > attackData_.recoveryTime)
			{
				attackType = "Tackle";
				attackData_.isAttack = false;
				attackData_.isFinisher = false;
				attackData_.isFinisherSecondAttack = false;
				attackData_.isTackle = true;
				animationTime_ = 0.0f;
				attackData_.attackAnimationFrame = 0;
				model_->SetAnimationTime(animationTime_);
				ResetCollision();
			}
		}

		if (characterState_.isDown)
		{
			timerData_.finisherTimer = 120;
			AttackEnd(attackData_.isFinisher);
			ResetCollision();
			attackData_.isFinisherFirstAttack = false;
			attackData_.isFinisherSecondAttack = false;
			isFinisherInvincible_ = false;
		}

		attackData_.attackAnimationFrame++;
	}
}

void Player::BehaviorJumpInitialize()
{
	const float kJumpFirstSpeed_ = 0.3f;

	moveData_.velocity.y = kJumpFirstSpeed_;
}

void Player::BehaviorJumpUpdate()
{
	worldTransform_.translation = Add(worldTransform_.translation, moveData_.velocity);

	const float kGravityAcceleration_ = 0.02f;

	Vector3 accelerationVector_ = { 0.0f,-kGravityAcceleration_,0.0f };

	moveData_.velocity = Add(moveData_.velocity, accelerationVector_);

	if (input_->GetJoystickState())
	{
		if (worldTransform_.translation.y > 0.3f && !attackData_.isJumpAttack && (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X) || input_->IsPressButtonEnter(XINPUT_GAMEPAD_Y)
			|| input_->IsPressButtonEnter(XINPUT_GAMEPAD_B) || input_->IsPressButtonEnter(XINPUT_GAMEPAD_A)))
		{
			attackType = "JumpAttack";
			attackData_.isAttack = false;
			attackData_.isJumpAttack = true;
			animationTime_ = 0.0f;
			attackData_.attackAnimationFrame = 0;
			model_->SetAnimationTime(animationTime_);
			ResetCollision();
		}
	}

	if (attackData_.isJumpAttack)
	{
		animationIndex_ = 6;
		characterState_.isGuard = false;

		if (!characterState_.isDown)
		{
			UpdateAnimationTime(animationTime_, true, 40.0f, animationIndex_, model_);
		}

		if (characterState_.direction == Direction::Right)
		{
			aabb_ = { {0.0f,0.0f,0.0f},{0.5f,0.5f,0.5f} };
			SetAABB(aabb_);
		}
		else if (characterState_.direction == Direction::Left)
		{
			aabb_ = { {-0.5f,-0.5f,-0.5f},{0.0f,0.0f,0.0f} };
			SetAABB(aabb_);
		}

		EvaluateAttackTiming();

		if (worldTransform_.translation.y <= 0.0f && attackData_.attackAnimationFrame > attackData_.recoveryTime)
		{
			AttackEnd(attackData_.isJumpAttack);
			ResetCollision();
		}

		if (worldTransform_.translation.y <= 0.0f || characterState_.isDown)
		{
			worldTransform_.translation.y = 0.0f;
			moveData_.velocity = { 0.0f,0.0f,0.0f };
			AttackEnd(attackData_.isJumpAttack);
			ResetCollision();
		}


		//キャンセルの処理(弱パンチ)
		if (input_->GetJoystickState())
		{
			if (!characterState_.isDown && attackData_.attackAnimationFrame >= 10 && attackData_.attackAnimationFrame < 20
				&& input_->IsPressButtonEnter(XINPUT_GAMEPAD_X) && input_->IsPressButton(XINPUT_GAMEPAD_RIGHT_SHOULDER)
				&& characterState_.isHitCharacter)
			{
				characterState_.behaviorRequest = Behavior::kAttack;
				attackType = "LightPunch";
				attackData_.isAttack = false;
				attackData_.isJumpAttack = false;
				attackData_.isLightPunch = true;
				animationTime_ = 0.0f;
				attackData_.attackAnimationFrame = 0;
				model_->SetAnimationTime(animationTime_);
				ResetCollision();
			}
		}

		attackData_.attackAnimationFrame++;
	}
	else
	{
		animationIndex_ = 5;

		UpdateAnimationTime(animationTime_, true, 60.0f, animationIndex_, model_);

		if (worldTransform_.translation.y <= 0.0f || characterState_.isDown)
		{
			characterState_.behaviorRequest = Behavior::kRoot;
			worldTransform_.translation.y = 0.0f;
			animationTime_ = 0.0f;
			model_->SetAnimationTime(animationTime_);
		}
	}
}

void Player::BehaviorStanInitialize()
{
	animationIndex_ = 10;
}

void Player::BehaviorStanUpdate()
{
	timerData_.stanTimer--;
	if (timerData_.stanTimer > 55)
	{
		effectState_.isShake = true;
	}

	animationIndex_ = 10;
	float animationDuration;
	animationTime_ = model_->GetAnimationTime();
	animationDuration = model_->GetAnimation()[animationIndex_].duration;

	if (characterState_.direction == Direction::Left)
	{
		aabb_ = { {-0.6f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
		SetAABB(aabb_);
	}
	else if (characterState_.direction == Direction::Right)
	{
		aabb_ = { {-0.3f,-0.3f,-0.3f},{0.6f,0.3f,0.3f} };
		SetAABB(aabb_);
	}

	if (!characterState_.isDown)
	{
		animationTime_ += 1.0f / 60.0f;
	}

	model_->SetAnimationTime(animationTime_);
	model_->ApplyAnimation(animationIndex_);

	if (animationTime_ >= animationDuration || characterState_.isDown)
	{
		characterState_.behaviorRequest = Behavior::kRoot;
		animationTime_ = 0.0f;
		attackData_.attackAnimationFrame = 0;
		guardGauge_ = 0.0f;
		timerData_.stanTimer = 60;
		model_->SetAnimationTime(animationTime_);
		
	}
}

void Player::OnCollision(Collider* collider)
{
	if (collider->GetCollisionAttribute() & kCollisionAttributeEnemyBullet)
	{
		if (!characterState_.isDown && !characterState_.isGuard && worldTransform_.translation.y <= 0.0f)
		{
			animationTime_ = 0.0f;
			model_->SetAnimationTime(animationTime_);

			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			ApplyDamage();
			characterState_.isHitBullet = true;
			attackData_.isFinisher = false;

			AdjustFinisherGauge(1.0f);

			HitStop(5);
		}

		if (!characterState_.isDown && !characterState_.isGuard && worldTransform_.translation.y > 0.0f)
		{
			animationTime_ = 0.0f;
			model_->SetAnimationTime(animationTime_);

			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			ApplyDamage();
			characterState_.isHitAirBullet = true;

			AdjustFinisherGauge(1.0f);

			HitStop(5);
		}

		if (characterState_.isGuard && characterState_.direction == Direction::Right)
		{
			timerData_.guardAnimationTimer--;

			audio_->SoundPlayMP3(guardSoundHandle_, false, 1.0f);
			worldTransform_.translation.x -= 0.1f;
			guardGauge_ -= 6.0f;

			if (timerData_.guardAnimationTimer > 55)
			{

				particleEffectPlayer_->PlayParticle("Guard", { worldTransform_.translation.x + 0.1f,
					worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });
			}

			animationIndex_ = 2;
			UpdateAnimationTime(animationTime_, false, 40.0f, animationIndex_, model_);
		}
		else if (characterState_.isGuard && characterState_.direction == Direction::Left)
		{
			timerData_.guardAnimationTimer--;

			audio_->SoundPlayMP3(guardSoundHandle_, false, 1.0f);
			worldTransform_.translation.x += 0.1f;
			guardGauge_ -= 6.0f;

			if (timerData_.guardAnimationTimer > 55)
			{

				particleEffectPlayer_->PlayParticle("Guard", { worldTransform_.translation.x - 0.1f,
					worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });
			}

			animationIndex_ = 2;
			UpdateAnimationTime(animationTime_, false, 40.0f, animationIndex_, model_);
		}
	}

	//敵との当たり判定
	if (collider->GetCollisionAttribute() & kCollisionAttributeEnemy)
	{
		characterState_.isHitCharacter = true;

		if (enemy_->GetIsAttack() && !enemy_->GetIsTackle() && characterState_.isGuard && characterState_.direction == Direction::Right)
		{
			timerData_.guardAnimationTimer--;

			float enemyPosition = enemy_->GetWorldPosition().x;

			audio_->SoundPlayMP3(guardSoundHandle_, false, 1.0f);
			worldTransform_.translation.x -= 0.3f;
			enemyPosition += 0.2f;
			guardGauge_ -= 1.0f;

			enemy_->SetPositionX(enemyPosition);

			if (timerData_.guardAnimationTimer > 55)
			{

				particleEffectPlayer_->PlayParticle("Guard", { worldTransform_.translation.x + 0.1f,
					worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });
			}

			animationIndex_ = 2;
			UpdateAnimationTime(animationTime_, false, 40.0f, animationIndex_, model_);
		}

		if (enemy_->GetIsAttack() && !enemy_->GetIsTackle() && characterState_.isGuard && characterState_.direction == Direction::Left)
		{
			timerData_.guardAnimationTimer--;

			float enemyPosition = enemy_->GetWorldPosition().x;

			audio_->SoundPlayMP3(guardSoundHandle_, false, 1.0f);
			worldTransform_.translation.x += 0.3f;
			enemyPosition -= 0.2f;
			guardGauge_ -= 1.0f;

			enemy_->SetPositionX(enemyPosition);

			if (timerData_.guardAnimationTimer > 55)
			{

				particleEffectPlayer_->PlayParticle("Guard", { worldTransform_.translation.x - 0.1f,
					worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });
			}

			animationIndex_ = 2;
			UpdateAnimationTime(animationTime_, false, 40.0f, animationIndex_, model_);
		}

		if (enemy_->GetIsAttack() && enemy_->GetIsTackle() && characterState_.isGuard && characterState_.direction == Direction::Right)
		{
			timerData_.guardAnimationTimer--;

			audio_->SoundPlayMP3(guardSoundHandle_, false, 1.0f);
			worldTransform_.translation.x -= 0.2f;
			guardGauge_ -= 1.0f;

			if (timerData_.guardAnimationTimer > 55)
			{
				particleEffectPlayer_->PlayParticle("Guard", { worldTransform_.translation.x + 0.1f,
					worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });
			}

			animationIndex_ = 2;
			UpdateAnimationTime(animationTime_, false, 40.0f, animationIndex_, model_);
		}

		if (enemy_->GetIsAttack() && enemy_->GetIsTackle() && characterState_.isGuard && characterState_.direction == Direction::Left)
		{
			timerData_.guardAnimationTimer--;

			audio_->SoundPlayMP3(guardSoundHandle_, false, 1.0f);
			worldTransform_.translation.x += 0.2f;
			guardGauge_ -= 1.0f;

			if (timerData_.guardAnimationTimer > 55)
			{
				particleEffectPlayer_->PlayParticle("Guard", { worldTransform_.translation.x - 0.1f,
					worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });
			}

			animationIndex_ = 2;
			UpdateAnimationTime(animationTime_, false, 40.0f, animationIndex_, model_);
		}

		if (!isFinisherInvincible_)
		{
			//弱パンチ
			if (enemy_->GetIsAttack() && enemy_->GetIsLightPunch() && !characterState_.isDown && !characterState_.isGuard)
			{
				audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
				ApplyDamage();
				characterState_.isHitLightPunch = true;

				AdjustFinisherGauge(1.0f);

				HitStop(10);
			}

			//強パンチ
			if (enemy_->GetIsHighPunch() && !characterState_.isDown && !characterState_.isGuard)
			{
				audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
				ApplyDamage();
				characterState_.isHitHighPunch = true;

				AdjustFinisherGauge(2.0f);

				HitStop(10);
			}

			//TC中パンチ
			if (enemy_->GetIsTCMiddlePunch() && !characterState_.isDown && !characterState_.isGuard)
			{
				audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
				ApplyDamage();
				characterState_.isHitTCMiddlePunch = true;

				AdjustFinisherGauge(2.0f);

				HitStop(10);
			}

			//タックル
			if (enemy_->GetIsTackle() && enemy_->GetIsAttack() && !characterState_.isGuard)
			{
				if (!characterState_.isDown)
				{
					//キャンセルじゃないとき
					audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
					ApplyDamage();
					characterState_.isHitTackle = true;

					AdjustFinisherGauge(4.0f);

					HitStop(30);
				}
				else if (characterState_.isDown && worldTransform_.translation.y > 0.5f)
				{
					//キャンセルのとき
					attackData_.isDamaged = false;
					audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
					ApplyDamage();
					timerData_.downAnimationTimer = 60;
					float animationTime = 0.0f;
					model_->SetAnimationTime(animationTime);
					characterState_.isHitHighPunch = false;
					characterState_.isHitTackle = true;

					AdjustFinisherGauge(3.0f);

					HitStop(10);
				}
			}
		}
	}
}

void Player::UpdateAnimationTime(float animationTime, bool isLoop, float frameRate, 
	int animationIndex, std::unique_ptr<Model>& modelFighterBody)
{
	ICharacter::UpdateAnimationTime(animationTime, isLoop, frameRate, animationIndex, modelFighterBody);
}

void Player::Move()
{
	//コントローラーの取得
	if (input_->GetJoystickState())
	{
		const float deadZone = 0.7f;
		bool isFrontMove_ = false;
		bool isBackMove_ = false;

		moveData_.velocity = { 0.0f, 0.0f, 0.0f };

		//敵の位置を取得する（例: enemyPosition という変数）
		Vector3 enemyPosition = enemy_->GetWorldPosition();

		if (characterState_.isHitCharacter)
		{
			if (characterState_.direction == Direction::Right && input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT))
			{
				//敵を右方向に押す
				PushEnemy(enemyPosition, 0.05f);
			}
			else if (characterState_.direction == Direction::Left && input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT))
			{
				//敵を左方向に押す
				PushEnemy(enemyPosition, -0.05f);
			}
		}

		//移動処理
		//前方向に移動(左を向いているとき)
		if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT) && characterState_.direction == Direction::Left && !characterState_.isDown)
		{
			moveData_.velocity.x = -0.01f;
			isFrontMove_ = true;
			characterState_.isGuard = false;
		}

		//前方向に移動(右を向いているとき)
		if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT) && characterState_.direction == Direction::Right && !characterState_.isDown)
		{
			moveData_.velocity.x = 0.01f;
			isFrontMove_ = true;
			characterState_.isGuard = false;
		}

		//後ろ方向に移動(右を向いているとき)
		if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT) && characterState_.direction == Direction::Right && !characterState_.isDown)
		{
			characterState_.isGuard = true;

			//移動しながらガード
			if (!input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN))
			{
				moveData_.velocity.x = -0.01f;
				isBackMove_ = true;
			}

			//止まってガード
			if (characterState_.isGuard && input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP))
			{
				animationIndex_ = 2;
				UpdateAnimationTime(animationTime_, false, 40.0f, animationIndex_, model_);
				moveData_.velocity.x = 0.0f;
				isBackMove_ = false;
			}
		}

		//後ろ方向に移動(左を向いているとき)
		if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT) && characterState_.direction == Direction::Left && !characterState_.isDown)
		{
			characterState_.isGuard = true;

			//移動しながらガード
			if (!input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN))
			{
				moveData_.velocity.x = 0.01f;
				isBackMove_ = true;
			}

			//止まってガード
			if (characterState_.isGuard && input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP))
			{
				animationIndex_ = 2;
				UpdateAnimationTime(animationTime_, false, 40.0f, animationIndex_, model_);
				moveData_.velocity.x = 0.0f;
				isBackMove_ = false;
			}
		}

		//移動していない時
		if (!input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT))
		{
			characterState_.isGuard = false;
		}

		//移動
		if (isFrontMove_)
		{
			animationIndex_ = 1;
			UpdateAnimationTime(animationTime_, true, 30.0f, animationIndex_, model_);

			moveData_.velocity = Normalize(moveData_.velocity);
			moveData_.velocity = Multiply(frontSpeed_, moveData_.velocity);

			worldTransform_.translation = Add(worldTransform_.translation, moveData_.velocity);

			worldTransform_.UpdateMatrixEuler();
		}
		else if (isBackMove_)
		{
			animationIndex_ = 0;
			UpdateAnimationTime(animationTime_, true, 30.0f, animationIndex_, model_);

			moveData_.velocity = Normalize(moveData_.velocity);
			moveData_.velocity = Multiply(backSpeed_, moveData_.velocity);

			worldTransform_.translation = Add(worldTransform_.translation, moveData_.velocity);

			worldTransform_.UpdateMatrixEuler();
		}
		else if(timerData_.guardAnimationTimer == 60 && !characterState_.isGuard)
		{
			animationIndex_ = 5;
			UpdateAnimationTime(animationTime_, true, 60.0f, animationIndex_, model_);
		}
	}
}

void Player::AttackStart(bool& isAttackType)
{
	ICharacter::AttackStart(isAttackType);
}

void Player::AttackEnd(bool& isAttackType)
{
	ICharacter::AttackEnd(isAttackType);
}

void Player::EvaluateAttackTiming()
{
	ICharacter::EvaluateAttackTiming();
}

void Player::ApplyDamage()
{
	if (!attackData_.isDamaged)
	{
		attackData_.isDamaged = true;
		hp_ += enemy_->GetDamage();
	}
}

void Player::ResetCollision()
{
	aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };
	SetAABB(aabb_);
}

void Player::ConfigureCollision(Vector3 min, Vector3 max)
{
	aabb_ = { {min.x, min.y, min.z},{max.x, max.y, max.z} };
	SetAABB(aabb_);
}

void Player::HPBarUpdate()
{
	hpBar_.size_ = { (static_cast<float>(hp_) / static_cast<float>(maxHp_)) * barSize_, 7.0f };

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
		characterState_.isGuard = false;
		attackData_.isAttack = false;
		characterState_.behaviorRequest = Behavior::kStan;
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
		isFinisherCharge_ = true;
	}
	else
	{
		isFinisherCharge_ = false;
	}
}

void Player::AdjustFinisherGauge(float value)
{
	if (finisherGauge_ > -50.0f)
	{
		finisherGauge_ -= value;

		if (finisherGauge_ < -50.0f)
		{
			finisherGauge_ = -50.0f;
		}
	}

	float finisherGaugeEnemy = enemy_->GetFinisherGauge();

	if (finisherGaugeEnemy < 50.0f)
	{
		finisherGaugeEnemy += value;

		if (finisherGaugeEnemy > 50.0f)
		{
			finisherGaugeEnemy = 50.0f;
		}
	}

	enemy_->SetFinisherGauge(finisherGaugeEnemy);
}

void Player::Reset()
{
	ICharacter::Reset();

	hp_ = -100;

	animationIndex_ = 5;
	animationTime_ = 0.0f;
	model_->SetAnimationTime(animationTime_);

	worldTransform_.translation = { -3.0f,0.0f,0.0f };
	worldTransform_.rotation = { 0.0f,1.7f,0.0f };
	characterState_.direction = Direction::Right;

	worldTransform_.UpdateMatrixEuler();
}

void Player::HitStop(int milliseconds)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void Player::DownAnimation()
{
	//弱攻撃
	if (characterState_.isHitLightPunch)
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
		UpdateAnimationTime(animationTime_, false, 30.0f, animationIndex_, model_);

		//次の入力を受け取ったらこの処理にする
		if (!enemy_->GetIsLightPunch() && hp_ < 0)
		{
			DownAnimationEnd(5, characterState_.isHitLightPunch);
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

			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x + particlePosX,
				 worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });
		}

		animationIndex_ = 4;
		UpdateAnimationTime(animationTime_, false, 30.0f, animationIndex_, model_);

		if (!enemy_->GetIsTCMiddlePunch() && hp_ < 0)
		{
			DownAnimationEnd(5, characterState_.isHitTCMiddlePunch);
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

			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x + particlePosX,
				 worldTransform_.translation.y + 0.5f,worldTransform_.translation.z });

			const float kJumpFirstSpeed_ = 0.15f;

			moveData_.velocity.x = (characterState_.direction == Direction::Right) ? -0.025f : 0.025f;
			moveData_.velocity.y = kJumpFirstSpeed_;
		}
		else if (timerData_.downAnimationTimer > -30)
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

		aabb_ = (characterState_.direction == Direction::Right) ? AABB{ {-0.8f, -0.1f, -0.3f}, {-0.1f, 0.1f, 0.3f} } :
			AABB{ {0.1f, -0.1f, -0.3f}, {0.8f, 0.1f, 0.3f} };

		animationIndex_ = 7;
		UpdateAnimationTime(animationTime_, false, 30.0f, animationIndex_, model_);

		if (!enemy_->GetIsHighPunch() &&  worldTransform_.translation.y <= 0.0f && hp_ < 0)
		{
			DownAnimationEnd(5, characterState_.isHitHighPunch);
			ResetCollision();
		}
	}

	//タックル攻撃
	if (characterState_.isHitTackle)
	{
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;

		float particlePosX = (characterState_.direction == Direction::Right) ? 0.1f : -0.1f;
		float moveX = (characterState_.direction == Direction::Right) ? -0.08f : 0.08f;

		aabb_ = (characterState_.direction == Direction::Right) ? AABB{ {-0.8f, -0.0f, -0.3f}, {-0.1f, 0.1f, 0.3f} } :
			AABB{ {0.1f, -0.0f, -0.3f}, {0.8f, 0.1f, 0.3f} };

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

		animationIndex_ = 7;
		UpdateAnimationTime(animationTime_, false, 30.0f, animationIndex_, model_);

		SetAABB(aabb_);

		if (timerData_.downAnimationTimer < 0 && hp_ < 0)
		{
			DownAnimationEnd(4, characterState_.isHitTackle);
			ResetCollision();
		}
	}

	//弾攻撃
	if (characterState_.isHitBullet)
	{
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;

		if (!isParticle_)
		{
			effectState_.isShake = true;

			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x + (characterState_.direction == Direction::Left ? -0.1f : 0.1f),
				worldTransform_.translation.y + 0.5f, worldTransform_.translation.z });

			isParticle_ = true;
		}

		animationIndex_ = 4;
		UpdateAnimationTime(animationTime_, false, 30.0f, animationIndex_, model_);

		if (timerData_.downAnimationTimer < 30 && hp_ < 0)
		{
			DownAnimationEnd(4, characterState_.isHitBullet);
			ResetCollision();

			isParticle_ = false;
		}
	}
	else if (characterState_.isHitAirBullet)
	{
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;

		if (!isParticle_) 
		{
			effectState_.isShake = true;

			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x + (characterState_.direction == Direction::Left ? -0.1f : 0.1f),
				worldTransform_.translation.y + 0.5f, worldTransform_.translation.z });

			isParticle_ = true;
		}

		if (timerData_.downAnimationTimer > 35 && worldTransform_.translation.x > -4.0f)
		{
			worldTransform_.translation.x -= characterState_.direction == Direction::Left ? -0.08f : 0.08f;
		}

		if (worldTransform_.translation.y > 0.0f)
		{
			worldTransform_.translation.y -= 0.03f;
		}
		else if (worldTransform_.translation.y <= 0.0f)
		{
			worldTransform_.translation.y = 0.0f;
		}

		animationIndex_ = 7;
		UpdateAnimationTime(animationTime_, false, 30.0f, animationIndex_, model_);

		aabb_ = { {-0.8f,-0.3f,-0.3f},{-0.1f,0.0f,0.3f} };
		SetAABB(aabb_);

		if (timerData_.downAnimationTimer < 30 && hp_ < 0)
		{
			DownAnimationEnd(4, characterState_.isHitAirBullet);
			ResetCollision();

			isParticle_ = false;
		}
	}
}

void Player::DownAnimationEnd(int animationIndex, bool& isHitAttackType)
{
	ICharacter::DownAnimationEnd(animationIndex, isHitAttackType);
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

void Player::ComboNumberSpriteUpdate()
{
	int comboNum = comboCount_;

	comboNumTextureHandle_ = TextureManager::LoadTexture("resource/number/" + std::to_string(comboNum) + ".png");

	comboNumSprite_->SetTexture(comboNumTextureHandle_);
}

void Player::HitCombo()
{
	//コンボを食らっているとき
	if (characterState_.isHitJumpAttack && comboCount_ == 0)
	{
		firstAttack_ = "JumpAttack";
		comboCount_ = 1;
		timerData_.comboTimer = 60;
		timerData_.comboTimer--;
	}

	if (characterState_.isHitLightPunch && comboCount_ == 0)
	{
		firstAttack_ = "LightPunch";
		comboCount_ = 1;
		timerData_.comboTimer = 60;
		timerData_.comboTimer--;
	}

	if (firstAttack_ == "JumpAttack")
	{
		if (characterState_.isHitLightPunch && comboCount_ == 1)
		{
			comboCount_ = 2;
			timerData_.comboTimer = 60;
			timerData_.comboTimer--;
		}

		if (characterState_.isHitTCMiddlePunch && comboCount_ == 2)
		{
			comboCount_ = 3;
			timerData_.comboTimer = 60;
			timerData_.comboTimer--;
		}

		if (characterState_.isHitTCHighPunch && comboCount_ == 3)
		{
			comboCount_ = 4;
			timerData_.comboTimer = 60;
			timerData_.comboTimer--;
		}

		if (characterState_.isHitHighPunch && comboCount_ == 3)
		{
			comboCount_ = 4;
			timerData_.comboTimer = 120;
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
			timerData_.comboTimer = 60;
			timerData_.comboTimer--;
		}

		if (characterState_.isHitTCHighPunch && comboCount_ == 2)
		{
			comboCount_ = 3;
			timerData_.comboTimer = 60;
			timerData_.comboTimer--;
		}

		if (characterState_.isHitHighPunch && comboCount_ == 2)
		{
			comboCount_ = 3;
			timerData_.comboTimer = 120;
			timerData_.comboTimer--;
		}

		if (characterState_.isHitTackle && comboCount_ == 3)
		{
			comboCount_ = 4;
			timerData_.comboTimer = 60;
			timerData_.comboTimer--;
		}
	}

	if (timerData_.comboTimer >= 0)
	{
		timerData_.comboTimer--;
	}

	if (timerData_.comboTimer < 0)
	{
		timerData_.comboTimer = 0;
		comboCount_ = 0;
		firstAttack_ = "";
	}
}

Vector3 Player::GetRightHandJointWorldPosition()
{
	WorldTransform handJointWorldTransform = model_->GetJointWorldTransform("mixamorig:RightHandIndex1");

	handJointWorldTransform.matWorld = Multiply(handJointWorldTransform.matWorld, worldTransform_.matWorld);

	return Vector3 { handJointWorldTransform.matWorld.m[3][0], handJointWorldTransform.matWorld.m[3][1], handJointWorldTransform.matWorld.m[3][2] };
}
