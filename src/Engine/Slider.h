#pragma once

#include "Interactable.h"
#include <functional>
#include <string>

enum class SliderOrientation
{
    Horizontal,
    Vertical
};
 

// Note: 
// Fill mode : value can only be changed by SetValue()
// Handle mode : value can be changed by both the mouse and SetValue()
enum class SliderVisualMode
{
    Handle,
    Fill
};
 
// Note: 
// Start Slider visual go down from right to left 
// Center Slider visual go from left and right to middle
// End Slider visual go down from left to right 
enum class SliderFillAnchor
{
    Start,
    Center,
    End
};

class Slider : public Interactable
{
public:
    void SetRange(float32 minValue, float32 maxValue);
    void SetValue(float32 value, bool notify = true);
    float32 GetValue() const;

    void SetOrientation(SliderOrientation orientation);

    void SetVisualMode(SliderVisualMode mode);
    SliderVisualMode GetVisualMode() const;

    void SetFillAnchor(SliderFillAnchor anchor);
    SliderFillAnchor GetFillAnchor() const;

    void SetHandle(UI* handle);
    void SetHandleSizeRatio(float32 ratio);
     
    void SetHandleTexture(const std::string& id);

    void SetOnValueChanged(std::function<void(float32)> callback);
     
    void SetPosition(float32 x, float32 y);
    void SetRenderSize(gcle::Shapes shapeType, const std::vector<float32>& points);

    void SetActive(bool active) override;

protected:
    void OnInitialize() override;
    void Update(float32 dt) override;
    void OnPressed() override;

private:
    void UpdateValueFromMouse();

    void UpdateVisual();
    void UpdateVisualHandle();
    void UpdateVisualFill();

    static float32 RatioForAnchor(SliderFillAnchor anchor);

protected:
    float32 m_MinValue = 0.f;
    float32 m_MaxValue = 1.f;
    float32 m_Value = 0.f;

    SliderOrientation m_Orientation = SliderOrientation::Horizontal;
    SliderVisualMode  m_VisualMode = SliderVisualMode::Handle;
    SliderFillAnchor  m_FillAnchor = SliderFillAnchor::Start;

    float32 m_HandleSizeRatio = 1.f;

    UI* mp_Handle = nullptr;
    std::string m_HandleTextureId;

    std::function<void(float32)> m_OnValueChanged;

private: 
    Vector2f m_AnchorPosition{ 0.f, 0.f }; 
    Vector2f m_FullSize{ 0.f, 0.f }; 
    bool     m_HasFullSize = false;
};