#include "DebugScene.h"
#include "DebugPlayer.h"
#include "Render/Sprite.h"
#include "Render/Text.h"
#include "Engine/Collider.h"
#include "Engine/PhysicsManager.h"
#include "Tag.h"
//#include "Render/Shape.h"

void DebugScene::OnInitialize()
{
	Scene::OnInitialize();

	std::string text = "Test Salut, je suis thomas le goat et ethan va te faire";
	CreateText(text, { 40, 40 }, 50);

	Collider* setUpCollider = nullptr;

	//basic creation of an entity
	Entity* entity1 = CreateEntity<DebugPlayer>(gcle::Shapes::Rectangle);
	{
		entity1->SetPosition(50, -200);
		entity1->SetRotation(0);

		//player tag
		entity1->SetTag(Tag::Player);

		entity1->SetRigidBody(true);
		entity1->GetRigidBody()->SetGravity(10, true);
		entity1->GetRigidBody()->SetMaxSpeed(100);

		// call Brake() when you want it to work
		entity1->GetRigidBody()->SetBrakeDeceleration(100);

		//if an entity is static she can't move and get her velocity set to 0
		entity1->SetStatic(false);

		//an entity can't clip through another one
		//but take a high cost 
		entity1->GetRigidBody()->SetCollisionOnContinuous();

		setUpCollider = entity1->CreateCollider(gcle::Shapes::Rectangle, true, {{ 0, 0 }, 0, { 1, 1 }});
	}

	//creation d'un trigger
	Entity* entity2 = CreateEntity<Entity>(gcle::Shapes::Rectangle);
	{
		entity2->SetPosition(200, -100);
		entity2->SetScale({ 1, 5 });
		entity2->SetRotation(0);
		entity2->SetTag(Tag::Trigger);
		entity2->SetRigidBody(false);
		entity2->SetStatic(true);
		setUpCollider = entity2->CreateCollider(gcle::Shapes::Rectangle, true, { { 0, 0 }, 0, { 1, 1 } });
	}

	//creation d'un sol
	Entity* entity3 = CreateEntity<Entity>(gcle::Shapes::Rectangle);
	{
		entity3->SetPosition(0, 300);
		entity3->SetScale({ 10, 1 });
		entity3->SetTag(Tag::Ground);
		entity3->SetRotation(0);
		entity3->SetRigidBody(true);
		entity3->SetStatic(true);
		setUpCollider = entity3->CreateCollider(gcle::Shapes::Rectangle, true, {{ 0, 0 }, 0, { 1, 1 }});
	}

	Button* ui = CreateButton(gcle::Shapes::Rectangle, "hey");
	ui->SetScale({ 2, 2 });
	ui->GetText()->SetFontSize(60);
	ui->GetText()->SetColor(0, 0, 0, 255);
	ui->SetPosition(150, 150);
	//ui->SetFunctions({});
	//ou 
	//ui->AddFunction();

	mp_MainCamera->SetFollowing(entity1);

	//pSceneCamera = CreateCamera();

	PhysicsManager::GetInstance().SetFrameBetweenQuadTreeRegenerations(1);
	PhysicsManager::GetInstance().SetActivateQuadTree(true);
	PhysicsManager::GetInstance().SetQuadTreePos1({ -50000, -50000 });
	PhysicsManager::GetInstance().SetQuadTreePos2({ 50000, 50000 });
}

void DebugScene::OnUpdate(Clock& time)
{
	Scene::OnUpdate(time);

	if (InputManager::GetInstance().IsDown(Space))
	{
		SceneManager::GetInstance().SetCurrentSceneToPreviousScene();
	}

	if (InputManager::GetInstance().IsDown('A'))
	{
		pEntity->Destroy();
	}
}
