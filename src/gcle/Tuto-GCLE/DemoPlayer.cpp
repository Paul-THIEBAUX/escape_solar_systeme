#include "DemoPlayer.h"

#include "Core/InputManager.h"
#include "Engine/GameManager.h"
#include "Engine/SceneManager.h"
#include "Core/Timer.h"

#include "Tuto-GCLE/Projectile.h"
#include "Tag.h"

namespace Demo
{
	void GCPlayer::OnUpdate()
	{
		Character::OnUpdate();

		InputManager& im = InputManager::GetInstance();
		float32 dt = static_cast<float32>(::GameManager::GetInstance().GetTime()->GetDeltaTime());
		RigidBody2D* rb = GetRigidBody();

		constexpr float32 MOVE_FORCE = 1200.0f;

		UpdateDodgeRoll(dt);

		if (m_IsDodging)
			return;

		if (im.IsHeld('Q') && m_CanMove)
		{
			rb->AddForce({ -1, 0 }, MOVE_FORCE, dt);
			SetTextureFlip(TextureFlipMode::FLIP_HORIZONTAL);
			PlayAnimation("Walk", AnimationMode::Loop | AnimationMode::IgnoreIfAlreadyPlaying);
		}
		if (im.IsHeld('D') && m_CanMove)
		{
			rb->AddForce({ 1, 0 }, MOVE_FORCE, dt);
			SetTextureFlip(TextureFlipMode::FLIP_NONE);
			PlayAnimation("Walk", AnimationMode::Loop | AnimationMode::IgnoreIfAlreadyPlaying);
		}
		if (im.IsHeld('S') && m_CanMove)
		{
			rb->AddForce({ 0, 1 }, MOVE_FORCE, dt);
			PlayAnimation("Walk", AnimationMode::Loop | AnimationMode::IgnoreIfAlreadyPlaying);
		}
		if (im.IsHeld('Z') && m_CanMove)
		{
			rb->AddForce({ 0, -1 }, MOVE_FORCE, dt);
			PlayAnimation("Walk", AnimationMode::Loop | AnimationMode::IgnoreIfAlreadyPlaying);
		}



		if (
			rb->GetVelocity().x <= 0.05f && rb->GetVelocity().y <= 0.05f &&
			rb->GetVelocity().x >= -0.05f && rb->GetVelocity().y >= -0.05f &&
			m_CanMove
			) {
			PlayAnimation("Idle", AnimationMode::Loop | AnimationMode::IgnoreIfAlreadyPlaying);
		}
		else
		{
			m_LastMoveDirection = rb->GetVelocity();
		}

		if (im.IsDown(LeftButton))
		{
			Shoot();
		}

		if (im.IsDown(Space) && m_DodgeCooldownTimer <= 0.0f && m_CanMove)
		{
			m_IsDodging = true;
			m_DodgeTimer = m_DodgeDuration;
			m_DodgeCooldownTimer = m_DodgeCooldown;

			SetInvincible(true, m_DodgeDuration);
			SetColor({ 255, 255, 255, 50 });

			rb->Stop();

			if (m_LastMoveDirection.x == 0 && m_LastMoveDirection.y == 0)
				m_LastMoveDirection = { 1.0f, 0.0f };

			rb->AddImpulse(m_LastMoveDirection, m_DodgeForce);

			PlayAnimation("Walk", AnimationMode::Loop | AnimationMode::IgnoreIfAlreadyPlaying);
		}

		if (im.IsDown('F') && mp_InteractableObject.size() != 0)
		{
			mp_InteractableObject[0]->Interact(this);
			mp_InteractableObject.erase(mp_InteractableObject.begin());
		}
		
		if (im.IsDown('P'))
		{
			Damage(10);
		} 

	}

	void GCPlayer::UpdateDodgeRoll(float32 dt)
	{
		if (m_DodgeCooldownTimer > 0.0f)
			m_DodgeCooldownTimer -= dt;

		if (!m_IsDodging)
			return;

		m_DodgeTimer -= dt;

		if (m_DodgeTimer <= 0.0f)
		{
			m_IsDodging = false;
			SetColor(Color::White);
		}
	}

	void GCPlayer::OnDestroy() {}

