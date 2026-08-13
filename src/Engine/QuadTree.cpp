#include "QuadTree.h"
#include "Entity.h"

QuadTree::QuadTree(float32 worldMinX, float32 worldMinY, float32 worldMaxX, float32 worldMaxY): m_Pool(256) {
	m_Root = QuadNode{worldMinX, worldMinY, worldMaxX, worldMaxY , 0};
}

void QuadTree::Insert(Collider* pEntry){
	m_Root.Insert(pEntry, m_Pool);
}

std::vector<Collider*>& QuadTree::Query(Collider* pEntry){

	for (auto& r : m_SeenEntities) {
		r->SetInQuerySeen(false);
	}
	m_SeenEntities.clear();
	m_QueryResult.clear();

	pEntry->SetInQuerySeen(true);
	m_SeenEntities.push_back(pEntry);


	float32 margin = 1.f;
	AABB marginAABB = { pEntry->GetAABB().minX - margin, pEntry->GetAABB().minY - margin, pEntry->GetAABB().maxX + margin, pEntry->GetAABB().maxY + margin };
	m_Root.Query(marginAABB, m_QueryResult, m_SeenEntities);
	
	return m_QueryResult;
}

void QuadTree::Clear(){
	for (auto& r : m_SeenEntities) {
		r->SetInQuerySeen(false);
	}
	m_SeenEntities.clear();
	m_QueryResult.clear();
	m_Pool.Reset();
	m_Root.Clear();
}
