#include "GameManager.h"
#include "Camera.h"
#include "WorldText.h"

static uint64 sId = 0;

Camera::~Camera()
{
	mp_Window = nullptr;
	mp_FollowingEntity = nullptr;
}

void Camera::Init(Window* pWindow)
{
	AddActiveScene(SceneManager::GetInstance().GetCurrentSceneTag());

	m_Transform.Initialize({ 0.0f, 0.0f }, 0); 

	mp_Window = pWindow;

	m_ScreenMiddle = { RENDER_TARGET_WIDTH / 2.f, RENDER_TARGET_HEIGHT / 2.f };

	m_Id = sId++;
}

void Camera::SetFollowing(Entity* pNewEntity)
{
	mp_FollowingEntity = pNewEntity;
}

Entity* Camera::GetFollowing()
{
	return mp_FollowingEntity;
} 

void Camera::SetPosition(Vector2f v)
{
	m_Transform.SetPosition(v);
}

Vector2f Camera::GetPosition()
{
	return m_Transform.GetPosition();
} 

void Camera::Update(Clock& time, std::vector<std::vector<Entity*>>& entities)
{
	if (mp_FollowingEntity != nullptr)
		m_Transform.SetPosition(mp_FollowingEntity->GetPosition());

	m_ScreenMiddle = Vector2f{ RENDER_TARGET_WIDTH, RENDER_TARGET_HEIGHT } *0.5f;

	for (auto& layer : entities)
	{
		for (Entity* entity : layer)
		{
			if (entity->IsActiveIn(SceneManager::GetInstance().GetCurrentSceneTag())) {
				if (entity->GetRenderShape() != nullptr) {
					Vector2f offset = (entity->GetPosition() - GetPosition()) * GetZoom();
					entity->SetRenderPosition(offset + m_ScreenMiddle);

					Vector2f realScale = entity->GetScale();
					entity->GetRenderShape()->SetScale({ realScale.x * static_cast<float32>(GetZoom()), realScale.y * static_cast<float32>(GetZoom()) });
					entity->GetRenderShape()->SetRotation(entity->GetRotation());
				}
				else if (entity->IsWorldText()) {
					WorldText* text = static_cast<WorldText*>(entity);
					Vector2f offset = (text->GetPosition() - GetPosition()) * GetZoom();
					text->SetRenderPosition(offset + m_ScreenMiddle);
				}
			}

		}
	}
}

void Camera::SetZoom(float32 zoom)
{
	m_Zoom = zoom;
}

float32 Camera::GetZoom() const
{
	return m_Zoom;
}

Vector2f Camera::GetScreenMousePosition()
{
	return GameManager::GetInstance().GetWindow()->GetMousePositionOnRenderTarget();
}

Vector2f Camera::GetMouseScreenToWorldPosition()
{  
	Vector2f mousePosOnTarget = mp_Window->GetMousePositionOnRenderTarget();
	Vector2f screenCenter = Vector2f{ RENDER_TARGET_WIDTH, RENDER_TARGET_HEIGHT } *0.5f;

	return m_Transform.GetPosition() + (mousePosOnTarget - screenCenter) / m_Zoom;
} 

void Camera::SetActive(bool isActive)
{
	m_IsActive = isActive;
}

bool Camera::IsActive() const
{
	return m_IsActive;
}

uint64 Camera::GetId() const
{
	return m_Id;
}

void Camera::AddActiveScene(const std::string& sceneTag) {
	if (std::find(m_ActiveScenes.begin(), m_ActiveScenes.end(), sceneTag) != m_ActiveScenes.end()) {
		std::cerr << sceneTag << "exists" << std::endl;
		return;
	}

	m_ActiveScenes.push_back(sceneTag);
	SceneManager::GetInstance().GetSceneWithTag(sceneTag)->AddActiveCamera(this);
}

void Camera::RemoveActiveScene(const std::string& sceneTag) {
	std::vector<std::string>::iterator it = std::find(m_ActiveScenes.begin(), m_ActiveScenes.end(), sceneTag);
	if (it == m_ActiveScenes.end()) {
		std::cerr << sceneTag << " doesn't exists " << std::endl;
		return;
	}
	m_ActiveScenes.erase(it);
}

bool Camera::IsActiveIn(const std::string& sceneTag) {
	return (std::find(m_ActiveScenes.begin(), m_ActiveScenes.end(), sceneTag) != m_ActiveScenes.end());
}

bool Camera::HasActiveScenes() const
{
	return !m_ActiveScenes.empty();
}