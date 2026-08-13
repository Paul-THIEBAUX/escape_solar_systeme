#include "MainScene.h"
#include "Render/Sprite.h"
#include "Render/Text.h"
#include "Engine/Collider.h"
#include "Engine/PhysicsManager.h"

#include "Tag.h"
#include "gcle/Player.h"

//#include "Render/Shape.h"

void MainScene::OnInitialize()
{
	debug = true;
	
	Scene::OnInitialize();
	// afficher du text
	//std::string text = "Test";
	//CreateText(text, { 40, 40 }, 50);

	Collider* setUpCollider = nullptr;

	//creation d'un player classique
	player = CreateEntity<MainPlayer>(gcle::Shapes::Rectangle);
	{
		player->SetPosition(-280, 197);
		if (debug == true) {
			player->SetColor(Color::Blue);
			player->SetScale({ 1, 1 });
		}
		else {
			player->SetTexture("E_Joueur");
			player->SetScale({ 1, 2 });
		}
		player->SetTag(Tag::Player);
		player->SetRigidBody(true);
		player->SetStatic(false);
		player->SetLayer(1);
		setUpCollider = player->CreateCollider(gcle::Shapes::Rectangle, true, gcle::ColliderDesc({0, 0}, 0.0f, {1, 1}));
	}

	Entity* WallUp = CreateEntity<Entity>(gcle::Shapes::Rectangle);
	{
		WallUp->SetTag(Tag::Obstacle);
		WallUp->SetColor(Color::Red);
		WallUp->SetScale({ 20, 1 });
		WallUp->SetPosition(35, 50);
		WallUp->SetRigidBody(true);
		WallUp->SetStatic(true);
		WallUp->SetLayer(1);
		setUpCollider = WallUp->CreateCollider(gcle::Shapes::Rectangle, true, gcle::ColliderDesc({ 0, 0 }, 0.0f, { 1, 1 }));
		BedRoomEntities.push_back(WallUp);
	}

	Entity* WallDown = CreateEntity<Entity>(gcle::Shapes::Rectangle);
	{
		WallDown->SetTag(Tag::Obstacle);
		WallDown->SetColor(Color::Red);
		WallDown->SetScale({ 20, 1 });
		WallDown->SetPosition(35, 810);
		WallDown->SetRigidBody(true);
		WallDown->SetStatic(true);
		WallDown->SetLayer(1);
		setUpCollider = WallDown->CreateCollider(gcle::Shapes::Rectangle, true, gcle::ColliderDesc({ 0, 0 }, 0.0f, { 1, 1 }));
		BedRoomEntities.push_back(WallDown);
	}

	Entity* WallLeft = CreateEntity<Entity>(gcle::Shapes::Rectangle);
	{
		WallLeft->SetTag(Tag::Obstacle);
		WallLeft->SetColor(Color::Red);
		WallLeft->SetScale({ 1, 10 });
		WallLeft->SetPosition(-1000, 400);
		WallLeft->SetRigidBody(true);
		WallLeft->SetStatic(true);
		WallLeft->SetLayer(1);
		setUpCollider = WallLeft->CreateCollider(gcle::Shapes::Rectangle, true, gcle::ColliderDesc({ 0, 0 }, 0.0f, { 1, 1 }));
		BedRoomEntities.push_back(WallLeft);
	}

	Entity* WallRight = CreateEntity<Entity>(gcle::Shapes::Rectangle);
	{
		WallRight->SetTag(Tag::Obstacle);
		WallRight->SetColor(Color::Red);
		WallRight->SetScale({ 1, 5 });
		WallRight->SetPosition(985, 600);
		WallRight->SetRigidBody(true);
		WallRight->SetStatic(true);
		WallRight->SetLayer(1);
		setUpCollider = WallRight->CreateCollider(gcle::Shapes::Rectangle, true, gcle::ColliderDesc({ 0, 0 }, 0.0f, { 1, 1 }));
		BedRoomEntities.push_back(WallRight);
	}

	Entity* Door = CreateEntity<Entity>(gcle::Shapes::Rectangle);
	{
		Door->SetTag(Tag::Obstacle);
		Door->SetColor(Color::Red);
		Door->SetScale({ 1, 5 });
		Door->SetPosition(985, 600);
		Door->SetRigidBody(true);
		Door->SetStatic(true);
		Door->SetLayer(1);
		setUpCollider = Door->CreateCollider(gcle::Shapes::Rectangle, true, gcle::ColliderDesc({ 0, 0 }, 0.0f, { 1, 1 }));
		BedRoomEntities.push_back(Door);
	}


	Entity* Bed = CreateEntity<Entity>(gcle::Shapes::Rectangle);
	{
		Bed->SetTag(Tag::Obstacle);
		Bed->SetTexture("S_Bed");
		Bed->SetScale({ 4, 1.65 });
		Bed->SetPosition(-275, 107);
		Bed->SetRigidBody(true);
		Bed->SetStatic(true);
		Bed->SetLayer(1);
		setUpCollider = Bed->CreateCollider(gcle::Shapes::Rectangle, true, gcle::ColliderDesc({ 0, 0 }, 0.0f, { 1, 1 }));
		BedRoomEntities.push_back(Bed);
	}

	Entity* Table = CreateEntity<Entity>(gcle::Shapes::Rectangle);
	{
		Table->SetTag(Tag::Obstacle);
		Table->SetTexture("S_Table");
		Table->SetScale({ 0.75, 1 });
		Table->SetPosition(-450, 700);
		Table->SetRigidBody(true);
		Table->SetStatic(true);
		Table->SetLayer(1);
		setUpCollider = Table->CreateCollider(gcle::Shapes::Rectangle, true, gcle::ColliderDesc({ 0, 0 }, 0.0f, { 1, 1 }));
		BedRoomEntities.push_back(Table);
	}

	Entity* Background = CreateEntity<Entity>(gcle::Shapes::Rectangle);
	{
		Background->SetPosition(0, 300);
		Background->SetScale({ 16.8f * 1.2f, 9 * 1.2f });
		Background->SetTag(Tag::Ground);
		Background->SetRotation(0);
		Background->SetLayer(0);
		Background->SetRigidBody(false);
		Background->SetStatic(true);
		Background->SetTexture("S_BedRoom");
		setUpCollider = Background->CreateCollider(gcle::Shapes::Rectangle, true, gcle::ColliderDesc({0, 0}, 0.0f, {1, 1}));
	}



	//creation d'un trigger
	//Entity* entity2 = CreateEntity<Entity>(gcle::Shapes::Rectangle);
	//{
	//	entity2->SetPosition(200, -100);
	//	entity2->SetScale({ 1, 5 });
	//	entity2->SetRotation(0);
	//	entity2->SetTag(Demo::Tag::Trigger);
	//	entity2->SetRigidBody(false);
	//	entity2->SetStatic(true);
	//	setUpCollider = entity2->CreateCollider(gcle::Shapes::Rectangle, true, { 0, 0 }, 0, { 1, 1 });
	//}

	//Button* ui = CreateButton(gcle::Shapes::Rectangle, "hey");
	//ui->SetScale({ 10, 10 });
	//ui->GetText()->SetFontSize(300);
	//ui->GetText()->SetColor(0, 0, 0, 255);
	//ui->SetPosition(500, 500);

	mp_MainCamera->SetFollowing(player);
	GetMainCamera()->SetZoom(1);

	PhysicsManager::GetInstance().SetFrameBetweenQuadTreeRegenerations(1);
	PhysicsManager::GetInstance().SetActivateQuadTree(true);
	PhysicsManager::GetInstance().SetQuadTreePos1({ -50000, -50000 });
	PhysicsManager::GetInstance().SetQuadTreePos2({ 50000, 50000 });
}

