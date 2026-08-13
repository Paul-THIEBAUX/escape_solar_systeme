#include "GCEnemy.h"

#include <limits>

#include "Core/InputManager.h"
#include "Engine/GameManager.h"
#include "Engine/SceneManager.h"
#include "Core/Timer.h"

#include "Tuto-GCLE/Projectile.h"
#include "Tag.h"
#include "Tuto-GCLE/InteractableHeart.h"

namespace Demo
{
	void GCEnemy::OnUpdate()
	{
		Character::OnUpdate();

		if (m_State == EnemyState::Dead)
			return;

		if (m_State == EnemyState::Spawning)
			return;

		float32 dt = static_cast<float32>(GameManager::GetInstance().GetTime()->GetDeltaTime());

		UpdateState(dt);
		 
		if (IsFlinching())
			return;

		switch (m_State)
		{
		case EnemyState::Idle:
			UpdateIdle(dt);
			break;
		case EnemyState::Chase:
			UpdateChase(dt);
			break;
		case EnemyState::Attack:
			UpdateAttack(dt);
			break;
		default:
			break;
		}

		if (mp_Target != nullptr && mp_Target->GetCurrentAnimation() == "Death" && mp_Target->IsAnimationAtEnd())
		{
			mp_Target = nullptr;
		}
	}

	void GCEnemy::UpdateState(float32 dt)
	{
		if (mp_Target == nullptr)
		{
			m_State = EnemyState::Idle;
			return;
		}

		float32 distance = DistanceToTarget();

		if (distance <= m_AttackRange)
			m_State = EnemyState::Attack;
		else if (distance <= m_DetectionRange)
			m_State = EnemyState::Chase;
		else
			m_State = EnemyState::Idle;
	}

	void GCEnemy::UpdateIdle(float32 dt)
	{
		PlayAnimation("Idle", AnimationMode::Loop | AnimationMode::IgnoreIfAlreadyPlaying);
	}

	void GCEnemy::UpdateChase(float32 dt)
	{
		Vector2f direction = mp_Target->GetPosition() - GetPosition();
		direction = direction.Normalized();

		if (direction.x < 0)
			SetTextureFlip(TextureFlipMode::FLIP_HORIZONTAL);
		else
			SetTextureFlip(TextureFlipMode::FLIP_NONE);

		GetRigidBody()->AddForce(direction, m_MoveForce, dt);

		PlayAnimation("Walk", AnimationMode::Loop | AnimationMode::IgnoreIfAlreadyPlaying);
	}

	void GCEnemy::UpdateAttack(float32 dt)
	{
		PlayAnimation("Idle", AnimationMode::Loop | AnimationMode::IgnoreIfAlreadyPlaying);

		if (m_IsTelegraphing)
		{
			m_TelegraphTimer -= dt;

			if (m_TelegraphTimer <= 0.0f)
			{
				m_IsTelegraphing = false;
				SetColor(Color::White);

				if (mp_Target != nullptr)
					ShootAt(mp_Target->GetPosition());

				m_ShootTimer = m_ShootCooldown;
			}
			return;
		}

		m_ShootTimer -= dt;

		if (m_ShootTimer <= 0.0f)
		{
			m_IsTelegraphing = true;
			m_TelegraphTimer = m_TelegraphDuration;
			SetColor({ 255, 255, 255, 180 });
		}
	}

#undef max

	float32 GCEnemy::DistanceToTarget()
	{
		if (mp_Target == nullptr)
			return std::numeric_limits<float32>::max();

		return GetPosition().GetDistance(mp_Target->GetPosition());
	}

	void GCEnemy::ShootAt(Vector2f worldTarget)
	{
		if (!m_CanShoot)
			return;

		::Scene* pScene = ::SceneManager::GetInstance().GetCurrentScene();

		Projectile* pProj = pScene->CreateEntity<Projectile>(gcle::Shapes::Circle);
		pProj->SetOwner(this);
		pProj->SetDamage(10);

		pProj->SetPosition(GetPosition().x, GetPosition().y);
		pProj->ScaleBy({ 0.1f, 0.1f });
		//pProj->GetTransform2D().UpdateChildPosition();

		pProj->SetColor(Color::Red);
		pProj->SetSpeed(700.0f);
		pProj->GoToDirection(worldTarget.x, worldTarget.y);
	}

	void GCEnemy::OnDestroy()
	{

	}

