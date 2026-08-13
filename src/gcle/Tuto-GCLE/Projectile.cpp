#include "Projectile.h"

#include "Tag.h"
#include "Tuto-GCLE/DemoPlayer.h"
#include "Tuto-GCLE/GCEnemy.h"

namespace Demo
{
	void Projectile::OnUpdate()
	{
	}

	void Projectile::OnDestroy()
	{
	}

	void Projectile::OnInitialize()
	{
		SetRigidBody(true);
		CreateCollider(gcle::Shapes::Circle, true, { { 0.0f, 0.0f }, 0, { 1.0f, 1.0f } }, true);
	}

	void Projectile::OnTrigger(Entity* collidedWith)
	{
	}

	void Projectile::OnTriggerExit(Entity* collidedWith)
	{
	}

	void Projectile::OnTriggerEnter(Entity* pEntity)
	{
		if (pEntity->IsTag(Tag::Obstacle))
		{
			Destroy();
			return;
		}

		if (pEntity->IsTag(Tag::Player) && m_OwnerTag == Tag::Enemy)
		{
			static_cast<GCPlayer*>(pEntity)->Damage(m_Damage);
			Destroy();
		}

		if (pEntity->IsTag(Tag::Enemy) && m_OwnerId != pEntity->GetId())
		{
			static_cast<GCEnemy*>(pEntity)->Damage(m_Damage);
			Destroy();
		}
	}

	void Projectile::SetOwner(Entity* pEntity)
	{
		mp_Owner = pEntity;
		if (pEntity != nullptr)
		{
			m_OwnerTag = pEntity->IsTag(Tag::Enemy) ? Tag::Enemy : pEntity->IsTag(Tag::Player) ? Tag::Player : -1;
			m_OwnerId = pEntity->GetId();
		}
	}

	Entity* Projectile::GetOwner() const
	{
		return mp_Owner;
	}

	void Projectile::SetDamage(int damage)
	{
		m_Damage = damage;
	}

	int Projectile::GetDamage() const
	{
		return m_Damage;
	}
}
