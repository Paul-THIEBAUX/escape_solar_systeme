#pragma once 
#include <vector>

#include "include.h"
#include "Entity.h"
#include "Collider.h"
#include "Render/Shape.h"
#include "QuadTree.h"
#include "Mask.h"



struct EntityInfo
{
	Entity* entity = nullptr;
	bool toRemove = false;
};


struct CollisionInfo {
	float32 penetration = 0.f;
	Vector2f orientation{ 0.f, 0.f };
};

struct ContinuousCollisionHit
{
	bool hit = false;
	float32 time = 1.0f;
	Vector2f normal{ 0.0f, 0.0f };
};

enum class RepulseTypes {
	AABB,
	Circle,
	OOB,

	Count
};

class PhysicsManager
{
public:
	static PhysicsManager& GetInstance();
	static void DestroyInstance();

	void AddEntity(Entity* pEntity);
	void RemoveEntity(Entity* pEntity);
	void Update(float32 deltaTime);
	bool IsColliding(Collider* pCollider1, Collider* pCollider2);
	bool IsInside(Shape* renderShape, Vector2f positionToCheck);
	void ThrowRepulse(Collider* pCollider1, Collider* pCollider2);
	bool IsQuadTreeActive() const;

private:
	bool CheckAABBAABBCollision(gcle::Rectangle* pRect1, gcle::Rectangle* pRect2);
	bool CheckAABBCircleCollision(gcle::Rectangle* pRect, gcle::Circle* pCircle);
	bool CheckCircleCircleCollision(gcle::Circle* pCircle1, gcle::Circle* pCircle2);
	bool CheckOBBAABBCollision(gcle::Rectangle* pRect1, gcle::Rectangle* pRect2);
	bool CheckOBBOBBCollision(gcle::Rectangle* pRect1, gcle::Rectangle* pRect2);
	bool CheckOBBCircleCollision(gcle::Rectangle* pRect, gcle::Circle* pCircle);

private:
	bool ThrowCheckRectRect(gcle::Shape* pA, gcle::Shape* pB);
	bool ThrowCheckCircleCircle(gcle::Shape* pA, gcle::Shape* pB);
	bool ThrowCheckRectCircle(gcle::Shape* pA, gcle::Shape* pB);
	bool ThrowCheckCircleRect(gcle::Shape* pA, gcle::Shape* pB);

private:
	void RepulseRectRect(Collider* pA, Collider* pB);
	void RepulseCircleCircle(Collider* pA, Collider* pB);
	void RepulseRectCircle(Collider* pA, Collider* pB);
	void RepulseCircleRect(Collider* pA, Collider* pB);

	float32 GetRepulseCorrectionMultiplyer(Collider* pA, Collider* pB);
	void AccumulateCorrection(Entity* pEntity, Vector2f delta);
	void RepulseOBB(Collider* pColA, Collider* pColB);

private:
	bool ShouldUseContinuousCollision(Collider* pCollider) const;
	bool TryResolveContinuousCollision(Collider* pColliderA, Collider* pColliderB, float64 deltaTime);
	ContinuousCollisionHit SweepColliderAgainstAABB(Collider* pMovingCollider, Collider* pObstacleCollider, float64 deltaTime);
	ContinuousCollisionHit SegmentAABBIntersection(Vector2f start, Vector2f end, const AABB& target);
	void ApplyContinuousCollisionResponse(Collider* pMovingCollider, const ContinuousCollisionHit& hit, float64 deltaTime);
	AABB ComputeCurrentAABB(Collider* pCollider);
	AABB ExpandAABB(const AABB& aabb, Vector2f amount);
	AABB TranslateAABB(const AABB& aabb, Vector2f delta);
	AABB UnionAABB(const AABB& a, const AABB& b);
	AABB ComputePredictedAABB(Collider* pCollider, const AABB& currentAABB, float32 dt);
	bool TestAxis(float32 startAxis, float32 deltaAxis, float32 minAxis, float32 maxAxis, Vector2f negativeNormal, Vector2f positiveNormal);

public:
	void SetActivateQuadTree(bool activate);
	void SetDynamicQuadTreeSize(bool activate);
	void SetQuadTreePos1(Vector2f pos1);
	void SetQuadTreePos2(Vector2f pos2);
	void RebuildQuadTree();
	void SetFrameBetweenQuadTreeRegenerations(int8 nbrFrame);

private:
	void EntityToRemove(std::vector<EntityInfo>& m_EntitiesToRemove, std::vector<EntityInfo>& m_EntitiesToUpdate);
	void EntityToAdd(std::vector<EntityInfo>& m_EntitiesToAdd, std::vector<EntityInfo>& m_EntitiesToUpdate);

private:
	void EntityToUpdate(std::vector<Collider*>* pActiveColliders, std::vector<EntityInfo>& m_EntitiesToUpdate);
	void GenerateQuadTree(std::vector<Collider*>* pActiveColliders, float32 dt);
	void PendingCorrections();
	void HandleCollision(std::pair<Collider*, Collider*> collider, float32 dt);
	void MakeTreePairs(std::vector<Collider*>* pActiveColliders);
	void MakePairs(std::vector<Collider*>* pActiveColliders);

	void UpdateQuadTree(std::vector<Collider*>& activeColliders, float32 dt);
	void UpdateWithoutQuadTree(std::vector<Collider*> activeColliders, float32 dt);

private:
	std::vector<EntityInfo> m_EntitiesToAdd;
	std::vector<EntityInfo> m_EntitiesToUpdate;
	std::vector<EntityInfo> m_EntitiesToRemove;

private:
	std::unordered_map<Entity*, Vector2f> m_PendingCorrections;

private:
	using CollisionFn = bool(PhysicsManager::*)(gcle::Shape*, gcle::Shape*);
	static CollisionFn m_CollisionTable[static_cast<int32>(gcle::Shapes::Count) - 1][static_cast<int32>(gcle::Shapes::Count) - 1];

	using RepulseFn = void(PhysicsManager::*)(Collider* a, Collider* b);
	static RepulseFn m_RepulseTable[static_cast<int32>(RepulseTypes::Count)][static_cast<int32>(RepulseTypes::Count)];

private:
	CollisionInfo m_ColDatas;


private:
	bool m_ActivateQuadTree = true;
	bool m_DynamicQuadTreeSize = false;
	bool m_ForceQuadTreeRegen = false;

	Vector2f m_QuadTreePos1{ -50000, -50000 };
	Vector2f m_QuadTreePos2{ 50000, 50000 };

	QuadTree* mp_QuadTree = new QuadTree(m_QuadTreePos1.x, m_QuadTreePos1.y, m_QuadTreePos2.x, m_QuadTreePos2.y);

	int8 m_FrameBetweenQuadTreeRegenerations = 1;
	int8 m_TimeBetweenRegeneration = 0;

	std::vector<std::pair<Collider*, Collider*>> m_Pairs;
	std::vector<Collider*> m_QueryResult;

private: 
	float32 m_EntryTime = 0.0f;
	float32 m_ExitTime = 1.0f;
	Vector2f m_EntryNormal{ 0.0f, 0.0f };

private:
	static PhysicsManager* s_Instance;

public : 
	~PhysicsManager();

	Collider* mp_CurrentColliderA = nullptr;
	Collider* mp_CurrentColliderB = nullptr;
};