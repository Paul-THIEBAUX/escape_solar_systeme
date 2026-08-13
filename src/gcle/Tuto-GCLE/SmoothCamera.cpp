#include "SmoothCamera.h"

#include <cmath>
#include <algorithm>


namespace Demo
{

	void SmoothCamera::Initialize(Camera* pCamera, Entity* pFollowingEntity)
	{
		mp_Camera = pCamera;
		mp_FollowingEntity = pFollowingEntity;


		if (mp_Camera && mp_FollowingEntity)
		{
			mp_Camera->SetPosition(
				mp_FollowingEntity->GetPosition()
			);
		}
	}


	void SmoothCamera::Update(float32 dt)
	{
		if (!mp_Camera || !mp_FollowingEntity)
			return;
		 
		Vector2f playerPosition = mp_FollowingEntity->GetPosition();
		 
		constexpr float32 SCREEN_WIDTH = RENDER_TARGET_WIDTH;
		constexpr float32 SCREEN_HEIGHT = RENDER_TARGET_HEIGHT;
		 
		Vector2f screenCenter =
		{
			SCREEN_WIDTH * 0.5f,
			SCREEN_HEIGHT * 0.5f
		};
		 
		Vector2f mousePosition = mp_Camera->GetScreenMousePosition();
		 
		Vector2f mouseOffset = mousePosition - screenCenter;
		 
		float32 mouseDistance = mouseOffset.Norm();
		 
		if (mouseDistance > m_MaxMouseOffset)
		{
			mouseOffset = (mouseOffset / mouseDistance) * m_MaxMouseOffset;
		}
		 
		mouseOffset *= m_MouseInfluence; 

		float32 mouseT = 1.0f - std::exp(-15.0f * dt);
		 
		m_CurrentMouseOffset += (mouseOffset - m_CurrentMouseOffset) * mouseT;
		 
		Vector2f targetPosition = playerPosition + m_CurrentMouseOffset; 

		Vector2f currentPosition = mp_Camera->GetPosition();


		float32 cameraT = 1.0f - std::exp(-m_Smoothness * dt);



		currentPosition += (targetPosition - currentPosition) * cameraT;
		 
		mp_Camera->SetPosition(currentPosition);
	}
	 
	void SmoothCamera::SetSmoothness(float32 smoothness)
	{
		m_Smoothness = smoothness;
	}
	 
	void SmoothCamera::SetMouseInfluence(float32 influence)
	{
		m_MouseInfluence = influence;
	} 

	void SmoothCamera::SetMaxMouseOffset(float32 offset)
	{
		m_MaxMouseOffset = offset;
	}

}