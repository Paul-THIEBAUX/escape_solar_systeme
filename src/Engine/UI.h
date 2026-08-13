#pragma once

#include "GameObject.h"
#include "Render/Text.h"

class UI : public GameObject
{
public:
    UI() = default;
    virtual ~UI();

protected:
    virtual void OnActivate() {}
    virtual void OnUpdate() override {}
    virtual void OnInitialize() override {}
    virtual void Update(float32 dt) override;

private:
    void Initialize(gcle::Shapes shape) override;
    void Initialize();
    void OnSetPosition(float32 x, float32 y) override;
public: 
    void SetText(const std::string& text);
    void SetTextObject(Text* pText) { mp_Text = pText; }

    void SetActive(bool active) override;

    Text* GetText() { return mp_Text; }

protected: 
    Text* mp_Text = nullptr;

private:
    friend class Scene;
    friend class GameManager;
};


