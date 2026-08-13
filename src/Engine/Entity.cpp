#include "Entity.h"

#include "Collider.h"
#include "PhysicsManager.h"

namespace
{
    int64 s_NextEntityId = 0;
}

Entity::~Entity()
{
    PhysicsManager::GetInstance().RemoveEntity(this);

    for (Collider* collider : mp_Colliders)
        delete collider;

    mp_Colliders.clear();
}

void Entity::Initialize(gcle::Shapes shape)
{

    m_Target.isSet = false;
    GameObject::Initialize(shape);
    Initialize();
}

void Entity::Initialize()
{
    m_Tag = -1;
    m_ToDestroy = false;
    m_Mask = 1;
    m_Id = s_NextEntityId++;

    m_Target.isSet = false;

    m_RigidBody.Initialize(&m_Transform);
    m_RigidBody.SetActive(true);

    OnInitialize();
}

void Entity::Update(float32 dt)
{
    if (m_RigidBody.IsActive())
        m_RigidBody.Update(dt);

    if (m_IsStatic == false)
    {
        float32 distance = dt * m_Speed;
        Vector2f translation = m_Direction * distance;

        Move(translation);
        if (mp_RenderShape != nullptr) {
            Texture* tex = mp_RenderShape->GetTexture();
            if (tex != nullptr)
            {
                if (tex->IsSprite())
                    static_cast<Sprite*>(tex)->UpdateAnimation(dt, mp_RenderShape);
            }
        }
        if (m_Target.isSet)
        {
            float32 x1 = GetPosition().x;
            float32 y1 = GetPosition().y;

            float32 x2 = x1 + m_Direction.x * m_Target.distance;
            float32 y2 = y1 + m_Direction.y * m_Target.distance;

            m_Target.distance -= distance;

            if (m_Target.distance <= 0)
            {
                //SetPosition(m_Target.position.x, m_Target.position.y);
                m_Direction = Vector2f({ 0.f, 0.f });
                m_Target.isSet = false;
            }
        }
    }


    GameObject::Update(dt);
    OnUpdate();
}

void Entity::Destroy()
{
    if (m_ToDestroy)
        return;

    m_ToDestroy = true;
    PhysicsManager::GetInstance().RemoveEntity(this);
    OnDestroy();
}

void Entity::Move(Vector2f translation) {
    Vector2f pivot = m_Transform.GetPosition();
    m_Transform.SetPosition({ pivot.x + translation.x, pivot.y + translation.y });
}

bool Entity::GoToPosition(float32 x, float32 y, float32 speed)
{
    if (GoToDirection(x, y, speed) == false)
        return false;

    Vector2f position = m_Transform.GetPosition();

    m_Target.position = { x, y };
    m_Target.distance = position.GetDistance({ x, y });
    m_Target.isSet = true;

    return true;
}

bool Entity::GoToDirection(float32 x, float32 y, float32 speed)
{
    Vector2f position = m_Transform.GetPosition();
    Vector2f direction = Vector2f({ x - position.x, y - position.y });

    direction = direction.Normalized();

    SetDirection(direction.x, direction.y, speed);

    return true;
}

void Entity::SetDirection(float32 x, float32 y, float32 speed)
{
    if (speed > 0)
    {
        m_Speed = speed;
    }

    m_Direction = { x, y };
}

void Entity::SetSpeed(float32 speed)
{
    m_Speed = speed;
}

void Entity::SetRigidBody(bool isRigidBody)
{
    m_RigidBody.SetActive(isRigidBody);
    m_IsHighlighted = isRigidBody;

    if (isRigidBody)
        PhysicsManager::GetInstance().AddEntity(this);
    else
        PhysicsManager::GetInstance().RemoveEntity(this);
}

void Entity::SetStatic(bool isStatic)
{
    m_IsStatic = isStatic;
}

int64 Entity::GetId() const
{
    return m_Id;
}

int32* Entity::GetCollisionLayer()
{
    return &m_Mask;
}

RigidBody2D* Entity::GetRigidBody()
{
    return &m_RigidBody;
}

const RigidBody2D* Entity::GetRigidBody() const
{
    return &m_RigidBody;
}

bool Entity::IsStatic() const
{
    return m_IsStatic;
}

bool Entity::IsColliding(Entity* other) const
{
    if (other == nullptr)
        return false;

    for (Collider* collider : mp_Colliders)
    {
        if (collider == nullptr || !collider->IsActive())
            continue;

        for (Collider* otherCollider : other->mp_Colliders)
        {
            if (otherCollider == nullptr || !otherCollider->IsActive())
                continue;

            if (PhysicsManager::GetInstance().IsColliding(collider, otherCollider))
                return true;
        }
    }

    return false;
}

bool Entity::IsRigidBody() const
{
    return m_RigidBody.IsActive();
}

bool Entity::HasCollider() const
{
    return !mp_Colliders.empty();
}

void Entity::SetParent(Transform2D* pParent)
{
    m_Transform.SetParent(pParent);
}

void Entity::AddCollider(Collider* collider)
{
    if (collider == nullptr)
        return;

    const bool hadNoCollider = mp_Colliders.empty();

    collider->SetOwner(this);
    mp_Colliders.insert(collider);

    if (hadNoCollider)
        PhysicsManager::GetInstance().AddEntity(this);
}

void Entity::RemoveCollider(Collider* collider)
{
    if (collider == nullptr)
        return;

    mp_Colliders.erase(collider);

    if (mp_Colliders.empty() && !m_RigidBody.IsActive())
        PhysicsManager::GetInstance().RemoveEntity(this);
}

const std::unordered_set<Collider*>& Entity::GetColliders() const
{
    return mp_Colliders;
}

Collider* Entity::CreateCollider( gcle::Shapes shape, bool isActive, gcle::ColliderDesc desc, bool isTrigger)
{
    gcle::Shape* colliderShape = GetBaseShape(shape);
    if (colliderShape == nullptr)
        return nullptr;

    const Vector2f entityScale = m_Transform.GetScale();
    colliderShape->SetScale({ desc.RelativeScale.x * entityScale.x, desc.RelativeScale.y * entityScale.y });

    Collider* collider = GCLE_NEW Collider();
    collider->SetTrigger(isTrigger);
    collider->Initialize( colliderShape, m_Transform.GetPosition() + desc.RelativePosition, m_Transform.GetDegAngle() + desc.RelativeRotation, this);

    AddCollider(collider);
    collider->SetActive(isActive);

    return collider;
}

bool Entity::IsWorldText() const
{
    return m_IsWorldText;
}
