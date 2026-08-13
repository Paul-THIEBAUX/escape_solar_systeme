#pragma once
#include "Entity.h"
#include "Render/Window.h"
#include "Vector2.hpp"
#include "Transform.h"
#include "Timer.h"

#include <vector>


class Camera
{
	public:
		~Camera();
		void Init(Window* pWindow);

		void SetFollowing(Entity* pNewEntity);
		Entity* GetFollowing();

		void SetPosition(Vector2f v);
		Vector2f GetPosition();
		 
		void SetZoom(float32 zoom);
		float32 GetZoom() const;

		Vector2f GetScreenMousePosition();
		Vector2f GetMouseScreenToWorldPosition();

		void SetActive(bool isActive);
		bool IsActive() const;

		uint64 GetId() const;

		void AddActiveScene(const std::string& sceneTag);
		void RemoveActiveScene(const std::string& sceneTag);
		bool IsActiveIn(const std::string& sceneTag);
		bool HasActiveScenes() const;

private:
		void Update(Clock& time, std::vector<std::vector<Entity*>>& entities);

private:
	Window*			mp_Window = nullptr;
	Vector2f		m_ScreenMiddle;
	float32			m_Zoom = 1;
	Transform2D		m_Transform;
	Entity*			mp_FollowingEntity = nullptr;

	uint64			m_Id = 0;

	std::vector<std::string> m_ActiveScenes;

	bool m_IsActive = false;

private: 
	friend class GameManager;
};

