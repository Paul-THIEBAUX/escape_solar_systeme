#pragma once
#include <iostream>
#include <array>
#include "include.h"
#include <Windows.h>
#include <Xinput.h>

#pragma comment(lib, "Xinput.lib")

namespace Mouse
{
	#define LeftButton VK_LBUTTON
	#define RightButton VK_RBUTTON
	#define MiddleButton VK_MBUTTON
	#define Button1 VK_XBUTTON1
	#define Button2 VK_XBUTTON2
}

namespace Keyboard
{
	#define Space VK_SPACE
	#define Shift VK_SHIFT
	#define Escape 0x1B
	#define Tab 0x09
	#define Backspace 0x08

	#define LeftArrow VK_LEFT
	#define RightArrow VK_RIGHT
	#define UpArrow VK_UP
	#define DownArrow VK_DOWN

	#define Delete VK_DELETE

	#define F1 VK_F1
	#define F2 VK_F2
	#define F3 VK_F3
	#define F4 VK_F4
	#define F5 VK_F5
	#define F6 VK_F6
	#define F7 VK_F7
	#define F8 VK_F8
	#define F9 VK_F9
	#define F10 VK_F10
	#define F11 VK_F11
	#define F12 VK_F12
}



#define XBOX_START XINPUT_GAMEPAD_START
#define XBOX_BACK XINPUT_GAMEPAD_BACK

#define XBOX_PAD_UP XINPUT_GAMEPAD_DPAD_UP
#define XBOX_PAD_LEFT XINPUT_GAMEPAD_DPAD_LEFT
#define XBOX_PAD_RIGHT XINPUT_GAMEPAD_DPAD_RIGHT
#define XBOX_PAD_DOWN XINPUT_GAMEPAD_DPAD_DOWN

#define XBOX_A XINPUT_GAMEPAD_A
#define XBOX_B XINPUT_GAMEPAD_B
#define XBOX_X XINPUT_GAMEPAD_X
#define XBOX_Y XINPUT_GAMEPAD_Y

#define XBOX_LJS XINPUT_GAMEPAD_LEFT_THUMB
#define XBOX_RJS XINPUT_GAMEPAD_RIGHT_THUMB
#define XBOX_LB XINPUT_GAMEPAD_LEFT_SHOULDER
#define XBOX_RB XINPUT_GAMEPAD_RIGHT_SHOULDER


struct Controller
{
	std::vector<int16> leftStickDeadzone = { 4000, -4000, 4000, -4000 };
	std::vector<int16> rightStickDeadzone = { 4000, -4000, 4000, -4000 };

	BYTE leftTriggerDeadzone = 30;
	BYTE rightTriggerDeadzone = 30;

	XINPUT_STATE state;
};

 
class InputManager
{
public:

	static InputManager& GetInstance();
	static void DestroyInstance();

	void Update(); 

	//Constructor 
	InputManager() = default;

	//Destructor 
	~InputManager() = default;


	//Input tests 
	Vector2<int32> GetMouseRelativePosition();
	Vector2<int32> GetMouseWorldPosition();
	void ResetScroll();
	void OnMouseWheel(int32 deltaX, int32 deltaY);
	int32 GetScrollDeltaX() const { return m_ScrollDeltaX; }
	int32 GetScrollDeltaY() const { return m_ScrollDeltaY; }
	bool IsScrollingUp() const { return m_ScrollDeltaY > 0; }
	bool IsScrollingDown() const { return m_ScrollDeltaY < 0; }



	bool IsDown(const int16 key);
	bool IsReleased(int16 key);
	bool IsHeld(int16 key);
	bool IsUp(int16 key);


	void AddController(Controller* pConntroller);
	bool IsControllerDown(int8 controller, int16 key);
	void SetVibration(int8 controller, float32 powerLeft, float32 powerRight);

	XINPUT_STATE GetGamepadState(int8 controller); 

	void SetLeftStickDeadZone(int8 controller, float32 Xpos, float32 Xneg, float32 Ypos, float32 Yneg);
	void SetRightStickDeadZone(int8 controller, float32 Xpos, float32 Xneg, float32 Ypos, float32 Yneg);
	std::vector<int16> GetLeftStickDeadZone(int8 controller);
	std::vector<int16> GetRightStickDeadZone(int8 controller);

	bool IsLeftStickInHorizontalDeadzone(int8 controller);
	bool IsLeftStickInVerticalDeadzone(int8 controller);
	bool IsRightStickInHorizontalDeadzone(int8 controller);
	bool IsRightStickInVerticalDeadzone(int8 controller);

	float32 GetLeftStickX(int8 controller);
	float32 GetLeftStickY(int8 controller);
	float32 GetRightStickX(int8 controller);
	float32 GetRightStickY(int8 controller);

	Vector2f LeftStickPressed(int8 controller);
	Vector2f RightStickPressed(int8 controller);


	float32 LeftTriggerPressed(int8 controller);
	float32 RightTriggerPressed(int8 controller);

private:

	int32 m_ScrollDeltaX = 0;
	int32 m_ScrollDeltaY = 0;

	std::array<bool, 256> m_Current{};
	std::array<bool, 256> m_Previous{};

	std::vector<Controller*> m_Controllers;

private:
	static InputManager* s_Instance;

};

