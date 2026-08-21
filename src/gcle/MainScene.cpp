#include "MainScene.h"
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
#include "InteractableObject.h"
#include "Cursor.h"

//#include "Render/Shape.h"

void MainScene::OnInitialize()
{
	debug = false;
	
	Scene::OnInitialize();
	// afficher du text
	//std::string text = "Test";
	//CreateText(text, { 40, 40 }, 50);


	//recup le player si il a pas encore été créer
	std::vector<Entity*> entities = GameManager::GetInstance().GetActiveEntities(m_Tag);
	for (Entity* e : entities)
	{
		if (e->IsTag(Tag::Player))
			player = static_cast<MainPlayer*>(e);
	}

	//creation d'un player classique
	if (player == nullptr) //Check si le player existe déjà
	{
		player = CreateEntity<MainPlayer>(gcle::Shapes::Rectangle);
		{
			//player->SetPosition(-280, 197);
			player->SetTag(Tag::Player);
			player->SetRigidBody(true);
			player->SetStatic(false);
			player->SetLayer(1);
			player->CreateCollider(gcle::Shapes::Rectangle, true, gcle::ColliderDesc({ 0, 0 }, 0.0f, { 1, 1 }));
			player->InitInventory(CreateEntity<Inventory>(gcle::Shapes::Rectangle));
			player->InitCursor(CreateEntity<Cursor>(gcle::Shapes::Rectangle));
		}
	}
	player->SetPosition(-280, 197); // seul truc qui doit être changé si le player existe déjà


	Wall* WallUp = CreateEntity<Wall>(gcle::Shapes::Rectangle);
	{	
		if (debug == true) {
			WallUp->SetColor(Color::Red);
		}
		else {
			WallUp->SetColor(Color::Transparent);;
		}
		WallUp->SetPosition(35, 50);
		WallUp->SetScale({ 20, 1 });
		BedRoomEntities.push_back(WallUp);
	}

	Wall* WallDown = CreateEntity<Wall>(gcle::Shapes::Rectangle);
	{
		if (debug == true) {
			WallDown->SetColor(Color::Red);
		}
		else {
			WallDown->SetColor(Color::Transparent);
		}
		WallDown->SetPosition(35, 810);
		WallDown->SetScale({ 20, 1 });
		BedRoomEntities.push_back(WallDown);
	}

	Wall* WallLeft = CreateEntity<Wall>(gcle::Shapes::Rectangle);
	{
		if (debug == true) {
			WallLeft->SetColor(Color::Red);
		}
		else {
			WallLeft->SetColor(Color::Transparent);
		}
		WallLeft->SetPosition(-1000, 400);
		WallLeft->SetScale({ 1, 10 });
		BedRoomEntities.push_back(WallLeft);
	}

	Wall* WallRight = CreateEntity<Wall>(gcle::Shapes::Rectangle);
	{
		if (debug == true) {
			WallRight->SetColor(Color::Red);
		}
		else {
			WallRight->SetColor(Color::Transparent);
		}
		WallRight->SetPosition(985, 600);
		WallRight->SetScale({ 1,5 });
		BedRoomEntities.push_back(WallRight);
	}

	door = CreateEntity<Door>(gcle::Shapes::Rectangle);
	{
		door->Unlock();
		door->SetPosition(985, 400);
		door->SetId("CorridorScene");
		door->SetScale({ 1, 5 });
		BedRoomEntities.push_back(door);
	}


	Entity* Bed = CreateEntity<Entity>(gcle::Shapes::Rectangle);
	{
		Bed->SetTag(Tag::Obstacle);
		Bed->SetTexture("S_Bed");
		Bed->SetScale({ 4.f, 1.65f });
		Bed->SetPosition(-275, 107);
		Bed->SetRigidBody(true);
		Bed->SetStatic(true);
		Bed->SetLayer(1);
		Bed->CreateCollider(gcle::Shapes::Rectangle, true, gcle::ColliderDesc({ 0, 0 }, 0.0f, { 1, 1 }));
		BedRoomEntities.push_back(Bed);
	}

	InteractableObject* Table = CreateEntity<InteractableObject>(gcle::Shapes::Rectangle); // InteractableObject = objet recuperable
	{
		Table->SetTag(Tag::Obstacle);
		Table->SetTexture("S_Table");
		Table->SetScale({ 0.75, 1 });
		Table->SetPosition(-450, 700);
		Table->SetRigidBody(true);
		Table->SetStatic(true);
		Table->SetLayer(1);
		Table->CreateCollider(gcle::Shapes::Rectangle, true, gcle::ColliderDesc({ 0, 0 }, 0.0f, { 1, 1 }));
		BedRoomEntities.push_back(Table);
		pickAbleItem.push_back(Table); // A rajoute si tu veux que ca marche
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
		Background->CreateCollider(gcle::Shapes::Rectangle, true, gcle::ColliderDesc({0, 0}, 0.0f, {1, 1}));
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

	//Check door
	Vector2f pos = GetMainCamera()->GetScreenMousePosition();
	door->CheckDoor(player, pos);


	//Pick an item
	if (pickAbleItem.size() == 0) return;

	for (auto it = pickAbleItem.begin(); it != pickAbleItem.end();) 
	{
		if ((*it)->HasBeenPick())
		{
			player->AddEntity((*it));
			it = pickAbleItem.erase(it);
		}
		else
			it++;
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

void MainScene::OnExit()
{
	for (Entity* e : BedRoomEntities)
	{
		e->Destroy();
	}
}
