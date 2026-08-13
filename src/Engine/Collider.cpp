#include "Collider.h"
#include "Entity.h"

Collider::~Collider()
{
    if (mp_Shape != nullptr)
    {
        delete mp_Shape;
        mp_Shape = nullptr;
    }
}

void Collider::Initialize(Shape* pShape, Vector2f position, float32 rotation, Entity* pOwner)
{
    mp_Shape = pShape;
    mp_Owner = pOwner;
    mp_Shape->SetPosition(position.x, position.y);
	mp_Shape->SetRotation(rotation);
    mp_Shape->GetTransform()->SetParent(&mp_Owner->GetTransform2D());
}

void Collider::CollidingOn(Vector2f direction)
{
    if (direction.x < 0.0f)
        m_CollisionDirection.isCollidingOnRight = true;
    else if (direction.x > 0.0f)
        m_CollisionDirection.isCollidingOnLeft = true;

    if (direction.y > 0.0f)
        m_CollisionDirection.isCollidingOnTop = true;
    else if (direction.y < 0.0f)
        m_CollisionDirection.isCollidingOnBottom = true;
}

void Collider::CollidingOnX(float32 direction)
{
    if (direction < 0.0f)
        m_CollisionDirection.isCollidingOnRight = true;
    else if (direction > 0.0f)
        m_CollisionDirection.isCollidingOnLeft = true;
}

void Collider::CollidingOnY(float32 direction)
{
    if (direction > 0.0f)
        m_CollisionDirection.isCollidingOnTop = true;
    else if (direction < 0.0f)
        m_CollisionDirection.isCollidingOnBottom = true;
}

void Collider::StoppedColliding()
{
    m_CollisionDirection.isCollidingOnRight = false;
    m_CollisionDirection.isCollidingOnLeft = false;
    m_CollisionDirection.isCollidingOnTop = false;
    m_CollisionDirection.isCollidingOnBottom = false;
}

bool Collider::IsActive()
{
    return m_IsActive;
}

bool Collider::GetInQuerySeen() {
    return m_InQuerySeen;
}

void Collider::SetInQuerySeen(bool seen) {
    m_InQuerySeen = seen;
}

AABB Collider::GetAABB(){
    return m_Aabb;
}

void Collider::SetAABB(AABB aabb){
    m_Aabb = aabb;
}

void Collider::SetActive(bool activate) { m_IsActive = activate; }

Shape*          Collider::GetShape()         { return mp_Shape; }
const Shape*    Collider::GetShape() const   { return mp_Shape; }

const CollisionDirection& Collider::GetCollisionDirection() const
{
    return m_CollisionDirection;
}

Entity*   Collider::GetOwner() const           { return mp_Owner; }
void      Collider::SetOwner(Entity* pOwner) { mp_Owner = pOwner; }

bool Collider::IsTrigger() const
{
    return m_IsTrigger;
}

void Collider::SetTrigger(bool isTrigger)
{
    m_IsTrigger = isTrigger;
}
