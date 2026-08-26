#include "CorridorScene.h"
#include "Render/Sprite.h"
#include "Render/Text.h"
#include "Engine/Collider.h"
#include "Engine/PhysicsManager.h"

#include "Tag.h"
#include "Player.h"
#include "Door.h"
#include "SaveManager.h"
#include "Wall.h"
#include "Inventory.h"

//#include "Render/Shape.h"

void CorridorScene::OnInitialize()
{
	debug = false;

	Scene::OnInitialize();
	// afficher du text
	//std::string text = "Test";
	//CreateText(text, { 40, 40 }, 50);

	std::vector<Entity*> entities = GameManager::GetInstance().GetActiveEntities(m_Tag);
	for (Entity* e : entities)
	{
		if (e->IsTag(Tag::Player))
			player = static_cast<MainPlayer*>(e);
	}

	//creation d'un player classique
	if(player == nullptr)
	{
		player = CreateEntity<MainPlayer>(gcle::Shapes::Rectangle);
		{
			//player->SetPosition(-280, 197);
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
			player->SetLayer(3);
			player->CreateCollider(gcle::Shapes::Rectangle, true, gcle::ColliderDesc({ 0, 0 }, 0.0f, { 1, 1 }));
			player->InitInventory(CreateEntity<Inventory>(gcle::Shapes::Rectangle));
		}
	}
	player->SetPosition(-280, 197);



	



	Wall* WallNE = CreateEntity<Wall>(gcle::Shapes::Rectangle);
	{
		if (debug == true) {
			WallNE->SetColor(Color::Red);
		}
		else {
			WallNE->SetColor(Color::Transparent);;
		}
		WallNE->SetPosition(-780, 60);
		WallNE->SetScale({ 5, 5 });
		CorridorEntities.push_back(WallNE);
		WallNE->SetLayer(0);
	}

	Wall* WallN = CreateEntity<Wall>(gcle::Shapes::Rectangle);
	{
		if (debug == true) {
			WallN->SetColor(Color::Red);
		}
		else {
			WallN->SetColor(Color::Transparent);;
		}
		WallN->SetPosition(75, 60);
		WallN->SetScale({ 7.7, 5 });
		CorridorEntities.push_back(WallN);
		WallN->SetLayer(0);
	}

	Wall* WallNW = CreateEntity<Wall>(gcle::Shapes::Rectangle);
	{
		if (debug == true) {
			WallNW->SetColor(Color::Red);
		}
		else {
			WallNW->SetColor(Color::Transparent);
		}
		WallNW->SetPosition(950, 60);
		WallNW->SetScale({ 5, 5 });
		CorridorEntities.push_back(WallNW);
		WallNW->SetLayer(0);
	}

	Wall* WallSE = CreateEntity<Wall>(gcle::Shapes::Rectangle);
	{
		if (debug == true) {
			WallSE->SetColor(Color::Red);
		}
		else {
			WallSE->SetColor(Color::Transparent);
		}
		WallSE->SetPosition(-780, 730);
		WallSE->SetScale({ 5, 5 });
		CorridorEntities.push_back(WallSE);
		WallSE->SetLayer(0);
	}

	Wall* WallS = CreateEntity<Wall>(gcle::Shapes::Rectangle);
	{
		if (debug == true) {
			WallS->SetColor(Color::Red);
		}
		else {
			WallS->SetColor(Color::Transparent);;
		}
		WallS->SetPosition(75, 730);
		WallS->SetScale({ 7.7, 5 });
		CorridorEntities.push_back(WallS);
		WallS->SetLayer(0);
	}

	Wall* WallSW = CreateEntity<Wall>(gcle::Shapes::Rectangle);
	{
		if (debug == true) {
			WallSW->SetColor(Color::Red);
		}
		else {
			WallSW->SetColor(Color::Transparent);
		}
		WallSW->SetPosition(950, 730);
		WallSW->SetScale({ 5, 5 });
		CorridorEntities.push_back(WallSW);
		WallSW->SetLayer(0);
	}



	Door* doorE = CreateEntity<Door>(gcle::Shapes::Rectangle);
	{
		doorE->Unlock();
		doorE->SetPosition(1000, 395);
		doorE->SetId("ReactorScene");
		doorE->SetTexture("S_Door");
		doorE->SetScale({ 1, 1.6667 });
		doorE->SetLayer(2);
		CorridorEntities.push_back(doorE);
		Doors.push_back(doorE);
		
	}

	Door* doorNE = CreateEntity<Door>(gcle::Shapes::Rectangle);
	{
		doorNE->Unlock();
		doorNE->SetPosition(-417, 60);
		doorNE->SetId("LaboScene");
		doorNE->SetTexture("S_Door90");
		doorNE->SetScale({ 2.3, 1 });
		doorNE->SetLayer(2);
		CorridorEntities.push_back(doorNE);
		Doors.push_back(doorNE);
	}

	Door* doorSE = CreateEntity<Door>(gcle::Shapes::Rectangle);
	{
		doorSE->Unlock();
		doorSE->SetPosition(-417, 730);
		doorSE->SetId("CuisineScene");
		doorSE->SetTexture("S_Door90");
		doorSE->SetScale({ 2.3, 1 });
		doorSE->SetLayer(2);
		CorridorEntities.push_back(doorSE);
		Doors.push_back(doorSE);
	}

	Door* doorNW = CreateEntity<Door>(gcle::Shapes::Rectangle);
	{
		doorNW->Unlock();
		doorNW->SetPosition(580, 60);
		doorNW->SetId("CabineScene");
		doorNW->SetTexture("S_Door90");
		doorNW->SetScale({ 2.3, 1 });
		doorNW->SetLayer(2);
		CorridorEntities.push_back(doorNW);
		Doors.push_back(doorNW);
	}

	Door* doorSW = CreateEntity<Door>(gcle::Shapes::Rectangle);
	{
		doorSW->Unlock();
		doorSW->SetPosition(580, 730);
		doorSW->SetId("StorageScene");
		doorSW->SetTexture("S_Door90");
		doorSW->SetScale({ 2.3, 1 });
		doorSW->SetLayer(2);
		CorridorEntities.push_back(doorSW);
		Doors.push_back(doorSW);
	}

	Door* doorW = CreateEntity<Door>(gcle::Shapes::Rectangle);
	{
		doorW->Unlock();
		doorW->SetPosition(-1000, 395);
		doorW->SetId("MainScene");
		doorW->SetTexture("S_Door");
		doorW->SetScale({ 1, 1.6667 });
		doorW->SetLayer(2);
		CorridorEntities.push_back(doorW);
		Doors.push_back(doorW);
	}


	Entity* Background = CreateEntity<Entity>(gcle::Shapes::Rectangle);
	{
		Background->SetPosition(0, 300);
		Background->SetScale({ 16.8f * 1.2f, 9 * 1.2f });
		Background->SetTag(Tag::Ground);
		Background->SetRotation(0);
		Background->SetLayer(1);
		Background->SetRigidBody(false);
		Background->SetStatic(true);
		Background->SetTexture("S_Corridor");
		Background->CreateCollider(gcle::Shapes::Rectangle, true, gcle::ColliderDesc({ 0, 0 }, 0.0f, { 1, 1 }));
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

void CorridorScene::OnUpdate(Clock& time)
{
	Scene::OnUpdate(time);
	float32 j = GetMainCamera()->GetZoom();

	if (InputManager::GetInstance().IsDown('I'))
	{
		GetMainCamera()->SetZoom(j + 1);
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
		player->GoToPosition(pos.x, pos.y, 1000);
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

	Vector2f pos = GetMainCamera()->GetScreenMousePosition();
	for (Door* door : Doors)
	{
		door->CheckDoor(player, pos);
	}
}

void CorridorScene::OnCollisionAB(MainPlayer* A, Entity* B)
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