#include "Table.h"

#include "Tag.h"

void Demo::Table::OnInitialize()
{
    SetTag(Tag::Decor);

    ScaleBy({ 0.8f, 0.64f });
    CreateCollider(gcle::Shapes::Rectangle, true, { { 0.0f, 0.0f }, 0, { 1.0f, 1.0f } });

    SetRigidBody(true);
    GetRigidBody()->SetGravity(false);
    SetStatic(true);
    GetRigidBody()->SetCollisionOnContinuous();

    SetTexture("Table_Vertical");
}

void Demo::Table::OnInteract(Entity* pEntity)
{
    if (m_IsDown == false)
    {
        SetTag(Tag::Obstacle);

        SetStatic(false);
        GetRigidBody()->ActivateFriction(true);
        GetRigidBody()->SetFriction({ 0.99f, 0.99f });

        m_CanInteract = false;
        m_IsDown = true;
    } 
}

bool Demo::Table::IsDown() const
{
    return m_IsDown;
}
