#pragma once
#include "include.h"
#include "QuadNode.h"
#include "Collider.h"

class QuadTree {

	QuadNode m_Root;
	QuadNodePool m_Pool;
	std::vector<Collider*> m_QueryResult;
	std::vector <Collider*> m_SeenEntities;

public:


	QuadTree(float32 worldMinX, float32 worldMinY, float32 worldMaxX, float32 worldMaxY);
	QuadTree() = default;


	~QuadTree() = default;

	void Insert(Collider* pEntry);

	void Clear();

	std::vector<Collider*>& Query(Collider* pEntry);
	friend class PhysicsManager;
};