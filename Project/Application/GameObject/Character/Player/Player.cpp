#include "Player.h"

Player::~Player()
{
	
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

	//particleModelの初期化
	particleModel_.reset(ParticleModel::CreateFromOBJ("resource/Particle", "Particle.obj"));
	particleSystem_ = std::make_unique<ParticleSystem>();
	particleSystem_->Initialize();
}

void Player::Update()
{
	//0は停止、1は攻撃(振り下ろし),2は歩き
	model_->ApplyAnimation(animationIndex);

	model_->Update();

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
}

void Player::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, camera, animationIndex);
}

void Player::BoneDraw(const Camera& camera)
{
	model_->BoneDraw(worldTransform_, camera, animationIndex);
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
