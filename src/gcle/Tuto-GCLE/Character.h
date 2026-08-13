#pragma once

#include "Engine/Entity.h"

namespace Demo
{
	class Character : public Entity
	{
	public:

		void OnUpdate() override;

		void SetMaxLife(int maxLife);
		void SetCurrentLife(int currentLife);

		int GetMaxLife() const;
		int GetCurrentLife() const;

		virtual void Damage(int amount);
		virtual void Death();
		virtual void Heal(int amount);

		virtual void Shoot() {};

	protected: 
		bool IsFlinching() const;

		void SetInvincible(bool isInvincible, float duration = 0.0f);
		bool IsInvincible() const;

	protected:
		bool m_CanShoot = false;

	private:
		int m_MaxLife = 20;
		int m_CurrentLife = 20;

		float m_baseDamageDuration = 1.0f;
		float m_damageDuration = 0.0f;
		bool m_wasHit = false;

		bool  m_IsInvincible = false;
		float m_InvincibilityTimer = 0.0f;

	protected:
		friend class DemoScene;
	}; 
}