	void GCPlayer::OnInitialize()
	{
		SetTag(Tag::Player);

		CreateCollider(gcle::Shapes::Rectangle, true, { { -3.0f, 0.0f }, 0, { 0.3f, 0.5f } });

		mp_InteractableRange = CreateCollider(gcle::Shapes::Circle, true, { { 0.0f, 0.0f }, 0, { 0.6f, 0.6f } }, true);

		

		SetRigidBody(true);
		GetRigidBody()->SetGravity(false);
		GetRigidBody()->ActivateFriction(true);
		GetRigidBody()->SetFriction({ 0.99f , 0.99f});
		GetRigidBody()->SetCollisionOnContinuous();

		SetTexture("player");

		AddAnimation("Idle", 0, 3, 0, 64, 64, 0.5f);
		AddAnimation("Walk", 0, 5, 3, 64, 64, 0.2f);
		AddAnimation("Hit", 0, 3, 5, 64, 64, 0.2f);
		AddAnimation("Death", 0, 10, 6, 64, 64, 0.25f);
		AddAnimation("Appear", 0, 11, 9, 64, 64, 0.095f);
		AddAnimation("Teleport", 0, 11, 21, 64, 64, 0.25f);

		AddFunctionInFrame("Appear", 11, [this]()
			{
				m_CanMove = true;
				PlayAnimation("Idle", AnimationMode::Loop | AnimationMode::IgnoreIfAlreadyPlaying);
				m_CanShoot = true;
			});

		AddFunctionInFrame("Hit", 0, [this]()
			{				
				if (this->GetCurrentLife() <= 0)
				{
					m_CanMove = false;
					PlayAnimation("Death", AnimationMode::Lock, AnimationInterrupt::Force);
					m_CanShoot = false;
				}
			});

		AddFunctionInFrame("Death", 10, [this]()
			{
				Destroy();
			});

		PlayAnimation("Appear", AnimationMode::IgnoreIfAlreadyPlaying | AnimationMode::Lock, AnimationInterrupt::Force);


		SetMaxLife(50);
		SetCurrentLife(50); 
	}

	void GCPlayer::OnCollision(Entity* collidedWith) {}
	void GCPlayer::OnCollisionExit(Entity* collidedWith) {}
	void GCPlayer::OnCollisionEnter(Entity* collidedWith) {}

	void GCPlayer::OnTriggerEnter(Entity* collidedWith)
	{
		if (collidedWith->IsTag(Tag::Decor))
		{
			Object* pObj = static_cast<Object*>(collidedWith);

			bool alreadyIn = false;
			for (auto& obj : mp_InteractableObject)
			{
				if (collidedWith->GetId() == obj->GetId())
					alreadyIn = true;
			}

			if (!alreadyIn && pObj->CanBeInteractWith())
			{
				mp_InteractableObject.push_back(pObj); 
			}
		}
	}

	void GCPlayer::OnTrigger(Entity* collidedWith)
	{
	}

	void GCPlayer::OnTriggerExit(Entity* collidedWith)
	{
		bool toClear = false;
		for (auto it = mp_InteractableObject.begin(); it != mp_InteractableObject.end(); )
		{
			Object* obj = *it;

			if (obj == nullptr || obj->GetId() == collidedWith->GetId())
			{   
				it = mp_InteractableObject.erase(it);
			} 
			else
			{
				++it;
			}

		}
	}
	 
	void GCPlayer::Death()
	{
		Character::Death(); 
	}

	void GCPlayer::Heal(int amount)
	{
		Character::Heal(amount);
	}

	void GCPlayer::Shoot()
	{
		if (!m_CanShoot)
			return;

		Scene* pScene = ::SceneManager::GetInstance().GetCurrentScene();

		Vector2f posToGo = pScene->GetCurrentCamera()->GetMouseScreenToWorldPosition();

		Projectile* pProj = pScene->CreateEntity<Projectile>(gcle::Shapes::Circle);
		pProj->SetOwner(this);

		pProj->SetPosition(GetPosition().x, GetPosition().y);
		pProj->ScaleBy({ 0.1f, 0.1f });
		pProj->GetTransform2D().UpdateChildPosition();

		pProj->SetColor(Color::Yellow);
		pProj->SetSpeed(400.0f);
		pProj->GoToDirection(posToGo.x, posToGo.y);
	}

	void GCPlayer::Damage(int amount)
	{
		Character::Damage(amount); 
	}
}