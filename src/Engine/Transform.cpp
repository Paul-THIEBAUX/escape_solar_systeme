#include "Transform.h"

Transform2D::~Transform2D() {

    if (mp_Parent != nullptr) {
        for (auto it = mp_Parent->m_Childs.begin(); it != mp_Parent->m_Childs.end(); ++it) {
            if (*it == this) {
                mp_Parent->m_Childs.erase(it);
                break;
            }
        }
    }

    for (auto pChild : m_Childs) {
        pChild->SetParent(nullptr);
    }
}
 
void Transform2D::Initialize(Vector2f position, Degrees angle)
{
    m_Position = position;
    m_DegAngle = angle;
    m_RadAngle = MathGC::DegToRad(m_DegAngle); 

    isSet = true;
}

#pragma region Getters

Transform2D* Transform2D::GetParent() {
    return mp_Parent;
}

Transform2D* Transform2D::GetChild(uint32 index) {
    return m_Childs[index];
}

const Transform2D* Transform2D::GetParent() const {
    return mp_Parent;
}

const Transform2D* Transform2D::GetChild(uint32 index) const {
    return m_Childs[index];
}

uint32 Transform2D::GetChildCount() const {
    return static_cast<uint32>(m_Childs.size());
}

Vector2f& Transform2D::GetPosition()
{
    return m_Position;
}

Vector2f& Transform2D::GetDirection()
{
    return m_Direction;
}

Vector2f& Transform2D::GetScale()
{
    return m_Scale;
}

Degrees& Transform2D::GetDegAngle()
{
    return m_DegAngle;
}

Radians& Transform2D::GetRadAngle()
{
    return m_RadAngle;
}
#pragma endregion

#pragma region Setters

void Transform2D::SetParent(Transform2D* pParent) {
    if (pParent == this) {
        //GPC_WARNING << "incest is bad" << ENDL;
        return;
    }
    if (mp_Parent != nullptr) {
        RemoveParent();
    }
    mp_Parent = pParent;
    if (mp_Parent != nullptr) {
        mp_Parent->m_Childs.push_back(this);

        float dx = m_Position.x - mp_Parent->GetPosition().x;
        float dy = m_Position.y - mp_Parent->GetPosition().y;
        m_DistanceFromParent = std::sqrt(dx * dx + dy * dy);

        m_AngleDifferenceToParent = m_RadAngle - mp_Parent->GetRadAngle();
         
        Vector2f offset = m_Position - mp_Parent->GetPosition();
        m_OffsetAngle = MathGC::VecToAngle(offset) - mp_Parent->GetRadAngle(); 

        Vector2f parentScale = mp_Parent->GetScale();
        m_ScaleRatioToParent = {
            (parentScale.x != 0.0f) ? m_Scale.x / parentScale.x : 1.0f,
            (parentScale.y != 0.0f) ? m_Scale.y / parentScale.y : 1.0f
        };
    }
}

void Transform2D::SetPosition(Vector2f position)
{
    if (position != m_Position) 
    {
        m_Position = position;
        SetDirty();
    }
}

void Transform2D::SetScale(Vector2f scale)
{
    m_Scale = scale;

    if (mp_Parent != nullptr)
    {
        Vector2f parentScale = mp_Parent->GetScale();

        m_ScaleRatioToParent = {
            parentScale.x != 0.0f ? m_Scale.x / parentScale.x : 1.0f,
            parentScale.y != 0.0f ? m_Scale.y / parentScale.y : 1.0f
        };
    }

    SetDirty();
}

void Transform2D::SetDirection(Vector2f position)
{
    m_Direction = position;
}
 
void Transform2D::SetDegAngle(Degrees angle)
{
    m_DegAngle = angle;
    m_RadAngle = MathGC::DegToRad(angle);

    if (mp_Parent != nullptr)
    {
        m_AngleDifferenceToParent = m_RadAngle - mp_Parent->GetRadAngle();
    }

    SetDirty();
}

void Transform2D::SetRadAngle(Radians angle)
{
    m_RadAngle = angle;
    m_DegAngle = MathGC::RadToDeg(angle);

    if (mp_Parent != nullptr)
    {
        m_AngleDifferenceToParent = m_RadAngle - mp_Parent->GetRadAngle();
    }

    SetDirty();
}

void Transform2D::SetDirty() {
    m_IsDirty = true;
    for (auto pChild : m_Childs) {
        pChild->SetDirty();
    }
}

void Transform2D::ClearDirty() {
    m_IsDirty = false;
}

bool Transform2D::IsDirty() const {
    return m_IsDirty;
}

Matrix3x3 Transform2D::GetMatrix() const {
    return Matrix3x3::Translation(m_Position) * Matrix3x3::Rotation(m_RadAngle) * Matrix3x3::Scale(m_Scale);
}

#pragma endregion

#pragma region Add/Remove

void Transform2D::AddChild(Transform2D* pChild) {
    if (pChild == this) {
        //GPC_WARNING << "Incest is bad" << ENDL;
        return;
    }
    for (auto& child : m_Childs) {
        if (child == pChild) return;
    }
    pChild->SetParent(this);

}

void Transform2D::RemoveParent() {
    if (mp_Parent != nullptr) {
        for (auto it = mp_Parent->m_Childs.begin(); it != mp_Parent->m_Childs.end(); ++it) {
            if (*it == this) {
                mp_Parent->m_Childs.erase(it);
                break;
            }
        }
    }
    mp_Parent = nullptr;
}

void Transform2D::RemoveChild(uint32 index) {
    if (index >= GetChildCount()) return;
    auto it = m_Childs.begin();
    std::advance(it, index);
    m_Childs[index]->mp_Parent = nullptr;
    m_Childs.erase(it);
}
#pragma endregion

#pragma region Updates

void Transform2D::RotateToDirection()
{ 
    Radians rad = MathGC::VecToAngle(m_Direction);

    SetRadAngle(rad);
    SetDegAngle(MathGC::RadToDeg(rad)); 
}

void Transform2D::Forward()
{ 
    m_Direction = MathGC::AngleToVec(m_RadAngle); 
}

void Transform2D::UpdatePositionWithParentPosition()
{
    if (mp_Parent == nullptr || !m_IsDirty) {
        return;
    }

    Radians currentParentAngle = mp_Parent->GetRadAngle(); 

    m_Position.x = mp_Parent->GetPosition().x + m_DistanceFromParent * std::cos(currentParentAngle + m_OffsetAngle);
    m_Position.y = mp_Parent->GetPosition().y + m_DistanceFromParent * std::sin(currentParentAngle + m_OffsetAngle);

    m_RadAngle = mp_Parent->GetRadAngle() + m_AngleDifferenceToParent; 
    m_DegAngle = MathGC::RadToDeg(m_RadAngle);

}

void Transform2D::UpdateChildPosition()
{
    if (m_Childs.empty())
        return;

    for (Transform2D* child : m_Childs)
    {
        child->m_Position.x =
            m_Position.x +
            child->m_DistanceFromParent * std::cos(m_RadAngle + child->m_OffsetAngle);

        child->m_Position.y =
            m_Position.y +
            child->m_DistanceFromParent * std::sin(m_RadAngle + child->m_OffsetAngle);


        child->m_RadAngle = m_RadAngle + child->m_AngleDifferenceToParent;
        child->m_DegAngle = MathGC::RadToDeg(child->m_RadAngle);

        child->m_Scale.x = m_Scale.x * child->m_ScaleRatioToParent.x;
        child->m_Scale.y = m_Scale.y * child->m_ScaleRatioToParent.y;

        child->UpdateChildPosition();
    }
}

#pragma endregion