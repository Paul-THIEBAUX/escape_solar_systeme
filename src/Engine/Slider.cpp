#include "Slider.h"
#include "GameManager.h"
#include <algorithm>

void Slider::SetRange(float32 minValue, float32 maxValue)
{
    m_MinValue = minValue;
    m_MaxValue = maxValue;

    SetValue(m_Value, false);
}

void Slider::SetHandle(UI* handle)
{
    mp_Handle = handle;

    if (mp_Handle != nullptr && !m_HandleTextureId.empty())
    {
        mp_Handle->SetTexture(m_HandleTextureId); 
    }

    UpdateVisual();
}

void Slider::SetHandleSizeRatio(float32 ratio)
{
    m_HandleSizeRatio = ratio;
    UpdateVisual();
}

void Slider::SetHandleTexture(const std::string& id)
{
    m_HandleTextureId = id;

    if (mp_Handle != nullptr && !id.empty())
        mp_Handle->SetTexture(id);
}

void Slider::SetOrientation(SliderOrientation orientation)
{
    m_Orientation = orientation;
    UpdateVisual();
}

void Slider::SetVisualMode(SliderVisualMode mode)
{
    m_VisualMode = mode;

    if (mode == SliderVisualMode::Handle && mp_Handle == nullptr)
    {
        Scene* pScene = SceneManager::GetInstance().GetCurrentScene();

        mp_Handle = pScene->CreateUI<UI>(gcle::Shapes::Circle);
        mp_Handle->SetRenderSize(gcle::Shapes::Circle, { 25.0f });
    }

    UpdateVisual();
}

SliderVisualMode Slider::GetVisualMode() const
{
    return m_VisualMode;
}

void Slider::SetFillAnchor(SliderFillAnchor anchor)
{
    m_FillAnchor = anchor;
    UpdateVisual();
}

SliderFillAnchor Slider::GetFillAnchor() const
{
    return m_FillAnchor;
}

void Slider::SetOnValueChanged(std::function<void(float32)> callback)
{
    m_OnValueChanged = std::move(callback);
}

float32 Slider::GetValue() const
{
    return m_Value;
}

void Slider::SetValue(float32 value, bool notify)
{
    value = std::clamp(value, m_MinValue, m_MaxValue);

    if (value == m_Value)
        return;

    m_Value = value;

    UpdateVisual();

    if (notify && m_OnValueChanged)
        m_OnValueChanged(m_Value);
}

void Slider::SetPosition(float32 x, float32 y)
{
    m_AnchorPosition = { x, y };

    GameObject::SetPosition(x, y);

    UpdateVisual();
}

void Slider::SetRenderSize(gcle::Shapes shapeType, const std::vector<float32>& points)
{
    if (shapeType == gcle::Shapes::Rectangle && points.size() >= 2)
    {
        m_FullSize = { points[0], points[1] };
        m_HasFullSize = true;
    }

    GameObject::SetRenderSize(shapeType, points);

    UpdateVisual();
}

void Slider::SetActive(bool active)
{
    if (mp_Handle != nullptr)
        mp_Handle->SetActive(active);

    UI::SetActive(active);
}

void Slider::OnInitialize()
{
    
}

void Slider::Update(float32 dt)
{
    Interactable::Update(dt);

    if (mp_Handle != nullptr)
    {
        if (GetLayer() + 1 != mp_Handle->GetLayer())
            mp_Handle->SetLayer(GetLayer() + 1);
    }

    if (IsPressed())
        UpdateValueFromMouse();
     
    if (m_VisualMode == SliderVisualMode::Fill)
        UpdateVisualFill();
}

void Slider::OnPressed()
{
    UpdateValueFromMouse();
}

void Slider::UpdateValueFromMouse()
{ 
    if (m_VisualMode == SliderVisualMode::Fill)
        return;

    Vector2f mouse = GameManager::GetInstance().GetWindow()->GetMousePositionOnRenderTarget(); 

    float32 width = GetRenderShape()->GetWidth();
    float32 height = GetRenderShape()->GetHeight();
     
    Vector2f center = GetRenderPosition();
    Vector2f topLeft = { center.x - width * 0.5f, center.y - height * 0.5f };

    float32 length =
        (m_Orientation == SliderOrientation::Horizontal)
        ? width
        : height;

    float32 axis =
        (m_Orientation == SliderOrientation::Horizontal)
        ? mouse.x - topLeft.x
        : mouse.y - topLeft.y;

    float32 t = std::clamp(axis / length, 0.f, 1.f);

    SetValue(m_MinValue + t * (m_MaxValue - m_MinValue)); 
}

void Slider::UpdateVisual()
{
    if (m_VisualMode == SliderVisualMode::Fill)
    {
        UpdateVisualFill();
        return;
    }

    UpdateVisualHandle();
}

void Slider::UpdateVisualHandle()
{
    if (!mp_Handle || !GetRenderShape())
        return;

    float32 t = (m_MaxValue > m_MinValue) ? (m_Value - m_MinValue) / (m_MaxValue - m_MinValue) : 0.f;

    float32 width = GetRenderShape()->GetWidth();
    float32 height = GetRenderShape()->GetHeight();

    Vector2f center = GetPosition(); 
    Vector2f topLeft = { center.x - width * 0.5f, center.y - height * 0.5f };

    if (m_Orientation == SliderOrientation::Horizontal)
    {
        float32 handleWidth = width * m_HandleSizeRatio;
        float32 x = t * (width - handleWidth);
        float32 newX = topLeft.x + x + handleWidth * 0.5f;

        mp_Handle->SetPosition(newX, center.y);
        mp_Handle->SetRenderPosition(newX, center.y);
    }
    else
    {
        float32 handleHeight = height * m_HandleSizeRatio;
        float32 y = t * (height - handleHeight);
         
        float32 newY = topLeft.y + y + handleHeight * 0.5f;

        mp_Handle->SetPosition(center.x, newY);
        mp_Handle->SetRenderPosition(center.x, newY);
    }
}

float32 Slider::RatioForAnchor(SliderFillAnchor anchor)
{
    switch (anchor)
    {
    case SliderFillAnchor::Start:  return 0.0f;
    case SliderFillAnchor::End:    return 1.0f;
    case SliderFillAnchor::Center:
    default:                       return 0.5f;
    }
}

void Slider::UpdateVisualFill()
{
    if (!m_HasFullSize || GetRenderShape() == nullptr)
        return;

    float32 t = (m_MaxValue > m_MinValue) ? (m_Value - m_MinValue) / (m_MaxValue - m_MinValue) : 0.f;
    t = std::clamp(t, 0.f, 1.f);

    float32 fixedRatio = RatioForAnchor(m_FillAnchor);
     
    if (m_Orientation == SliderOrientation::Horizontal)
    {
        float32 newWidth = m_FullSize.x * t;

        float32 fixedWorldX = (m_AnchorPosition.x - m_FullSize.x * 0.5f) + m_FullSize.x * fixedRatio;

        GameObject::SetRenderSize(gcle::Shapes::Rectangle, { newWidth, m_FullSize.y });
        SetRenderPosition({ fixedWorldX, m_AnchorPosition.y }, fixedRatio, 0.5f);
    }
    else
    {
        float32 newHeight = m_FullSize.y * t;

        float32 fixedWorldY = (m_AnchorPosition.y - m_FullSize.y * 0.5f) + m_FullSize.y * fixedRatio;

        GameObject::SetRenderSize(gcle::Shapes::Rectangle, { m_FullSize.x, newHeight });
        SetRenderPosition({ m_AnchorPosition.x, fixedWorldY }, 0.5f, fixedRatio);
    }
}