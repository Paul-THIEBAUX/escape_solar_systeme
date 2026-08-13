#pragma once

#include "Tuto-GCLE/Object.h"


namespace Demo
{
	class Table : public Object
	{
	public:
		void OnInitialize() override;
		void OnInteract(Entity* pEntity) override;

		bool IsDown() const;
	private:

		bool m_IsDown = false;
	};
}
