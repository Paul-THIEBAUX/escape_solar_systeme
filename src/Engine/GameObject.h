#pragma once

#include <cmath>
#include <functional>
#include <string>
#include <vector>

#include "include.h"
#include "Render/Shape.h"
#include "Render/Sprite.h"
#include "Render/Texture.h"
#include "Transform.h"

class GameObject
{
public:
    GameObject() = default;
    virtual ~GameObject();

    void SetPosition(float32 x, float32 y);
    void SetRenderPosition(Vector2f position, float32 ratioX = 0.5f, float32 ratioY = 0.5f);
    void SetRenderPosition(float32 x, float32 y, float32 ratioX = 0.5f, float32 ratioY = 0.5f);
    void SetRenderSize(gcle::Shapes shape, const std::vector<float32>& points);

    void SetScale(Vector2f scale);
    void SetScale(float32 scale);
    void ScaleBy(Vector2f factor);

    void SetRotation(Degrees angle);
    void Rotate(Degrees delta);

    void SetTexture(const std::string& id);
    void SetTextureFlip(TextureFlipMode mode);
    void SetColor(Color color);

    
    bool IsActive() const;

    void SetTag(int32 tag);
    bool IsTag(int32 tag) const;

    void SetLayer(int32 layer);

    void AddAnimation(
        const std::string& id,
        int32 firstFrame,
        int32 lastFrame,
        int32 line,
        int32 tileWidth,
        int32 tileHeight,
        float32 duration = 0.5f);

    void PlayAnimation(
        const std::string& id,
        AnimationMode mode = AnimationMode::IgnoreIfAlreadyPlaying,
        AnimationInterrupt interrupt = AnimationInterrupt::Normal);

    void StopAnimation();
    void AddFunctionInFrame(const std::string& animation, int32 frame, std::function<void()> function);
    void RemoveFunctionInFrame(const std::string& animation, int32 frame);

    Vector2f GetPosition();
    Vector2f GetRenderPosition();
    Vector2f GetScale();
    Degrees GetRotation();
    int32 GetLayer() const;
    Color GetColor() const;

    Transform2D& GetTransform2D();
    const Transform2D& GetTransform2D() const;

    gcle::Shape* GetRenderShape();
    const gcle::Shape* GetRenderShape() const;

    const std::string& GetCurrentAnimation() const;
    bool CanInterruptCurrentAnimation(AnimationInterrupt interrupt = AnimationInterrupt::Normal) const;
    bool IsAnimationAtStart() const;
    bool IsAnimationAtEnd() const;

    bool IsInside(Vector2f position) const;
    bool ToDestroy() const;

    void AddActiveScene(const std::string& sceneTag);
    void RemoveActiveScene(const std::string& sceneTag);
    bool IsActiveIn(const std::string& sceneTag) const;
    bool HasActiveScenes() const;

protected:
    virtual void Initialize(gcle::Shapes shape);
    virtual void Update(float32 dt);
    virtual void SetActive(bool active);

    virtual void OnInitialize() {}
    virtual void OnUpdate() {}
    virtual void OnDestroy() {}
    virtual void OnSetPosition(float32 x, float32 y) {}

    gcle::Shape* GetBaseShape(gcle::Shapes shape);
    void SetDebugLayer(int32 layer);

protected:
    Transform2D m_Transform;
    gcle::Shape* mp_RenderShape = nullptr;
    std::vector<std::string> m_ActiveScenes;

    int64 m_Id = 0;
    bool m_ToDestroy = false;
    int32 m_Mask = 0;
    int32 m_Layer = 0;

    bool m_IsActive = true;

    int32 m_Tag = -1;

private:
    Sprite* GetSprite();
    const Sprite* GetSprite() const;

    friend class Scene;
    friend class Camera;
    friend class GameManager;
    friend class PhysicsManager;
};
