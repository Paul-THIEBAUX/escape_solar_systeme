#pragma once

#include "Engine/Camera.h"
#include "Engine/Entity.h"

namespace Demo
{
	class SmoothCamera
	{
	public:

		SmoothCamera() = default;
		~SmoothCamera() = default;

		void Initialize(Camera* pCamera, Entity* pFollowingEntity);
		void Update(float32 dt);

		void SetSmoothness(float32 smoothness);
		void SetMaxMouseOffset(float32 offset);
		void SetMouseInfluence(float32 influence);

	private:

		Camera* mp_Camera = nullptr;
		Entity* mp_FollowingEntity = nullptr;

		float32 m_Smoothness = 9.0f;
		float32 m_MouseInfluence = 0.40f;
		float32 m_MaxMouseOffset = 180.0f;
		Vector2f m_CurrentMouseOffset = { 0.0f, 0.0f };
	};
}