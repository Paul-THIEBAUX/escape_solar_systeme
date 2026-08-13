#include "TileMapLoader.h"

#include "Core/include.h"
#include "Core/Mask.h"
#include "Render/Sprite.h"
#include "Engine/RessourceManager.h"
#include "Engine/GameManager.h"
#include "Engine/Scene.h"
#include "Engine/Entity.h"
#include "Engine/Collider.h"

#include <array>
#include <sstream>
#include <filesystem>

constexpr float32 BASE_SHAPE_SIZE = 100.0f;

void Demo::TileMapLoader::CreateMap(Scene* pScene, const std::string& path)
{
	std::ifstream file(path);
	if (!file.is_open())
	{
		GCLE_WARN << "Can't open file with path : " << path << ENDL;
		return;
	}

	TilemapHeader header{};

	std::string signature;
	file >> signature >> header.version;

	if (signature != "TEMAP")
	{
		GCLE_WARN << "Invalid signature, expected 'TEMAP' in file : " << path << ENDL;
		return;
	}

	std::string token;

	file >> token; // "mapsize"
	file >> header.mapSize[0] >> header.mapSize[1];

	file >> token; // "tileset"
	if (token != "tileset")
	{
		GCLE_WARN << "Expected 'tileset' token in file : " << path << ENDL;
		return;
	}
	ReadTileset(file, header);

	int layerIndex = 0;

	while (file >> token)
	{
		if (token == "layer")
		{ 
			TilemapLayer layer;
			file >> layer.name;
			ReadLayer(file, layer);

			CreateLayerEntities(pScene, header, layer, layerIndex);
			++layerIndex;
		}
		else if (token == "object")
		{
			TilemapObject object;
			file >> object.name >> object.id;
			ReadObject(file, object);

			CreateObjectEntity(pScene, object);
		}
		else if (token == "collider")
		{
			TilemapCollider collider;
			file >> collider.shape >> collider.id;
			ReadCollider(file, collider);

			CreateColliderEntity(pScene, collider);
		}
		else
		{
			GCLE_WARN << "Unknown token in tilemap file, ignored : " << token << ENDL;
		}
	}
}

void Demo::TileMapLoader::ReadTileset(std::ifstream& file, TilemapHeader& header)
{
	std::string token;
	file >> token; 

	if (token == "endtileset")
	{
		header.hasTileset = false;
		return;
	}

	header.hasTileset = true;
	header.tilemapPath = token;
	 
	file >> header.cellSize[0] >> header.cellSize[1] >> header.spacing >> header.margin >> header.rowCol[1] >> header.rowCol[0];

	file >> token; // "endtileset"
	if (token != "endtileset")
	{
		GCLE_WARN << "Expected 'endtileset' token, got : " << token << ENDL;
	}
}

void Demo::TileMapLoader::ReadLayer(std::ifstream& file, TilemapLayer& layer)
{
	std::string token;
	int visibleVal = 1;
	int lockedVal = 0;

	file >> token >> visibleVal;  
	file >> token >> lockedVal;   

	layer.visible = (visibleVal != 0);
	layer.locked = (lockedVal != 0);
	 
	std::string dummy;
	std::getline(file, dummy);

	std::string line;
	while (std::getline(file, line))
	{
		if (line == "endlayer")
			break;

		if (line.empty())
			continue;

		std::istringstream iss(line);
		std::vector<int> row;
		int value = 0;
		while (iss >> value)
			row.push_back(value);

		if (!row.empty())
			layer.cell.push_back(std::move(row));
	}
}

void Demo::TileMapLoader::ReadObject(std::ifstream& file, TilemapObject& object)
{
	file >> object.posX >> object.posY;
	file >> object.rotation;
	file >> object.scaleX >> object.scaleY;

	std::string dummy;
	std::getline(file, dummy);

	std::string line;
	while (std::getline(file, line))
	{
		if (line == "endobject")
			break;

		if (line.empty())
			continue;

		std::istringstream iss(line);
		std::string keyword;
		iss >> keyword;

		if (keyword == "prop")
		{
			std::string key;
			iss >> key;

			std::string value;
			std::getline(iss, value);

			if (!value.empty() && value.front() == ' ')
				value.erase(0, 1);

			object.properties[key] = value;
		}
	}
}

