#pragma once

#include "Engine/Entity.h"

namespace Demo
{
	class Projectile : public Entity
	{
	public:
		void OnUpdate()								override;
		void OnDestroy()							override;
		void OnInitialize()							override;
		void OnTrigger(Entity* collidedWith)		override;
		void OnTriggerExit(Entity* collidedWith)	override;
		void OnTriggerEnter(Entity* collidedWith)   override;

		void SetOwner(Entity* pEntity);
		Entity* GetOwner() const;

		void SetDamage(int damage);
		int GetDamage() const;

	private:
		Entity* mp_Owner = nullptr;
		int32   m_OwnerTag = -1;
		int64   m_OwnerId = -1;

		int m_Damage = 10;
	};

}
