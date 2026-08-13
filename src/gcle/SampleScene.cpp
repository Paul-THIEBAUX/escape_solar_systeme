#include "SampleScene.h"
#include "Player.h"
#include "Render/Sprite.h"
#include "Render/Text.h"
#include "AudioEmitter.h"
#include "include.h"
#include "PhysicsManager.h"


void SampleScene::OnInitialize()
{
	Scene::OnInitialize();
	 	 
	std::string text = "Test";
	Text* textObj = CreateText(text, { 40, 40 }, 50);
	//textObj->SetFont("street-fighter");


	Entity* entity1 = CreateEntity<Entity>(gcle::Shapes::Rectangle);
	entity1->SetPosition(-400, 0);
	entity1->SetRigidBody(true);
	entity1->SetTexture("test");
	entity1->AddAnimation("anim1", 0, 3, 0, 32, 32, 1.f);
	entity1->AddAnimation("anim2", 0, 3, 0, 32, 32, 0.2f);
	entity1->PlayAnimation("anim1");

	//Entity* entity2 = CreateEntity<Entity>(gcle::Shapes::Rectangle);
	//entity2->SetPosition(-300, 0);
	//entity2->SetRigidBody(true);
	//entity2->SetTexture("test");
	//entity2->AddAnimation("anim1", 0, 3, 0, 32, 32, 0.2f);
	//entity2->PlayAnimation("anim1");

	//pEntity = CreateEntity<Player>(gcle::Shapes::Circle);
	//pEntity->SetPosition(0, 0); 
	//pEntity->Rotate(0);
	//pEntity->SetRigidBody(true);  
	//pEntity->GetRigidBody()->SetDampingStrenght(0.9f);
	//pEntity->SetTexture("images");
	//Collider* pCol = pEntity->CreateCollider(gcle::Shapes::Circle, true, { 0.0f, 0.0f }, 0.0f, { 1.0f, 1.0f }); 
	//pEntity->GetRigidBody()->SetCollisionOnContinuous();

	pEntity = entity1;
	mp_MainCamera->SetFollowing(entity1);

	pSceneCamera = CreateCamera();
	pSceneCamera->SetFollowing(entity1);

	for (int32 i = 0; i < 50; i++) 
	{
		for (int32 y = 0; y < 10; y++)
		{
			Entity* entity = CreateEntity<Entity>(gcle::Shapes::Rectangle);
			entity->SetScale(0.2f);
			entity->SetPosition(0.0f + i * 100, 0.0f + y * 100);
			entity->SetRigidBody(true);
			entity->GetRigidBody()->SetGravity(false); 
			entity->CreateCollider(gcle::Shapes::Rectangle, true, { { 0, 0 }, 0, { 1.0f, 1.0f } });
			entity->SetStatic(true);
		}
	}

	PhysicsManager::GetInstance().SetFrameBetweenQuadTreeRegenerations(1);
	PhysicsManager::GetInstance().SetActivateQuadTree(true);
	PhysicsManager::GetInstance().SetQuadTreePos1({ -50000, -50000 });
	PhysicsManager::GetInstance().SetQuadTreePos2({ 50000, 50000 });
}

void SampleScene::OnUpdate(Clock& time)
{
	Scene::OnUpdate(time);
	 
	if (InputManager::GetInstance().IsDown('A'))
	{
		pEntity->PlayAnimation("anim2", AnimationMode::Loop | AnimationMode::IgnoreIfAlreadyPlaying);
		/*if (GetCurrentCamera()->GetId() != mp_MainCamera->GetId())
		{
			SwitchCamera(mp_MainCamera);
		}
		else
		{
			SwitchCamera(pSceneCamera);
		} */
	}

	if (InputManager::GetInstance().IsDown('E'))
	{
		pEntity->StopAnimation();
	}

	if (InputManager::GetInstance().IsDown('O'))
		SceneManager::GetInstance().SetCurrentSceneWithTag("DebugScene", false);
} 