void Demo::TileMapLoader::ReadCollider(std::ifstream& file, TilemapCollider& collider)
{
	file >> collider.posX >> collider.posY;

	if (collider.shape == "Rectangle")
	{
		file >> collider.sizeX >> collider.sizeY;
	}
	else
	{
		file >> collider.radius;
	}

	std::string token;
	int triggerVal = 0;

	file >> token >> triggerVal;      
	file >> token >> collider.layer;  
	file >> token >> collider.tag;    
	file >> token >> collider.mask;   

	collider.isTrigger = (triggerVal != 0);

	file >> token; // "endcollider"
	if (token != "endcollider")
	{
		GCLE_WARN << "Expected 'endcollider' token, got : " << token << ENDL;
	}
}

void Demo::TileMapLoader::CreateLayerEntities(Scene* pScene, const TilemapHeader& header, TilemapLayer& layer, int layerIndex)
{
	if (!header.hasTileset)
	{
		GCLE_WARN << "Tilemap has no tileset, can't create tile entities" << ENDL;
		return;
	}

	if (!layer.visible)
	{
		layer.cell.clear();
		layer.cell.shrink_to_fit();
		return;
	}

	auto OneDtoTwoDArray = [](int val, const TilemapHeader& h) -> std::array<int, 2>
		{
			return {
				val / h.rowCol[1],
				val % h.rowCol[1]
			};
		};

	std::string textureName = std::filesystem::path(header.tilemapPath).stem().string();

	for (int y = 0; y < static_cast<int>(layer.cell.size()); ++y)
	{
		std::vector<int>& row = layer.cell[y];

		for (int x = 0; x < static_cast<int>(row.size()); ++x)
		{
			int tileValue = row[x];
			if (tileValue == -1)
				continue;

			std::array<int, 2> tilePos = OneDtoTwoDArray(tileValue, header);

			Entity* pEntity = pScene->CreateEntity<Entity>(gcle::Shapes::Rectangle);
			pEntity->SetRigidBody(false);

			pEntity->SetPosition(
				static_cast<float32>(x * header.cellSize[0]),
				static_cast<float32>(y * header.cellSize[1]));

			pEntity->SetTexture(textureName);
			pEntity->GetRenderShape()->SetTextureRect(
				tilePos[0], tilePos[1],
				header.cellSize[0], header.cellSize[1],
				header.rowCol[0] * header.cellSize[0],
				header.rowCol[1] * header.cellSize[1]);

			pEntity->SetLayer(layerIndex);
		} 
		row.clear();
		row.shrink_to_fit();
	}

	layer.cell.clear();
	layer.cell.shrink_to_fit();
}

void Demo::TileMapLoader::CreateObjectEntity(Scene* pScene, const TilemapObject& objectData)
{
	Entity* pEntity = pScene->CreateEntity<Entity>();
	pEntity->SetRigidBody(false);

	pEntity->SetPosition(objectData.posX, objectData.posY);
	pEntity->SetRotation(objectData.rotation);
	pEntity->SetScale({ objectData.scaleX, objectData.scaleY });
	 
	auto tagIt = objectData.properties.find("tag");
	if (tagIt != objectData.properties.end())
	{
		pEntity->SetTag(std::stoi(tagIt->second));
	} 
}

void Demo::TileMapLoader::CreateColliderEntity(Scene* pScene, const TilemapCollider& colliderData)
{
	Entity* pEntity = pScene->CreateEntity<Entity>();

	pEntity->SetPosition(colliderData.posX, colliderData.posY);
	pEntity->SetTag(colliderData.tag);

	pEntity->SetRigidBody(true);
	pEntity->SetStatic(true);

	gcle::ColliderDesc desc{};
	gcle::Shapes shape;

	if (colliderData.shape == "Rectangle")
	{
		shape = gcle::Shapes::Rectangle;
		desc.RelativeScale = { colliderData.sizeX / BASE_SHAPE_SIZE, colliderData.sizeY / BASE_SHAPE_SIZE };
	}
	else
	{
		shape = gcle::Shapes::Circle;
		float32 s = colliderData.radius / BASE_SHAPE_SIZE;
		desc.RelativeScale = { s, s };
	}

	pEntity->CreateCollider(shape, true, desc, colliderData.isTrigger);
	 
	if (colliderData.mask != 0)
		*pEntity->GetCollisionLayer() = colliderData.mask;
	else
		Mask::AddLayer<int32>(*pEntity->GetCollisionLayer(), colliderData.layer);
}