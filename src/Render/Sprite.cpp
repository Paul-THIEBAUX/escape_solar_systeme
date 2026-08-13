#include "Sprite.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "Utils.h"
#include "Shape.h"

Sprite::Sprite(Window* pWindow, const std::string& path)
{
	m_isSprite = true;

	InitTexture(pWindow, path);

	if (!IsTextureInit())
		return;

	float32 texW = 0.f, texH = 0.f;
	SDL_GetTextureSize(GetSDLTexture(), &texW, &texH); 
	m_Width = static_cast<int32>(texW);
	m_Height = static_cast<int32>(texH);
}

Sprite::Sprite(Window* pWindow, Asset* pAsset)
{
	InitTextureWithBuffer(pWindow, pAsset);
}

Sprite::Sprite(Window* window, Surface* surface)
{
	InitTextureWithSurface(window, surface);
	//SDL_QueryTexture(GetSDLTexture(), NULL, NULL, &m_width, &m_height);
	m_Width = surface->GetSDL_Surface()->w;
	m_Height = surface->GetSDL_Surface()->h;
	m_isSprite = true;
}

Sprite::~Sprite()
{
	SDL_DestroyTexture(mp_texture);

	for (auto& anim : m_animationMap)
		delete anim.second;

	m_animationMap.clear();
}

void Sprite::AddAnimation(const std::string& id, int32 firstFrame, int32 lastFrame, int32 line, int32 tileWidth, int32 tileHeight, float32 duration)
{
	m_animationMap[id] = GCLE_NEW Animation(firstFrame, lastFrame, line, tileWidth, tileHeight, duration);
}


void Sprite::PlayAnimation(const std::string& id, AnimationMode mode, AnimationInterrupt interrupt)
{
	if (!m_animationMap.contains(id))
	{
		GCLE_WARN << "No animation with id : " << id << ENDL;
		return;
	}

	if (!CanInterruptCurrentAnimation(interrupt))
		return;

	if (HasFlag(mode, AnimationMode::IgnoreIfAlreadyPlaying) && m_currentAnimationId == id)
	{
		return;
	}

	m_currentAnimationId = id;
	mp_CurrentAnimation = m_animationMap[id];

	if (HasFlag(mode, AnimationMode::Reverse))
	{
		m_CurrentFrameX = mp_CurrentAnimation->lastFrame;
		mp_CurrentAnimation->m_frameId = mp_CurrentAnimation->lastFrame - mp_CurrentAnimation->firstFrame;
	}
	else
	{
		m_CurrentFrameX = mp_CurrentAnimation->firstFrame;
		mp_CurrentAnimation->m_frameId = 0;
	}

	m_CurrentFrameY = mp_CurrentAnimation->line;
	m_Timer = 0.f;
	m_Mode = mode;
}

void Sprite::StopAnimation()
{
	mp_CurrentAnimation = nullptr;
	m_CurrentFrameX = 0;
	m_CurrentFrameY = 0;
	m_Timer = 0.f;
	m_Mode = AnimationMode::None;
}

void Sprite::UpdateAnimation(float32 deltatime, gcle::Shape* pShape)
{
	Animation* anim = mp_CurrentAnimation;

	if (anim == nullptr)
		return;

	m_Timer += deltatime;
	if (m_Timer < mp_CurrentAnimation->duration)
		return;

	m_Timer = 0.f;

	int32 direction = HasFlag(m_Mode, AnimationMode::Reverse) ? -1 : 1;

	m_CurrentFrameX += direction;
	anim->m_frameId += direction;
	 
	if (!HasFlag(m_Mode, AnimationMode::Reverse))
	{
		if (m_CurrentFrameX > anim->lastFrame)
		{
			if (!HasFlag(m_Mode, AnimationMode::Loop))
			{
				StopAnimation();
				return;
			}

			m_CurrentFrameX = anim->firstFrame;
			anim->m_frameId = 0;
		}
	}
	else
	{
		if (m_CurrentFrameX < anim->firstFrame)
		{
			if (!HasFlag(m_Mode, AnimationMode::Loop))
			{
				StopAnimation();
				return;
			}

			m_CurrentFrameX = anim->lastFrame;
			anim->m_frameId = anim->lastFrame - anim->firstFrame;
		}
	}


	int32 x = m_CurrentFrameX * anim->tileW;
	int32 y = m_CurrentFrameY * anim->tileH;

	if (anim->m_animationFunction.find(anim->m_frameId) != anim->m_animationFunction.end()) {
		anim->m_animationFunction[anim->m_frameId]();
	}

	pShape->SetTextureRect(x, y, anim->tileW, anim->tileH, m_Width, m_Height);
}

bool Sprite::IsAnimationPlaying()
{
	return mp_CurrentAnimation == nullptr ? false : true;
}


void Sprite::AddFunctionInFrame(const std::string& animation, int32 frame, std::function<void()> function) {
	auto it = m_animationMap.find(animation);
	if (it == m_animationMap.end())
		return;

	Animation* anim = m_animationMap[animation];
	if (anim->m_animationFunction.find(frame) != anim->m_animationFunction.end())
		return;
	anim->m_animationFunction[frame] = function;
}

void Sprite::RemoveFunctionInFrame(const std::string& animation, int32 frame) {
	auto it = m_animationMap.find(animation);
	if (it == m_animationMap.end())
		return;

	Animation* anim = it->second;

	auto itFunction = anim->m_animationFunction.find(frame);
	if (itFunction != anim->m_animationFunction.end()) {
		anim->m_animationFunction.erase(itFunction);
	}
}

const std::string& Sprite::GetCurrentAnimation() const
{
	return m_currentAnimationId;
}

bool Sprite::CanInterruptCurrentAnimation(AnimationInterrupt interrupt) const
{
	if (mp_CurrentAnimation == nullptr)
		return true;

	if (interrupt == AnimationInterrupt::Force)
		return true;

	return !HasFlag(m_Mode, AnimationMode::Lock);
}

bool Sprite::IsAnimationAtStart() const
{
	if (mp_CurrentAnimation == nullptr)
		return false;

	return HasFlag(m_Mode, AnimationMode::Reverse) ? m_CurrentFrameX == mp_CurrentAnimation->lastFrame : m_CurrentFrameX == mp_CurrentAnimation->firstFrame;
}

bool Sprite::IsAnimationAtEnd() const
{
	if (mp_CurrentAnimation == nullptr)
		return false;

	return HasFlag(m_Mode, AnimationMode::Reverse) ? m_CurrentFrameX == mp_CurrentAnimation->firstFrame : m_CurrentFrameX == mp_CurrentAnimation->lastFrame;
}