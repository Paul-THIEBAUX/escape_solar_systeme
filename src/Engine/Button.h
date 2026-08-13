#pragma once
#include "Interactable.h"

class Button : public Interactable
{
public:
    Button() = default;
    ~Button() = default;

    void SetFunctions(std::vector<std::function<void()>> functions) { m_FunctionToCallOnActivate = functions; }
    void AddFunction(std::function<void()> function) { m_FunctionToCallOnActivate.push_back(function); }

    virtual void OnClick() {}

protected:
    void OnReleased(bool insideOnRelease) override;

private:
    std::vector<std::function<void()>> m_FunctionToCallOnActivate;
};