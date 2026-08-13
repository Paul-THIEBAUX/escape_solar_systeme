#pragma once
#include "UI.h"

class Image : public UI
{
public:
    void SetSprite(const std::string& id);
    void SetTint(Color color);
};