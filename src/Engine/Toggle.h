#pragma once 
#include "Interactable.h"
#include <functional>

class Toggle : public Interactable
{
public:
    void SetOn(bool isOn, bool notify = true);
    bool IsOn() const;

    void SetOnCheckedTexture(const std::string& id);
    void SetOnUncheckedTexture(const std::string& id);

    void SetOnValueChanged(std::function<void(bool)> callback);

protected:
    void OnReleased(bool insideOnRelease) override;

private:
    bool m_IsOn = false;
    std::string m_CheckedTexId = "0";
    std::string m_UncheckedTexId = "0";
    std::function<void(bool)> m_OnValueChanged;
};