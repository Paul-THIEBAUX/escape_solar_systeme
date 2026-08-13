#include "Button.h"

 
void Button::OnReleased(bool insideOnRelease)
{
	if (!insideOnRelease) return;

	for (auto& fn : m_FunctionToCallOnActivate) 
		fn();
	
	OnClick();
}
