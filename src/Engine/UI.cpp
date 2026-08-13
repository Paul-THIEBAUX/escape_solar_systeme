#include "UI.h"

UI::~UI()
{
}

void UI::Initialize(gcle::Shapes shape)
{
    GameObject::Initialize(shape);

    if (mp_RenderShape != nullptr)
    {
        Vector2f pos = m_Transform.GetPosition();

        mp_RenderShape->SetPosition(pos.x, pos.y);
        mp_RenderShape->SetScale(m_Transform.GetScale());
        mp_RenderShape->SetRotation(m_Transform.GetDegAngle());
    }

    OnInitialize();
}

void UI::Initialize()
{
    OnInitialize();
}

void UI::Update(float32 dt)
{
    GameObject::Update(dt);

    if (mp_RenderShape != nullptr)
    {
        Vector2f pos = m_Transform.GetPosition();

        // UI = position écran / render target, pas position monde
        mp_RenderShape->SetPosition(pos.x, pos.y);
        mp_RenderShape->SetScale(m_Transform.GetScale());
        mp_RenderShape->SetRotation(m_Transform.GetDegAngle());
    }

    OnUpdate();
}

void UI::OnSetPosition(float32 x, float32 y)
{
    if (mp_Text != nullptr) {
        mp_Text->SetPosition(static_cast<int32>(x - (mp_Text->GetSizes().x / 2)), static_cast<int32>(y - (mp_Text->GetSizes().y / 2)));
    }
}

void UI::SetText(const std::string& text)
{
    mp_Text->SetText(text);
}

void UI::SetActive(bool active)
{
    GameObject::SetActive(active);

    if (mp_Text != nullptr)
    {
        mp_Text->SetActive(active);
    }
}
