#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include <ostream>
#include <fstream>
 
struct TilemapHeader
{
	int version = 1;

	int mapSize[2] = { 0, 0 }; 

	bool hasTileset = false;
	std::string tilemapPath;
	int cellSize[2] = { 0, 0 };
	int spacing = 0;
	int margin = 0;
	int rowCol[2] = { 0, 0 };  
};

struct TilemapLayer
{
	std::string name;
	bool visible = true;
	bool locked = false;

	std::vector<std::vector<int>> cell;  
};

struct TilemapObject
{
	std::string name;
	int id = 0;

	float posX = 0.f;
	float posY = 0.f;
	float rotation = 0.f;
	float scaleX = 1.f;
	float scaleY = 1.f;

	std::unordered_map<std::string, std::string> properties;
};

struct TilemapCollider
{
	std::string shape;  
	int id = 0;

	float posX = 0.f;
	float posY = 0.f;

	float sizeX = 0.f;  
	float sizeY = 0.f;  
	float radius = 0.f; 

	bool isTrigger = false;
	int layer = 0;
	int tag = -1;
	int mask = 0;
};

class Scene;
class Entity;

namespace Demo
{ 
	class TileMapLoader
	{
	public:
		TileMapLoader() = default;
		~TileMapLoader() = default;

		void CreateMap(Scene* pScene, const std::string& path);

	private:
		void ReadTileset(std::ifstream& file, TilemapHeader& header);
		void ReadLayer(std::ifstream& file, TilemapLayer& layer);
		void ReadObject(std::ifstream& file, TilemapObject& object);
		void ReadCollider(std::ifstream& file, TilemapCollider& collider);

		void CreateLayerEntities(Scene* pScene, const TilemapHeader& header, TilemapLayer& layer, int layerIndex);
		void CreateObjectEntity(Scene* pScene, const TilemapObject& object);
		void CreateColliderEntity(Scene* pScene, const TilemapCollider& collider);
	};
}