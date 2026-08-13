#pragma once
#include "Slider.h"

class Scrollbar : public Slider
{
public:
    void SetHandleSizeRatio(float32 ratio);
    float32 GetHandleSizeRatio() const;
};