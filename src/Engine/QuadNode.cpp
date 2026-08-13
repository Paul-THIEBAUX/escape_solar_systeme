#include "QuadNode.h"
#include "Entity.h"
#include "Collider.h"
#include "Utils.h"


QuadNode::QuadNode(float32 x1, float32 y1, float32 x2, float32 y2, int32 d) {
	m_Bounds = { x1, y1, x2, y2 };
	m_Depth = d;
}

QuadNode::~QuadNode(){
	if (m_Childs[0] == nullptr)
		return;
}

void QuadNode::Subdivide(QuadNodePool& pool){
	Vector2f mid{ (m_Bounds.minX + m_Bounds.maxX) * 0.5f, (m_Bounds.minY + m_Bounds.maxY) * 0.5f };
	int32 d = m_Depth + 1;

	m_Childs[0] = pool.Get(m_Bounds.minX, m_Bounds.minY, mid.x, mid.y, d);
	m_Childs[1] = pool.Get(mid.x, m_Bounds.minY, m_Bounds.maxX, mid.y, d);
	m_Childs[2] = pool.Get(m_Bounds.minX, mid.y, mid.x, m_Bounds.maxY, d);
	m_Childs[3] = pool.Get(mid.x, mid.y, m_Bounds.maxX, m_Bounds.maxY, d);

	for (auto& e : m_Entities) {
		for (auto& child : m_Childs) {
			if (child->m_Bounds.overlaps(e->GetAABB())) {
				child->m_Entities.push_back(e);
			}
		}
	}
	m_Entities.clear();

	for (auto& child : m_Childs) {
		if (static_cast<int32>(child->m_Entities.size()) > maxEntities && child->m_Depth < maxDepth) {
			child->Subdivide(pool);
		}
	}
}

bool QuadNode::IsLeaf(){
	return m_Childs[0] == nullptr;
}

void QuadNode::Insert(Collider* pCollider, QuadNodePool& pool){
	AABB aabb = pCollider->GetAABB();

	if (!m_Bounds.overlaps(aabb))
		return;

	if (IsLeaf()) {
		m_Entities.push_back(pCollider);
		if (static_cast<int32>(m_Entities.size()) > maxEntities && m_Depth < maxDepth) {
			Subdivide(pool);
		}
	}
	else {
		for (auto* c : m_Childs) {
			c->Insert(pCollider, pool);
		}
	}
}

void QuadNode::Query(AABB& range, std::vector<Collider*>& results, std::vector<Collider*>& seen){
	if (!m_Bounds.overlaps(range)) {
		return;
	}
	
	if (IsLeaf()) {
		for (auto& e : m_Entities) {
			if (!e->GetInQuerySeen()) {
				e->SetInQuerySeen(true);
				seen.push_back(e);
				results.push_back(e);
			}
		}
	}
	else {
		for (auto& c : m_Childs) {
			c->Query(range, results, seen);
		}
	}
}

void QuadNode::Clear(){
	m_Entities.clear();
	m_Childs[0] = nullptr;
	m_Childs[1] = nullptr;
	m_Childs[2] = nullptr;
	m_Childs[3] = nullptr;
}



void QuadNodePool::AddBlock(){
	m_Blocks.emplace_back(m_BlockSize);
	mp_CurrentBlock = &m_Blocks.back();
	m_Index = 0;
}

QuadNodePool::QuadNodePool(int32 size): m_BlockSize(size){
	AddBlock();
}

QuadNode* QuadNodePool::Get(float32 x, float32 y, float32 x2, float32 y2, int32 depth){
	if (m_Index >= static_cast<int32>(mp_CurrentBlock->size())) {
		AddBlock();
	}
	QuadNode* node = &(*mp_CurrentBlock)[m_Index++];
	node->m_Bounds = {x, y, x2, y2};
	node->m_Depth = depth;
	node->m_Entities.clear();
	node->m_Childs[0] = nullptr;
	node->m_Childs[1] = nullptr;
	node->m_Childs[2] = nullptr;
	node->m_Childs[3] = nullptr;
	return node;
}

void QuadNodePool::Reset(){
	m_Index = 0;
	if (m_Blocks.size() > 1) {
		m_Blocks.resize(1);
	}
	mp_CurrentBlock = &m_Blocks[0];
}


