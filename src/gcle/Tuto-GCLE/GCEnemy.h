#pragma once

#include "Tuto-GCLE/Character.h"

namespace Demo
{
	enum class EnemyState
	{
		Spawning,
		Idle,
		Chase,
		Attack,
		Dead
	};

    class GCEnemy : public Character
    {
    public: 

		void OnUpdate()								override;
		void OnDestroy()							override;
		void OnInitialize()							override;
		void OnCollision(Entity* collidedWith)		override;
		void OnCollisionExit(Entity* collidedWith)	override;
		void OnCollisionEnter(Entity* collidedWith) override;

		void Death()								override;
		void Heal(int amount)						override;
		void Shoot()								override;
		void Damage(int amount)                     override;

		void SetTarget(Entity* pTarget);
		void SetDetectionRange(float32 range);
		void SetAttackRange(float32 range);
		void SetMoveForce(float32 force);
		void SetShootCooldown(float32 cooldown);

		EnemyState GetState() const;

	private:
		void UpdateState(float32 dt);
		void UpdateIdle(float32 dt);
		void UpdateChase(float32 dt);
		void UpdateAttack(float32 dt);

		float32 DistanceToTarget();
		void ShootAt(Vector2f worldTarget);

	private:
		Entity* mp_Target = nullptr;

		EnemyState m_State = EnemyState::Spawning;

		float32 m_DetectionRange = 500.0f;
		float32 m_AttackRange = 350.0f;
		float32 m_MoveForce = 600.0f;

		float32 m_ShootCooldown = 1.2f;
		float32 m_ShootTimer = 0.0f;
		 
		bool	m_IsTelegraphing = false;
		float32 m_TelegraphDuration = 0.3f;
		float32 m_TelegraphTimer = 0.0f;
	}; 
}
