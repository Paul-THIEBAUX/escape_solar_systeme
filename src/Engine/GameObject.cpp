#include "GameObject.h"

#include <algorithm>

#include "GameManager.h"
#include "PhysicsManager.h"
#include "RessourceManager.h"
#include "SceneManager.h"

GameObject::~GameObject()
{
    if (mp_RenderShape != nullptr)
        delete mp_RenderShape;
    mp_RenderShape = nullptr;
}

void GameObject::Initialize(gcle::Shapes shape)
{
    m_ToDestroy = false;

    delete mp_RenderShape;
    mp_RenderShape = GetBaseShape(shape);
}

void GameObject::Update(float32 dt)
{
    m_Transform.UpdateChildPosition();

    Sprite* sprite = GetSprite();
    if (sprite != nullptr)
        sprite->UpdateAnimation(dt, mp_RenderShape);
}

void GameObject::SetPosition(float32 x, float32 y)
{
    m_Transform.SetPosition({ x, y });
    OnSetPosition(x, y);
}

void GameObject::SetRenderPosition(Vector2f position, float32 ratioX, float32 ratioY)
{
    SetRenderPosition(position.x, position.y, ratioX, ratioY);
}

void GameObject::SetRenderPosition(float32 x, float32 y, float32 ratioX, float32 ratioY)
{
    if (mp_RenderShape != nullptr)
        mp_RenderShape->SetPosition(x, y, ratioX, ratioY);
}

void GameObject::SetRenderSize(gcle::Shapes shape, const std::vector<float32>& points)
{
    if (mp_RenderShape == nullptr)
        return;

    switch (shape)
    {
    case gcle::Shapes::Rectangle:
        if (points.size() < 2)
            return;

        static_cast<gcle::Rectangle*>(mp_RenderShape)->SetWidth(points[0]);
        mp_RenderShape->SetHeight(points[1]);
        break;

    case gcle::Shapes::Circle:
        if (points.empty())
            return;

        mp_RenderShape->SetRadius(points[0]);
        break;

    case gcle::Shapes::Triangle:
        if (points.size() < 6)
            return;

        mp_RenderShape->SetTrianglePoints({
            { points[0], points[1] },
            { points[2], points[3] },
            { points[4], points[5] }
            });
        break;

    default:
        break;
    }
}

void GameObject::SetScale(Vector2f scale)
{
    m_Transform.SetScale(scale);
}

void GameObject::SetScale(float32 scale)
{
    SetScale({ scale, scale });
}

void GameObject::ScaleBy(Vector2f factor)
{
    const Vector2f currentScale = m_Transform.GetScale();
    m_Transform.SetScale({ currentScale.x * factor.x, currentScale.y * factor.y });
}

void GameObject::SetRotation(Degrees angle)
{
    m_Transform.SetDegAngle(angle);
}

void GameObject::Rotate(Degrees delta)
{
    Degrees angle = std::fmod(m_Transform.GetDegAngle() + delta, 360.0f);
    if (angle < 0.0f)
        angle += 360.0f;

    m_Transform.SetDegAngle(angle);
}

void GameObject::SetTexture(const std::string& id)
{
    if (mp_RenderShape == nullptr)
        return;

    RessourceManager& resourceManager = RessourceManager::GetInstance();

    if (!SceneManager::GetInstance().GetCurrentSceneTag().empty())
    {
        for (const std::string& sceneId : m_ActiveScenes)
            SceneManager::GetInstance().GetSceneWithTag(sceneId)->AddDrawnTexture(id);

        auto* surf = resourceManager.GetSurface(id);

        if (surf == nullptr) 
        {
            GCLE_WARN << "Texture inconnue : " << id; 
            return;
        }

        if (surf->mp_surface == nullptr)
        {
            const std::string path = "../../assets/textures/" + id + ".png";
            resourceManager.LoadSurface(GameManager::GetInstance().GetWindow(), path, id);
        }
    }

    mp_RenderShape->SetTexture(
        GameManager::GetInstance().GetWindow(),
        resourceManager.GetSurface(id));

    resourceManager.AddTexture(id, mp_RenderShape->GetTexture());
}

void GameObject::SetTextureFlip(TextureFlipMode mode)
{
    if (mp_RenderShape != nullptr)
        mp_RenderShape->SetFlip(mode);
}

void GameObject::SetColor(Color color)
{
    if (mp_RenderShape != nullptr)
        mp_RenderShape->SetColor(color);
}

void GameObject::SetActive(bool active)
{
    m_IsActive = active;
}

bool GameObject::IsActive() const
{
    return m_IsActive;
}

void GameObject::SetTag(int32 tag)
{
    m_Tag = tag;
}

bool GameObject::IsTag(int32 tag) const
{
    return m_Tag == tag;
}

void GameObject::SetLayer(int32 layer)
{
    m_Layer = std::clamp(layer, 0, 15);
}

void GameObject::SetDebugLayer(int32 layer)
{
    m_Layer = std::clamp(layer, 0, 31);
}

void GameObject::AddAnimation(
    const std::string& id,
    int32 firstFrame,
    int32 lastFrame,
    int32 line,
    int32 tileWidth,
    int32 tileHeight,
    float32 duration)
{
    Sprite* sprite = GetSprite();
    if (sprite == nullptr)
    {
        GCLE_WARN << "GameObject doesn't have a sprite texture. Set a texture before adding an animation." << ENDL;
        return;
    }

    sprite->AddAnimation(id, firstFrame, lastFrame, line, tileWidth, tileHeight, duration);
}

