#include "InputManager.h"

InputManager* InputManager::s_Instance = nullptr;

InputManager& InputManager::GetInstance()
{
	if (s_Instance == nullptr)
		s_Instance = GCLE_NEW InputManager();
	return *s_Instance;
}

void InputManager::DestroyInstance()
{
	delete s_Instance;
	s_Instance = nullptr;
}

void InputManager::Update()
{
	m_Previous = m_Current;

	for (int i = 0; i < 256; ++i)
	{
		m_Current[i] = (GetAsyncKeyState(i) & 0x8000) != 0;
	}

	for (int8 i = 0; i < m_Controllers.size(); i++)
	{
		m_Controllers[i]->state = GetGamepadState(i);
	}

	ResetScroll();
} 

Vector2<int32> InputManager::GetMouseRelativePosition()
{
	POINT pos;

	GetCursorPos(&pos);
	return { pos.x, pos.y };
}

//TO DO
Vector2<int32> InputManager::GetMouseWorldPosition()
{
	return Vector2<int32>();
}

void InputManager::ResetScroll()
{
	m_ScrollDeltaX = 0;
	m_ScrollDeltaY = 0;
}

void InputManager::OnMouseWheel(int32 deltaX, int32 deltaY)
{
	m_ScrollDeltaX += deltaX;
	m_ScrollDeltaY += deltaY;
}
 
bool InputManager::IsDown(int16 key)
{
	return m_Current[key] && !m_Previous[key];
}

bool InputManager::IsReleased(int16 key)
{
	return !m_Current[key] && m_Previous[key];
}

bool InputManager::IsHeld(int16 key)
{
	return m_Current[key];
}

bool InputManager::IsUp(int16 key)
{
	return !m_Current[key];
}

void InputManager::AddController(Controller* pController)
{
	m_Controllers.push_back(pController);
}



bool InputManager::IsControllerDown(int8 controller, int16 key)
{ 
	XINPUT_STATE state;
	if (!XInputGetState(controller, &state) == ERROR_SUCCESS)
		return false;
	 
	if (state.Gamepad.wButtons & key)
	{
		return true;
	}

	return false;
}

void InputManager::SetVibration(int8 controller, float32 powerLeft, float32 powerRight)
{
	XINPUT_VIBRATION VibrationState;

	ZeroMemory(&VibrationState, sizeof(XINPUT_VIBRATION));

	int iLeftMotor = int(powerLeft * 65535.0f);
	int iRightMotor = int(powerRight * 65535.0f);

	VibrationState.wLeftMotorSpeed = iLeftMotor;
	VibrationState.wRightMotorSpeed = iRightMotor;

	XInputSetState(controller, &VibrationState);
}





XINPUT_STATE InputManager::GetGamepadState(int8 controller)
{
	XINPUT_STATE GamepadState;

	ZeroMemory(&GamepadState, sizeof(XINPUT_STATE));

	XInputGetState(controller, &GamepadState);

	return GamepadState;
}



void InputManager::SetLeftStickDeadZone(int8 controller, float32 Xpos, float32 Xneg, float32 Ypos, float32 Yneg)
{
	m_Controllers[controller]->leftStickDeadzone = { static_cast<int16>(Xpos * 32767.0f), static_cast<int16>(Xneg * 32768.0f), static_cast<int16>(Ypos * 32767.0f), static_cast<int16>(Yneg * 32768.0f) };
}

void InputManager::SetRightStickDeadZone(int8 controller, float32 Xpos, float32 Xneg, float32 Ypos, float32 Yneg)
{
	m_Controllers[controller]->rightStickDeadzone = { static_cast<int16>(Xpos * 32767.0f), static_cast<int16>(Xneg * 32768.0f), static_cast<int16>(Ypos * 32767.0f), static_cast<int16>(Yneg * 32768.0f) };
}

std::vector<int16> InputManager::GetLeftStickDeadZone(int8 controller)
{
	return m_Controllers[controller]->leftStickDeadzone;
}

std::vector<int16> InputManager::GetRightStickDeadZone(int8 controller)
{
	return m_Controllers[controller]->rightStickDeadzone;
}



bool InputManager::IsLeftStickInHorizontalDeadzone(int8 controller)
{
	int16 sX = m_Controllers[controller]->state.Gamepad.sThumbLX;

	if ((sX <= m_Controllers[controller]->leftStickDeadzone[0]) and (sX >= m_Controllers[controller]->leftStickDeadzone[1]))
	{
		return true;
	}

	return false;
}

bool InputManager::IsLeftStickInVerticalDeadzone(int8 controller)
{
	int16 sY = m_Controllers[controller]->state.Gamepad.sThumbLY;

	if ((sY <= m_Controllers[controller]->leftStickDeadzone[2]) and (sY >= m_Controllers[controller]->leftStickDeadzone[3]))
	{
		return true;
	}

	return false;
}

