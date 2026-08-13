#include "Object.h"

#include "Tag.h"

void Demo::Object::Interact(Entity* pEntity)
{
	if (m_CanInteract)
		OnInteract(pEntity);
}

bool Demo::Object::CanBeInteractWith() const
{
	return m_CanInteract;
}

void Demo::Object::SetInteract(bool interact)
{
	m_CanInteract = interact;
}

bool Demo::Object::IsPlayerInRange() const
{
	return m_PlayerInRange;
}

void Demo::Object::OnTriggerEnter(Entity* pOther)
{
	if (pOther != nullptr && pOther->IsTag(Tag::Player))
		m_PlayerInRange = true;
}

void Demo::Object::OnTriggerExit(Entity* pOther)
{
	if (pOther != nullptr && pOther->IsTag(Tag::Player))
		m_PlayerInRange = false;
}