void GameObject::PlayAnimation(
    const std::string& id,
    AnimationMode mode,
    AnimationInterrupt interrupt)
{
    Sprite* sprite = GetSprite();
    if (sprite == nullptr)
    {
        GCLE_WARN << "GameObject doesn't have a sprite texture. Set a texture before playing an animation." << ENDL;
        return;
    }

    sprite->PlayAnimation(id, mode, interrupt);
}

void GameObject::StopAnimation()
{
    Sprite* sprite = GetSprite();
    if (sprite != nullptr)
        sprite->StopAnimation();
}

void GameObject::AddFunctionInFrame(
    const std::string& animation,
    int32 frame,
    std::function<void()> function)
{
    Sprite* sprite = GetSprite();
    if (sprite != nullptr)
        sprite->AddFunctionInFrame(animation, frame, std::move(function));
}

void GameObject::RemoveFunctionInFrame(const std::string& animation, int32 frame)
{
    Sprite* sprite = GetSprite();
    if (sprite != nullptr)
        sprite->RemoveFunctionInFrame(animation, frame);
}

Vector2f GameObject::GetPosition()
{
    return  m_Transform.GetPosition();
}

Vector2f GameObject::GetRenderPosition()
{
    return mp_RenderShape != nullptr ? mp_RenderShape->GetPosition() : Vector2f{};
}

Vector2f GameObject::GetScale()
{
    return m_Transform.GetScale();
}

Degrees GameObject::GetRotation()
{
    return m_Transform.GetDegAngle();
}

int32 GameObject::GetLayer() const
{
    return m_Layer;
}

Color GameObject::GetColor() const
{
    return mp_RenderShape != nullptr ? mp_RenderShape->GetColor() : Color{};
}

Transform2D& GameObject::GetTransform2D()
{
    return m_Transform;
}

const Transform2D& GameObject::GetTransform2D() const
{
    return m_Transform;
}

gcle::Shape* GameObject::GetRenderShape()
{
    return mp_RenderShape;
}

const gcle::Shape* GameObject::GetRenderShape() const
{
    return mp_RenderShape;
}

const std::string& GameObject::GetCurrentAnimation() const
{
    static const std::string emptyAnimation;

    const Sprite* sprite = GetSprite();
    return sprite != nullptr ? sprite->GetCurrentAnimation() : emptyAnimation;
}

bool GameObject::CanInterruptCurrentAnimation(AnimationInterrupt interrupt) const
{
    const Sprite* sprite = GetSprite();
    return sprite != nullptr && sprite->CanInterruptCurrentAnimation(interrupt);
}

bool GameObject::IsAnimationAtStart() const
{
    const Sprite* sprite = GetSprite();
    return sprite != nullptr && sprite->IsAnimationAtStart();
}

bool GameObject::IsAnimationAtEnd() const
{
    const Sprite* sprite = GetSprite();
    return sprite != nullptr && sprite->IsAnimationAtEnd();
}

bool GameObject::IsInside(Vector2f position) const
{
    return mp_RenderShape != nullptr &&
        PhysicsManager::GetInstance().IsInside(mp_RenderShape, position);
}

bool GameObject::ToDestroy() const
{
    return m_ToDestroy;
}

void GameObject::AddActiveScene(const std::string& sceneTag)
{
    if (std::find(m_ActiveScenes.begin(), m_ActiveScenes.end(), sceneTag) != m_ActiveScenes.end())
        return;

    m_ActiveScenes.push_back(sceneTag);

    if (mp_RenderShape != nullptr && mp_RenderShape->GetTexture() != nullptr)
    {
        SceneManager::GetInstance()
            .GetSceneWithTag(sceneTag)
            ->AddDrawnTexture(mp_RenderShape->GetTexture()->GetId());
    }
}

void GameObject::RemoveActiveScene(const std::string& sceneTag)
{
    const auto it = std::find(m_ActiveScenes.begin(), m_ActiveScenes.end(), sceneTag);
    if (it != m_ActiveScenes.end())
        m_ActiveScenes.erase(it);
}

bool GameObject::IsActiveIn(const std::string& sceneTag) const
{
    return std::find(m_ActiveScenes.begin(), m_ActiveScenes.end(), sceneTag) != m_ActiveScenes.end();
}

bool GameObject::HasActiveScenes() const
{
    return !m_ActiveScenes.empty();
}

gcle::Shape* GameObject::GetBaseShape(gcle::Shapes shape)
{
    switch (shape)
    {
    case gcle::Shapes::Rectangle:
        return GCLE_NEW gcle::Rectangle(
            0.0f, 0.0f, 100.0f, 100.0f,
            Color{ 255, 255, 255, 255 }, this);

    case gcle::Shapes::Circle:
        return GCLE_NEW gcle::Circle(
            0.0f, 0.0f, 100.0f, 32,
            Color{ 255, 255, 255, 255 }, this);

    case gcle::Shapes::Triangle:
        return GCLE_NEW gcle::Triangle(
            0.0f, 0.0f,
            0.0f, 100.0f,
            100.0f, 100.0f,
            Color{ 255, 255, 255, 255 }, this);

    case gcle::Shapes::Count:
    default:
        return nullptr;
    }
}

Sprite* GameObject::GetSprite()
{
    if (mp_RenderShape == nullptr)
        return nullptr;

    Texture* texture = mp_RenderShape->GetTexture();
    if (texture == nullptr || !texture->IsSprite())
        return nullptr;

    return static_cast<Sprite*>(texture);
}

const Sprite* GameObject::GetSprite() const
{
    if (mp_RenderShape == nullptr)
        return nullptr;

    Texture* texture = mp_RenderShape->GetTexture();
    if (texture == nullptr || !texture->IsSprite())
        return nullptr;

    return static_cast<const Sprite*>(texture);
}
