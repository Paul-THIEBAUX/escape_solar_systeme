#include "Interactable.h"
#include "GameManager.h"
#include "Core/InputManager.h"

void Interactable::SetInteractable(bool enabled)
{
    m_IsDisabled = !enabled;
}

bool Interactable::IsInteractable() const
{
    return !m_IsDisabled;
}

bool Interactable::IsHovered() const
{
    return m_IsHovered;
}

bool Interactable::IsPressed() const
{
    return m_IsPressed;
}

void Interactable::Update(float32 dt)
{
    UI::Update(dt);

    if (m_IsDisabled)
    {
        if (m_IsHovered) { m_IsHovered = false; OnHoverExit(); }
        return;
    }

    Vector2f mousePos = GameManager::GetInstance().GetWindow()->GetMousePositionOnRenderTarget();
    bool insideNow = IsInside(mousePos);

    if (insideNow && !m_IsHovered) 
    { 
        m_IsHovered = true; 
        OnHoverEnter(); 
    }

    if (!insideNow && m_IsHovered) 
    { 
        m_IsHovered = false; 
        OnHoverExit(); 
    }

    if (insideNow && InputManager::GetInstance().IsDown(LeftButton))
    {
        m_IsPressed = true;
        OnPressed();
    }
    else if (m_IsPressed && InputManager::GetInstance().IsUp(LeftButton))
    {
        bool wasStillInside = m_IsPressed && insideNow;
        m_IsPressed = false;
        OnReleased(wasStillInside);
    }
}