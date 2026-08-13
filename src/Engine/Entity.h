#pragma once

#include <unordered_map>
#include <unordered_set>

#include "GameObject.h"
#include "RigidBody.h"

class Collider;

struct Target
{
    Vector2f position;
    float distance = 0.f;
    bool isSet = false;
};

namespace gcle
{
    // Every collider informations are relative to the entity it was created from
    // 
    // Relative Position : 
    //      if x = 1.0f and y = 0.0f it means that the collider is 1 unite on the right of the entity
    //      if entity position is at { 10.0f, 5.0f }, collider position is at { 11.0f, 5.0f }
    //      because 10.0f + 1.0f = 11.0f
    // 
    // Relative Rotation :
    //      if rotation = 10.0f it means that the collider is rotate from 10.0f more degree than the entity
    //      if entity rotation is 10.0f degree, collider rotation is 20.0f degree
    //      because 10.0f + 10.0f = 20.0f
    // 
    // Relative Scale : 
    //      if x = 2.0f and y = 1.0f it means that the collider is 2 times bigger on x than the entity
    //      if entity scale is at { 10.0f, 5.0f }, collider scale is at { 20.0f, 5.0f }
    //      because 10.0f * 2.0f = 20.0f
    struct ColliderDesc
    {
        Vector2f    RelativePosition    = { 0.0f, 0.0f };
        float32     RelativeRotation    =   0.0f;
        Vector2f    RelativeScale       = { 1.0f, 1.0f };

        ColliderDesc() = default;
        ColliderDesc(Vector2f _RelativePosition, float32 _RelativeRotation, Vector2f _RelativeScale) : 
            RelativePosition(_RelativePosition),
            RelativeRotation(_RelativeRotation),
            RelativeScale(_RelativeScale)
        { }
            
    };               
}

class Entity : public GameObject
{
public:
    void Destroy();

    void Move(Vector2f translation);
    bool GoToPosition(float32 x, float32 y, float32 speed = -1.f);
    bool GoToDirection(float32 x, float32 y, float32 speed = -1.f);
    void SetDirection(float32 x, float32 y, float32 speed);
    void SetSpeed(float32 speed);
     
    void SetRigidBody(bool isRigidBody);
    void SetStatic(bool isStatic);

    int64 GetId() const;
    int32* GetCollisionLayer();
    RigidBody2D* GetRigidBody();
    const RigidBody2D* GetRigidBody() const;

    bool IsStatic() const;
    bool IsColliding(Entity* other) const; 
    bool IsRigidBody() const;
    bool HasCollider() const;

    void AddCollider(Collider* collider);
    void RemoveCollider(Collider* collider);
    const std::unordered_set<Collider*>& GetColliders() const;

    void SetParent(Transform2D* pParent);

    Collider* CreateCollider(
        gcle::Shapes shape,
        bool isActive,
        gcle::ColliderDesc desc,
        bool isTrigger = false);

    bool IsWorldText() const;

    virtual void OnCollision(Entity* other) {}
    virtual void OnCollisionEnter(Entity* other) {}
    virtual void OnCollisionExit(Entity* other) {}

    virtual void OnTrigger(Entity* other) {}
    virtual void OnTriggerEnter(Entity* other) {}
    virtual void OnTriggerExit(Entity* other) {}

protected:
    Entity() = default;
    ~Entity() override;

    void OnUpdate() override {}
    void OnDestroy() override {}
    void OnInitialize() override {}

    bool m_IsWorldText = false;

private:
    void Initialize(gcle::Shapes shape) override;
    void Initialize();
    void Update(float32 dt) override;

private:
    
    bool m_IsStatic = false;
    bool m_IsHighlighted = false;
    Target m_Target;
    float m_Speed = 0;
    Vector2f m_Direction;

    RigidBody2D m_RigidBody;
    std::unordered_set<Collider*> mp_Colliders;
    std::unordered_map<int64, Entity*> m_CollidingEntity;
    std::unordered_map<int64, Entity*> m_TriggeringEntity;

private:
    friend class Scene;
    friend class Camera;
    friend class GameManager;
    friend class PhysicsManager;
};
