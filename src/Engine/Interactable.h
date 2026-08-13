#pragma once
#include "UI.h"

class Interactable : public UI
{
public:
    void SetInteractable(bool enabled);
    bool IsInteractable() const;

    bool IsHovered() const;
    bool IsPressed() const;

protected:
    virtual void OnHoverEnter() {}
    virtual void OnHoverExit() {}
    virtual void OnPressed() {} 
    virtual void OnReleased(bool insideOnRelease) {} 
    virtual void Update(float32 dt) override;

private:

private:
    bool m_IsHovered = false;
    bool m_IsPressed = false;
    bool m_IsDisabled = false;

    friend class Scene;
};