bool InputManager::IsRightStickInHorizontalDeadzone(int8 controller)
{
	int16 sX = m_Controllers[controller]->state.Gamepad.sThumbRX;

	if ((sX <= m_Controllers[controller]->rightStickDeadzone[0]) and (sX >= m_Controllers[controller]->rightStickDeadzone[1]))
	{
		return true;
	}

	return false;
}

bool InputManager::IsRightStickInVerticalDeadzone(int8 controller)
{
	int16 sY = m_Controllers[controller]->state.Gamepad.sThumbRY;

	if ((sY <= m_Controllers[controller]->rightStickDeadzone[2]) and (sY >= m_Controllers[controller]->rightStickDeadzone[3]))
	{
		return true;
	}

	return false;
}



float32 InputManager::GetLeftStickX(int8 controller)
{
	if (m_Controllers[controller]->state.Gamepad.sThumbLX < 0)
	{
		return static_cast<float32>(m_Controllers[controller]->state.Gamepad.sThumbLX / 32768.0f);
	}

	return (static_cast<float32>(m_Controllers[controller]->state.Gamepad.sThumbLX) / 32767.0f);
}

float32 InputManager::GetLeftStickY(int8 controller)
{
	if (m_Controllers[controller]->state.Gamepad.sThumbLY < 0)
	{
		return static_cast<float32>(m_Controllers[controller]->state.Gamepad.sThumbLY / 32768.0f);
	}

	return (static_cast<float32>(m_Controllers[controller]->state.Gamepad.sThumbLY) / 32767.0f);
}

float32 InputManager::GetRightStickX(int8 controller)
{
	if (m_Controllers[controller]->state.Gamepad.sThumbRX < 0)
	{
		return static_cast<float32>(m_Controllers[controller]->state.Gamepad.sThumbRX / 32768.0f);
	}

	return (static_cast<float32>(m_Controllers[controller]->state.Gamepad.sThumbRX) / 32767.0f);
}

float32 InputManager::GetRightStickY(int8 controller)
{
	if (m_Controllers[controller]->state.Gamepad.sThumbRY < 0)
	{
		return static_cast<float32>(m_Controllers[controller]->state.Gamepad.sThumbRY / 32768.0f);
	}

	return (static_cast<float32>(m_Controllers[controller]->state.Gamepad.sThumbRY) / 32767.0f);
}



Vector2f InputManager::LeftStickPressed(int8 controller)
{
	float32 sX = static_cast<float32>(m_Controllers[controller]->state.Gamepad.sThumbLX);
	float32 sY = static_cast<float32>(m_Controllers[controller]->state.Gamepad.sThumbLY);

	if (IsLeftStickInHorizontalDeadzone(controller) == true)
	{
		sX = 0;
	}

	if (IsLeftStickInVerticalDeadzone(controller) == true)
	{
		sY = 0;
	}

	if (sX < 0)
	{
		sX /= 32768.0f;
	}

	else
	{
		sX /= 32767.0f;
	}

	if (sY < 0)
	{
		sY /= 32768.0f;
	}

	else
	{
		sY /= 32767.0f;
	}

	return { sX, sY };
}

Vector2f InputManager::RightStickPressed(int8 controller)
{
	float32 sX = static_cast<float32>(m_Controllers[controller]->state.Gamepad.sThumbRX);
	float32 sY = static_cast<float32>(m_Controllers[controller]->state.Gamepad.sThumbRY);

	if (IsRightStickInHorizontalDeadzone(controller) == true)
	{
		sX = 0;
	}

	if (IsRightStickInVerticalDeadzone(controller) == true)
	{
		sY = 0;
	}

	if (sX < 0)
	{
		sX /= 32768.0f;
	}

	else
	{
		sX /= 32767.0f;
	}

	if (sY < 0)
	{
		sY /= 32768.0f;
	}

	else
	{
		sY /= 32767.0f;
	}

	return { sX, sY };
}

float32 InputManager::LeftTriggerPressed(int8 controller)
{
	BYTE Trigger = m_Controllers[controller]->state.Gamepad.bLeftTrigger;

	if (Trigger > m_Controllers[controller]->leftTriggerDeadzone)
	{
		return Trigger / 255.0f;
	}

	return 0.0f;
}

float32 InputManager::RightTriggerPressed(int8 controller)
{
	

	BYTE Trigger = m_Controllers[controller]->state.Gamepad.bRightTrigger;

	if (Trigger > m_Controllers[controller]->rightTriggerDeadzone)
	{
		return Trigger / 255.0f;
	}

	return 0.0f;
}