	void GCEnemy::OnInitialize()
	{
		SetTag(Tag::Enemy);

		CreateCollider(gcle::Shapes::Rectangle, true, { { -3.0f, 0.0f }, 0, { 0.3f, 0.45f } });

		SetRigidBody(true);
		GetRigidBody()->SetGravity(false);
		GetRigidBody()->ActivateFriction(true);
		GetRigidBody()->SetFriction({ 0.99f , 0.99f});
		GetRigidBody()->SetCollisionOnContinuous();

		SetTexture("enemy");

		AddAnimation("Idle", 0, 3, 0, 64, 64, 0.09f);
		AddAnimation("Walk", 0, 5, 3, 64, 64, 0.05f);
		AddAnimation("Hit", 0, 3, 5, 64, 64, 0.05f);
		AddAnimation("Death", 0, 10, 6, 64, 64, 0.095f);
		AddAnimation("Appear", 0, 11, 9, 64, 64, 0.095f);
		AddAnimation("Teleport", 0, 11, 21, 64, 64, 0.075f);

		AddFunctionInFrame("Appear", 11, [this]()
			{
				PlayAnimation("Idle", AnimationMode::Loop | AnimationMode::IgnoreIfAlreadyPlaying);
				m_CanShoot = true;
				m_State = EnemyState::Idle;
			});

		AddFunctionInFrame("Hit", 2, [this]()
			{
				if (this->GetCurrentLife() <= 0)
				{
					PlayAnimation("Death", AnimationMode::Lock, AnimationInterrupt::Force);
					m_CanShoot = false;
				}
			});

		AddFunctionInFrame("Death", 10, [this]()
			{
				InteractableHeart* pDrop = SceneManager::GetInstance().GetCurrentScene()->CreateEntity<InteractableHeart>(gcle::Shapes::Rectangle);
				pDrop->SetPosition(GetPosition().x, GetPosition().y);

				pDrop->SetParent(&GetTransform2D());

				Destroy();
			});

		//PlayAnimation("Appear", AnimationMode::IgnoreIfAlreadyPlaying | AnimationMode::Lock, AnimationInterrupt::Force);

		SetPosition(100.0f, 100.0f);
		 
		Entity* pSpawnZone = SceneManager::GetInstance().GetCurrentScene()->CreateEntity<Entity>(gcle::Shapes::Rectangle);
		pSpawnZone->SetPosition(GetPosition().x, GetPosition().y); 
		pSpawnZone->SetParent(&GetTransform2D());
		pSpawnZone->SetRigidBody(false);
		pSpawnZone->SetTexture("Spawn");
		pSpawnZone->AddAnimation("Spawn", 0, 3, 0, 32, 32, 0.1f);
		
		pSpawnZone->AddFunctionInFrame("Spawn", 1, [this]()
			{
				PlayAnimation("Appear", AnimationMode::IgnoreIfAlreadyPlaying | AnimationMode::Lock, AnimationInterrupt::Force);
			});
		
		pSpawnZone->AddFunctionInFrame("Spawn", 3, [pSpawnZone]()
			{
				pSpawnZone->Destroy();
			});

		pSpawnZone->PlayAnimation("Spawn", AnimationMode::Lock);

	}

	void GCEnemy::OnCollision(Entity* collidedWith) {}
	void GCEnemy::OnCollisionExit(Entity* collidedWith) {}
	void GCEnemy::OnCollisionEnter(Entity* collidedWith) {}

	void GCEnemy::Death()
	{
		Character::Death();
		m_State = EnemyState::Dead;

		GetRigidBody()->Stop();
		GetRigidBody()->SetActive(false);
	}

	void GCEnemy::Heal(int amount)
	{
		Character::Heal(amount);
	}

	void GCEnemy::Shoot()
	{
		if (mp_Target != nullptr)
			ShootAt(mp_Target->GetPosition());
	}

	void GCEnemy::Damage(int amount)
	{
		if (m_State == EnemyState::Spawning)
			return;

		Character::Damage(amount);
	}

	void GCEnemy::SetTarget(Entity* pTarget) { mp_Target = pTarget; }
	void GCEnemy::SetDetectionRange(float32 range) { m_DetectionRange = range; }
	void GCEnemy::SetAttackRange(float32 range) { m_AttackRange = range; }
	void GCEnemy::SetMoveForce(float32 force) { m_MoveForce = force; }
	void GCEnemy::SetShootCooldown(float32 cooldown) { m_ShootCooldown = cooldown; }
	EnemyState GCEnemy::GetState() const { return m_State; }
}