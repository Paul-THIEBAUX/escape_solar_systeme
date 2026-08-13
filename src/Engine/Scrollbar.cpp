#include "Scrollbar.h"

void Scrollbar::SetHandleSizeRatio(float32 ratio)
{
    m_HandleSizeRatio = std::clamp(ratio, 0.05f, 1.0f);
}

float32 Scrollbar::GetHandleSizeRatio() const
{
    return m_HandleSizeRatio;
}
