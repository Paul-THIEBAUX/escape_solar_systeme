#include "SampleScene2.h"
#include "Player.h"

void SampleScene2::OnInitialize()
{ 
	 
	Entity* entity1 = CreateEntity<Entity>(gcle::Shapes::Rectangle);
 	entity1->SetPosition(0, -100); 
	 
	Entity* entity2 = CreateEntity<MainPlayer>(gcle::Shapes::Rectangle);
	entity2->SetPosition(10, 100); 

	Entity* entity3 = CreateEntity<Entity>(gcle::Shapes::Rectangle);
	entity3->SetPosition(110, -100);
	/*entity2->SetRigidBody(true);
	entity2->GetRigidBody()->SetGravity(false);
	entity2->GetShape()->SetIsStatic(false);
	entity2->SetTexture("test");
	entity2->AddAnimation("anim1", 0, 3, 0, 32, 32);
	entity2->PlayAnimation("anim1");*/
}

void SampleScene2::OnUpdate(Clock& time)
{
	if (InputManager::GetInstance().IsDown(Space))
	{
		GameManager::GetInstance().SetWindowClearColor({ 255, 255, 255, 255 });
	}

} 