void MainScene::OnUpdate(Clock& time)
{
	Scene::OnUpdate(time);
	float32 j = GetMainCamera()->GetZoom();
	if (InputManager::GetInstance().IsDown('I'))
	{
		GetMainCamera()->SetZoom(j+1);
	}
	if (InputManager::GetInstance().IsDown('K'))
	{
		if (j > 1) {
			GetMainCamera()->SetZoom(j - 1);
		}
	}


	

	if ((GetKeyState(VK_LBUTTON) & 0x80) != 0 && action == false)
	{
		Vector2f pos = GetMainCamera()->GetMouseScreenToWorldPosition();
		player->GoToPosition(pos.x , pos.y, 1000);
	}

	if (InputManager::GetInstance().IsDown(Space))
	{
		SceneManager::GetInstance().SetCurrentSceneToPreviousScene();
	}

	if (InputManager::GetInstance().IsDown('A'))
	{
		if (pEntity != nullptr)
		{
		pEntity->Destroy();
		}
	}
}

void MainScene::OnCollisionAB(MainPlayer* A, Entity* B)
{
	if (A->IsTag(Tag::Player) && B->IsTag(Tag::Ground))
	{
		std::cout << "Collision Player/Ground" << std::endl;
	}
	else if (A->IsTag(Tag::Player) && B->IsTag(Tag::Obstacle))
	{
		std::cout << "Collision Player/Obstacle" << std::endl;
	}
	else if (A->IsTag(Tag::Player) && B->IsTag(Tag::Trigger))
	{
		std::cout << "Collision Player/Trigger" << std::endl;
	}
}