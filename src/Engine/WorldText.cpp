#include "WorldText.h"

void WorldText::OnInitialize(){
	m_IsWorldText = true;
}

void WorldText::OnUpdate(){
	if (m_Text != nullptr) {
		Transform2D& transform = GetTransform2D();
		Vector2f sizes = m_Text->GetSizes();
		m_Text->SetPosition(static_cast<int32>(m_RenderPosition.x - (sizes.x / 2.0f)), static_cast<int32>(m_RenderPosition.y - (sizes.y / 2.0f)));
	}
}

Text* WorldText::GetText() { 
	return m_Text; 
}


void WorldText::SetRenderPosition(Vector2f pos){
	m_RenderPosition = pos;
}
