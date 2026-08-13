#pragma once
#include "Texture.h"
#include "Core/include.h"
#include <unordered_map>
#include <functional>

namespace gcle
{
	class Shape;
}
class Window;

struct Animation
{
	int32 firstFrame = 0;
	int32 lastFrame = 0;

	int32 line = 0;

	int32 tileW = 0;
	int32 tileH = 0;
	
	float32 duration = 0;


	Animation(int32 firstFrame, int32 lastFrame, int32 line, int32 tileWidth, int32 tileHeight, float32 duration = 0.5f) :
		firstFrame(firstFrame),
		lastFrame(lastFrame),
		line(line),
		tileW(tileWidth),
		tileH(tileHeight),
		duration(duration)
	{}
	std::unordered_map<int, std::function<void()>> m_animationFunction;
	int m_frameId = 0;

};

// Mode : 
// Loop = infinite loop
// IgnoreIfAlreadyPlaying = If the same animation is already playing don't override it and let it end 
// Reverse = Play the animation in the reverse order
// Lock = Animation can't be cancel unless Animation Interrupt is set to Force
enum class AnimationMode : uint8
{
	None = 0,
	Loop = 1 << 0,
	IgnoreIfAlreadyPlaying = 1 << 1,
	Reverse = 1 << 2,
	Lock = 1 << 3
};

// Mode : 
// Normal = Animation can be interrupt unless the AnimationMode is Lock 
// Override the AnimationMode lock
enum class AnimationInterrupt : uint8
{
	Normal,
	Force
};

inline AnimationMode operator|(AnimationMode a, AnimationMode b)
{
	return static_cast<AnimationMode>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

inline AnimationMode operator&(AnimationMode a, AnimationMode b)
{
	return static_cast<AnimationMode>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

inline bool HasFlag(AnimationMode value, AnimationMode flag)
{
	return (static_cast<uint8_t>(value) & static_cast<uint8_t>(flag)) != 0;
}


class Sprite : public Texture
{
private:

	int32 m_Width = 0;
	int32 m_Height = 0;

	std::unordered_map<std::string, Animation*> m_animationMap;

	Animation* mp_CurrentAnimation = nullptr;
	int32 m_CurrentFrameX = 0;
	int32 m_CurrentFrameY = 0;

	std::string m_currentAnimationId;
	float32 m_Timer = 0.f;

	AnimationMode m_Mode = AnimationMode::None;

public:
	
	~Sprite() override;
	void UpdateAnimation(float32 deltatime, gcle::Shape* pShape);

	bool IsAnimationPlaying();

	Sprite(Window* pWindow, const std::string& path);
	Sprite(Window* pWindow, Asset* pAsset);
	Sprite(Window* pWindow, Surface* pSurface);

	void AddAnimation(const std::string& id, 
		int32 firstFrame, 
		int32 lastFrame, 
		int32 line, 
		int32 tileWidth, 
		int32 tileHeight, 
		float32 duration = 0.5f);
 
	void PlayAnimation(const std::string& id, AnimationMode mode, AnimationInterrupt interrupt = AnimationInterrupt::Normal);

	void AddFunctionInFrame(const std::string& animation, int32 frame, std::function<void()> function); 
	void StopAnimation();  
	void RemoveFunctionInFrame(const std::string& animation, int32 frame);


	const std::string& GetCurrentAnimation() const;
	bool CanInterruptCurrentAnimation(AnimationInterrupt interrupt = AnimationInterrupt::Normal) const;

	bool IsAnimationAtStart() const;
	bool IsAnimationAtEnd() const;
};