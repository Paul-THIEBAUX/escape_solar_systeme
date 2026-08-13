#pragma once

#include <vector>

#include "MathGC.h"
#include "include.h"

struct Transform2D
{
public: 
    Transform2D() = default;
    ~Transform2D();


    void RotateToDirection();
    void Forward();

    void UpdatePositionWithParentPosition();
    void UpdateChildPosition();

public:
    void SetPosition(Vector2f);
    void SetDirection(Vector2f);
    
    void SetDegAngle(Degrees angle);
    void SetRadAngle(Radians angle);

    void SetScale(Vector2f scale);

    void SetParent(Transform2D* pParent);
    
    void SetDirty();
    void ClearDirty();

public:
    Vector2f& GetPosition();
    Vector2f& GetDirection();
    Vector2f& GetScale();

    Radians& GetRadAngle();
    Degrees& GetDegAngle();

    Transform2D* GetParent();
    Transform2D* GetChild(uint32 index);

    const Transform2D* GetParent() const;
    const Transform2D* GetChild(uint32 index) const;

    uint32 GetChildCount() const;

    bool IsDirty() const;
     
    Matrix3x3 GetMatrix() const;

public:
    void AddChild(Transform2D* pChild);
    void RemoveParent();
    void RemoveChild(uint32 index);
    void Initialize(Vector2f m_Position, Degrees angle);

private:
    Vector2f m_Position = { 0.f,0.f };
    Vector2f m_Direction = { 0.f,0.f };
    Vector2f m_Scale = { 1.f,1.f };

    Degrees m_DegAngle = 0.f;
    Radians m_RadAngle = 0.f;

    Transform2D* mp_Parent = nullptr;
    std::vector<Transform2D*> m_Childs;

    float m_DistanceFromParent = 0;
    Radians m_AngleDifferenceToParent = 0;
    Radians m_OffsetAngle = 0;
    Vector2f m_ScaleRatioToParent = { 1.0f, 1.0f };

    Radians m_ParentAncientAngle = 0;

    bool m_IsDirty = false;

public:
    bool isSet = false;
};