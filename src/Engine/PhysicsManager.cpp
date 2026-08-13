#include <algorithm>
#include <cmath>
#include <limits>
#include "PhysicsManager.h"
#include "SceneManager.h"
#include "MathGC.h"

#undef min
#undef max

namespace
{
	constexpr float32 PHYSICS_EPSILON = 0.00001f;

	float32 Dot(const Vector2f& a, const Vector2f& b)
	{
		return a.x * b.x + a.y * b.y;
	}

	Vector2f SafeNormal(Vector2f value, Vector2f fallback)
	{
		float32 lengthSquared = (value.x * value.x) + (value.y * value.y);
		if (lengthSquared <= PHYSICS_EPSILON * PHYSICS_EPSILON)
			return fallback;

		return value / std::sqrt(lengthSquared);
	}

	bool TestRectAxis(
		const Vector2f& axis,
		const Vector2f& centerDelta,
		const Vector2f axesA[2],
		const Vector2f axesB[2],
		const Vector2f& extentsA,
		const Vector2f& extentsB,
		float32& minOverlap,
		Vector2f& bestNormal)
	{
		Vector2f normal = SafeNormal(axis, { 1.0f, 0.0f });

		float32 distance = Dot(centerDelta, normal);
		float32 radiusA =
			extentsA.x * std::abs(Dot(axesA[0], normal)) +
			extentsA.y * std::abs(Dot(axesA[1], normal));
		float32 radiusB =
			extentsB.x * std::abs(Dot(axesB[0], normal)) +
			extentsB.y * std::abs(Dot(axesB[1], normal));

		float32 overlap = (radiusA + radiusB) - std::abs(distance);
		if (overlap <= 0.0f)
			return false;

		if (overlap < minOverlap)
		{
			minOverlap = overlap;
			bestNormal = normal * (distance >= 0.0f ? 1.0f : -1.0f); // A -> B
		}

		return true;
	}

	float32 StaticFactor(Entity* pEntity)
	{
		return pEntity->IsStatic() ? 0.0f : 1.0f;
	}

	bool ShouldBlockMovement(Entity* pEntity, Entity* pOtherEntity)
	{
		// Si les deux objets sont Kinematic, ils sont tous les deux deplacables :
		// on les repousse, mais on ne bloque pas l'input avec CollisionDirection
		// et on ne supprime pas leur vitesse.
		// On bloque uniquement quand l'objet touche un obstacle non Kinematic.
		return pEntity != nullptr &&
			pOtherEntity != nullptr &&
			!pEntity->IsStatic() &&
			pOtherEntity->IsStatic();
	}

	void ApplyBlockingResponse(Collider* pCollider, Collider* pOtherCollider, const Vector2f& correction)
	{
		if (pCollider == nullptr || pOtherCollider == nullptr)
			return;

		Entity* pEntity = pCollider->GetOwner();
		Entity* pOtherEntity = pOtherCollider->GetOwner();

		if (!ShouldBlockMovement(pEntity, pOtherEntity))
			return;

		// La correction indique la direction qui sort l'objet de la collision.
		// On retire seulement la vitesse qui continue a pousser dans l'obstacle,
		// et on pose les flags uniquement contre les objets immobiles.
		pEntity->GetRigidBody()->RemoveVelocityAlongNormal(correction);
		pCollider->CollidingOn(correction);
	}
}

PhysicsManager::CollisionFn PhysicsManager::m_CollisionTable[static_cast<int32>(gcle::Shapes::Count) - 1][static_cast<int32>(gcle::Shapes::Count) - 1]
{
	{
		&PhysicsManager::ThrowCheckRectRect,
		&PhysicsManager::ThrowCheckRectCircle,
	},
	{
		&PhysicsManager::ThrowCheckCircleRect,
		&PhysicsManager::ThrowCheckCircleCircle,
	}
};

PhysicsManager::RepulseFn PhysicsManager::m_RepulseTable[static_cast<int32>(RepulseTypes::Count)][static_cast<int32>(RepulseTypes::Count)]
{
	{
		&PhysicsManager::RepulseRectRect,
		&PhysicsManager::RepulseRectCircle,
		&PhysicsManager::RepulseOBB,
	},
	{
		&PhysicsManager::RepulseCircleRect,
		&PhysicsManager::RepulseCircleCircle,
		&PhysicsManager::RepulseOBB,
	},
	{
		&PhysicsManager::RepulseOBB,
		&PhysicsManager::RepulseOBB,
		&PhysicsManager::RepulseOBB
	}
};

PhysicsManager* PhysicsManager::s_Instance = nullptr;

PhysicsManager& PhysicsManager::GetInstance()
{
	if (s_Instance == nullptr)
		s_Instance = GCLE_NEW PhysicsManager();
	return *s_Instance;
}

void PhysicsManager::DestroyInstance()
{
	delete s_Instance;
	s_Instance = nullptr;
}

#pragma region EntityHandling

void PhysicsManager::AddEntity(Entity* pEntity)
{
	for (auto& info : m_EntitiesToUpdate)
	{
		if (info.entity->GetId() == pEntity->GetId())
			return; 
	}
	m_EntitiesToUpdate.push_back({ pEntity, false });
}

void PhysicsManager::RemoveEntity(Entity* pEntity)
{
	for (auto& info : m_EntitiesToUpdate)
	{
		if (info.toRemove) continue; 
		if (info.entity->GetId() == pEntity->GetId())
		{
			info.toRemove = true;
			m_ForceQuadTreeRegen = true;
			return;
		}
	}
}

void PhysicsManager::EntityToRemove(std::vector<EntityInfo>& m_EntitiesToRemove, std::vector<EntityInfo>& m_EntitiesToUpdate)
{
	for (EntityInfo entity : m_EntitiesToRemove) {
		for (int i = (int)m_EntitiesToUpdate.size() - 1; i >= 0; --i)
		{
			if (m_EntitiesToUpdate[i].entity == entity.entity) {
				m_EntitiesToUpdate.erase(m_EntitiesToUpdate.begin() + i);
			}
		}
	}
	m_EntitiesToRemove.clear();
}

void PhysicsManager::EntityToAdd(std::vector<EntityInfo>& m_EntitiesToAdd, std::vector<EntityInfo>& m_EntitiesToUpdate)
{
	for (EntityInfo entity : m_EntitiesToAdd) {
		m_EntitiesToUpdate.push_back(entity);
	}
	m_EntitiesToAdd.clear();

}

void PhysicsManager::EntityToUpdate(std::vector<Collider*>* pActiveColliders, std::vector<EntityInfo>& m_EntitiesToUpdate)
{
	for (auto& e : m_EntitiesToUpdate) {
		if (e.toRemove == false)
		{
			if (e.entity->IsActiveIn(SceneManager::GetInstance().GetCurrentSceneTag()))
			{
				for (auto& collider : e.entity->GetColliders())
				{
					if (collider->IsActive())
						pActiveColliders->push_back(collider);
				}
			}
		}
	}
}

#pragma endregion

#pragma region Update

void PhysicsManager::Update(float32 dt)
{
	for (int32 i = static_cast<int32>(m_EntitiesToUpdate.size()) - 1; i >= 0; i--)
	{
		if (m_EntitiesToUpdate[i].toRemove)
			m_EntitiesToUpdate.erase(m_EntitiesToUpdate.begin() + i);
	}

	EntityToRemove(m_EntitiesToRemove, m_EntitiesToUpdate);
	EntityToAdd(m_EntitiesToAdd, m_EntitiesToUpdate);

	std::vector<Collider*> activeColliders;

	EntityToUpdate(&activeColliders, m_EntitiesToUpdate);

	const std::string& currentScene = SceneManager::GetInstance().GetCurrentSceneTag();

	if (m_ActivateQuadTree == true) {
		UpdateQuadTree(activeColliders, dt);
	}

	else {
		UpdateWithoutQuadTree(activeColliders, dt);
	}
}

void PhysicsManager::UpdateQuadTree(std::vector<Collider*>& activeColliders, float32 dt)
{
	int32 nbrTest = 0;
	m_Pairs.clear();
	m_TimeBetweenRegeneration += 1;

	GenerateQuadTree(&activeColliders, dt);
	MakeTreePairs(&activeColliders);

	for (auto& pair : m_Pairs) 
	{
		nbrTest += 1;
		HandleCollision(pair, dt);
		PendingCorrections();
	}
	m_Pairs.clear();
}

void PhysicsManager::UpdateWithoutQuadTree(std::vector<Collider*> activeColliders, float32 dt)
{
	m_Pairs.clear();

	MakePairs(&activeColliders);

	for (auto& pair : m_Pairs)
	{

		HandleCollision(pair, dt);

		PendingCorrections();
	}

	for (int32 i = static_cast<int32>(m_EntitiesToUpdate.size()) - 1; i >= 0; i--)
	{
		if (m_EntitiesToUpdate[i].toRemove)
			m_EntitiesToUpdate.erase(m_EntitiesToUpdate.begin() + i);
	}

	m_Pairs.clear();
}

#pragma region Helpers

void PhysicsManager::GenerateQuadTree(std::vector<Collider*>* pActiveColliders, float32 dt)
{
	if (m_TimeBetweenRegeneration >= m_FrameBetweenQuadTreeRegenerations || m_ForceQuadTreeRegen)
	{
		mp_QuadTree->Clear();

		for (auto& collider : *pActiveColliders)
		{
			AABB currentAABB = ComputeCurrentAABB(collider);
			AABB finalAABB = currentAABB;

			if (ShouldUseContinuousCollision(collider))
			{
				AABB futureAABB = ComputePredictedAABB(collider, currentAABB, dt);
				finalAABB = UnionAABB(currentAABB, futureAABB);
			}

			collider->SetAABB(finalAABB);
			mp_QuadTree->Insert(collider);
		}

		m_TimeBetweenRegeneration = 0;
		m_ForceQuadTreeRegen = false; 
	}
}

void PhysicsManager::PendingCorrections()
{
	for (auto& pair : m_PendingCorrections)
	{
		Entity* pEntity = pair.first;
		Vector2f delta = pair.second;

		Vector2f current = pEntity->GetPosition();
		pEntity->SetPosition(current.x + delta.x, current.y + delta.y);
	}
	m_PendingCorrections.clear();
}

void PhysicsManager::MakePairs(std::vector<Collider*>* pActiveColliders) {
	for (size_t i = 0; i < pActiveColliders->size(); ++i) {
		Collider* colliderA = (*pActiveColliders)[i];
		for (size_t j = i + 1; j < pActiveColliders->size(); ++j) {
			Collider* colliderB = (*pActiveColliders)[j];
			bool greater = colliderA < colliderB;
			bool notTheSameEntity = colliderA->GetOwner()->GetId() != colliderB->GetOwner()->GetId();
			bool onSameLayer = (Mask::AreOnASameLayer<int32>(*colliderA->GetOwner()->GetCollisionLayer(), *colliderB->GetOwner()->GetCollisionLayer()));

			if (greater && notTheSameEntity && onSameLayer) {
				m_Pairs.push_back({ colliderA, colliderB });
			}
		}
	}
}

void PhysicsManager::MakeTreePairs(std::vector<Collider*>* pActiveColliders)
{
	for (auto& ColliderA : *pActiveColliders) {

		const auto& candidates = mp_QuadTree->Query(ColliderA);

		for (auto& ColliderB : candidates) {
			bool greater = ColliderA < ColliderB;
			bool notTheSameEntity = ColliderA->GetOwner()->GetId() != ColliderB->GetOwner()->GetId();
			bool onSameLayer = (Mask::AreOnASameLayer<int32>(*ColliderA->GetOwner()->GetCollisionLayer(), *ColliderB->GetOwner()->GetCollisionLayer()));

			if (greater && notTheSameEntity && onSameLayer) {
				m_Pairs.push_back({ ColliderA, ColliderB });
			}
		}
	}
}

void PhysicsManager::HandleCollision(std::pair<Collider*, Collider*> collider, float32 dt)
{
	Collider* colliderA = collider.first;
	Collider* colliderB = collider.second;

	Entity* entityA = colliderA->GetOwner();
	Entity* entityB = colliderB->GetOwner();

	bool coliding = IsColliding(colliderA, colliderB);
	bool isTriggerPair = colliderA->IsTrigger() || colliderB->IsTrigger(); 

	if (coliding)
	{
		if (isTriggerPair) 
		{
			if (!entityA->m_TriggeringEntity.contains(entityB->GetId()))
			{
				entityA->OnTriggerEnter(entityB);
				entityA->m_TriggeringEntity.insert({ entityB->GetId(), entityB });

				entityB->OnTriggerEnter(entityA);
				entityB->m_TriggeringEntity.insert({ entityA->GetId(), entityA });
			}
			else
			{
				entityA->OnTrigger(entityB);
				entityB->OnTrigger(entityA);
			}

			return;
		}

		if (entityA->IsRigidBody() && entityB->IsRigidBody())
		{
			ThrowRepulse(colliderA, colliderB);
		}

		if (!entityA->m_CollidingEntity.contains(entityB->GetId()))
		{
			entityA->OnCollisionEnter(entityB);
			entityA->m_CollidingEntity.insert({ entityB->GetId(), entityB });

			entityB->OnCollisionEnter(entityA);
			entityB->m_CollidingEntity.insert({ entityA->GetId(), entityA });
		}
		else
		{
			entityA->OnCollision(entityB);
			entityB->OnCollision(entityA);
		}

		return;
	}
	 
	bool needAntiTunneling = !isTriggerPair &&
		(ShouldUseContinuousCollision(colliderA) || ShouldUseContinuousCollision(colliderB));

	if (needAntiTunneling)
	{
		if (TryResolveContinuousCollision(colliderA, colliderB, dt))
		{
			return;
		}
	}
	 
	if (entityA->m_TriggeringEntity.contains(entityB->GetId()))
	{
		entityA->OnTriggerExit(entityB);
		entityA->m_TriggeringEntity.erase(entityB->GetId());

		entityB->OnTriggerExit(entityA);
		entityB->m_TriggeringEntity.erase(entityA->GetId());
	}

	if (entityA->m_CollidingEntity.contains(entityB->GetId()))
	{
		entityA->OnCollisionExit(entityB);
		entityA->m_CollidingEntity.erase(entityB->GetId());

		entityB->OnCollisionExit(entityA);
		entityB->m_CollidingEntity.erase(entityA->GetId());
	}
}

void PhysicsManager::AccumulateCorrection(Entity* pEntity, Vector2f delta)
{
	m_PendingCorrections[pEntity] += delta;
}

bool PhysicsManager::IsColliding(Collider* pCollider1, Collider* pCollider2)
{
	auto* shapeA = pCollider1->GetShape();
	auto* shapeB = pCollider2->GetShape();

	if (shapeA->GetShape() == gcle::Shapes::Triangle || shapeB->GetShape() == gcle::Shapes::Triangle)
	{
		return false;
	}

	int32 typeA = static_cast<int32>(shapeA->GetShape()) - 1;
	int32 typeB = static_cast<int32>(shapeB->GetShape()) - 1;
	int32 maxType = static_cast<int32>(gcle::Shapes::Count) - 1;

	if (typeA < 0 || typeB < 0 || typeA >= maxType || typeB >= maxType)
		return false;

	return (this->*m_CollisionTable[typeA][typeB])(shapeA, shapeB);
}

bool PhysicsManager::IsInside(Shape* renderShape, Vector2f positionToCheck)
{
	if (renderShape == nullptr)
		return false;

	switch (renderShape->GetShape())
	{
	case gcle::Shapes::Rectangle:
	{
		gcle::Rectangle* pRect = static_cast<gcle::Rectangle*>(renderShape);
		return
		{
			positionToCheck.x >= pRect->GetPosition().x - pRect->GetWidth() * 0.5f &&
			positionToCheck.x <= pRect->GetPosition().x + pRect->GetWidth() * 0.5f &&
			positionToCheck.y >= pRect->GetPosition().y - pRect->GetHeight() * 0.5f &&
			positionToCheck.y <= pRect->GetPosition().y + pRect->GetHeight() * 0.5f
		};
	}
	case gcle::Shapes::Circle:
	{
		gcle::Circle* pCircle = static_cast<gcle::Circle*>(renderShape);
		return pCircle->GetPosition().GetDistance(positionToCheck) <= pCircle->GetRadius();
	}
	default:
		break;
	}

	return false;
}

#pragma endregion

#pragma region ContinuousCollision

ContinuousCollisionHit PhysicsManager::SweepColliderAgainstAABB(Collider* pMovingCollider, Collider* pObstacleCollider, float64 dt)
{
	if (pMovingCollider == nullptr || pObstacleCollider == nullptr)
		return {};

	Entity* movingEntity = pMovingCollider->GetOwner();
	if (movingEntity == nullptr || !movingEntity->IsRigidBody())
		return {};

	AABB movingAABB = ComputeCurrentAABB(pMovingCollider);
	AABB obstacleAABB = ComputeCurrentAABB(pObstacleCollider);

	float32 movingWidth = movingAABB.maxX - movingAABB.minX;
	float32 movingHeight = movingAABB.maxY - movingAABB.minY;

	if (movingWidth <= PHYSICS_EPSILON || movingHeight <= PHYSICS_EPSILON)
		return {};

	Vector2f movingHalfSize{
		movingWidth * 0.5f,
		movingHeight * 0.5f
	};

	AABB expandedObstacleAABB = ExpandAABB(obstacleAABB, movingHalfSize);

	Vector2f start = pMovingCollider->GetShape()->GetPosition(0.5f, 0.5f);
	Vector2f velocity = movingEntity->GetRigidBody()->GetVelocity();
	Vector2f end = start + velocity * static_cast<float32>(dt);

	return SegmentAABBIntersection(start, end, expandedObstacleAABB);
}

ContinuousCollisionHit PhysicsManager::SegmentAABBIntersection(Vector2f start, Vector2f end, const AABB& target)
{
	m_EntryTime = 0.0f;
	m_ExitTime = 1.0f;
	m_EntryNormal = { 0.0f, 0.0f };

	Vector2f delta = end - start;

	if (std::abs(delta.x) <= PHYSICS_EPSILON && std::abs(delta.y) <= PHYSICS_EPSILON)
		return {};

	if (!TestAxis(start.x, delta.x, target.minX, target.maxX, { -1.0f, 0.0f }, { 1.0f, 0.0f }))
		return {};

	if (!TestAxis(start.y, delta.y, target.minY, target.maxY, { 0.0f, -1.0f }, { 0.0f, 1.0f }))
		return {};

	if (m_EntryTime < 0.0f || m_EntryTime > 1.0f)
		return {};

	ContinuousCollisionHit hit;
	hit.hit = true;
	hit.time = m_EntryTime;
	hit.normal = m_EntryNormal;

	return hit;
}

AABB PhysicsManager::ComputeCurrentAABB(Collider* pCollider)
{
	if (pCollider == nullptr || pCollider->GetShape() == nullptr)
		return {};

	gcle::Shape* shape = pCollider->GetShape();

	if (shape->GetShape() == gcle::Shapes::Circle)
	{
		Vector2f min = shape->GetPosition(0.0f, 0.0f);
		Vector2f max = shape->GetPosition(1.0f, 1.0f);

		return {
			std::min(min.x, max.x),
			std::min(min.y, max.y),
			std::max(min.x, max.x),
			std::max(min.y, max.y)
		};
	}

	Degrees angle = shape->GetRotation();

	if (static_cast<int32>(angle) % 180 == 0)
	{
		Vector2f pos1 = shape->GetPosition(0.0f, 0.0f);
		Vector2f pos2 = shape->GetPosition(1.0f, 1.0f);

		return {
			std::min(pos1.x, pos2.x),
			std::min(pos1.y, pos2.y),
			std::max(pos1.x, pos2.x),
			std::max(pos1.y, pos2.y)
		};
	}

	return GetRotatedAABB(
		shape->GetPosition(),
		{
			shape->GetWidth(),
			shape->GetHeight()
		},
		angle * DEG_TO_RAD
	);
}

AABB PhysicsManager::ComputePredictedAABB(Collider* pCollider, const AABB& currentAABB, float32 dt)
{
	Entity* entity = pCollider->GetOwner();

	if (entity == nullptr || !entity->IsRigidBody())
		return currentAABB;

	Vector2f motion = entity->GetRigidBody()->GetVelocity() * dt;
	return TranslateAABB(currentAABB, motion);
}

bool PhysicsManager::TestAxis(float32 startAxis, float32 deltaAxis, float32 minAxis, float32 maxAxis, Vector2f negativeNormal, Vector2f positiveNormal)
{
	if (std::abs(deltaAxis) <= PHYSICS_EPSILON)
	{
		return startAxis >= minAxis && startAxis <= maxAxis;
	}

	float32 invDelta = 1.0f / deltaAxis;
	float32 axisEntry = (minAxis - startAxis) * invDelta;
	float32 axisExit = (maxAxis - startAxis) * invDelta;
	Vector2f axisNormal = negativeNormal;

	if (axisEntry > axisExit)
	{
		std::swap(axisEntry, axisExit);
		axisNormal = positiveNormal;
	}

	if (axisEntry > m_EntryTime)
	{
		m_EntryTime = axisEntry;
		m_EntryNormal = axisNormal;
	}

	m_ExitTime = std::min(m_ExitTime, axisExit);
	return m_EntryTime <= m_ExitTime;
}

AABB PhysicsManager::UnionAABB(const AABB& a, const AABB& b)
{
	return {
		std::min(a.minX, b.minX),
		std::min(a.minY, b.minY),
		std::max(a.maxX, b.maxX),
		std::max(a.maxY, b.maxY)
	};
}

AABB PhysicsManager::ExpandAABB(const AABB& aabb, Vector2f amount)
{
	return {
		aabb.minX - amount.x,
		aabb.minY - amount.y,
		aabb.maxX + amount.x,
		aabb.maxY + amount.y
	};
}

bool PhysicsManager::ShouldUseContinuousCollision(Collider* pCollider) const
{
	if (pCollider == nullptr)
		return false;

	Entity* entity = pCollider->GetOwner();

	if (entity == nullptr)
		return false;

	if (!entity->IsRigidBody())
		return false;

	return entity->GetRigidBody()->UseContinuousCollision();
}

AABB PhysicsManager::TranslateAABB(const AABB& aabb, Vector2f delta)
{
	return {
		aabb.minX + delta.x,
		aabb.minY + delta.y,
		aabb.maxX + delta.x,
		aabb.maxY + delta.y
	};
}

bool PhysicsManager::TryResolveContinuousCollision(Collider* pColliderA, Collider* pColliderB, float64 dt)
{
	if (ShouldUseContinuousCollision(pColliderA))
	{
		ContinuousCollisionHit hitA = SweepColliderAgainstAABB(pColliderA, pColliderB, dt);

		if (hitA.hit)
		{
			ApplyContinuousCollisionResponse(pColliderA, hitA, dt);
			return true;
		}
	}

	if (ShouldUseContinuousCollision(pColliderB))
	{
		ContinuousCollisionHit hitB = SweepColliderAgainstAABB(pColliderB, pColliderA, dt);

		if (hitB.hit)
		{
			ApplyContinuousCollisionResponse(pColliderB, hitB, dt);
			return true;
		}
	}

	return false;
}

void PhysicsManager::ApplyContinuousCollisionResponse(Collider* pMovingCollider, const ContinuousCollisionHit& hit, float64 dt)
{
	if (pMovingCollider == nullptr || !hit.hit)
		return;

	Entity* movingEntity = pMovingCollider->GetOwner();
	if (movingEntity == nullptr || !movingEntity->IsRigidBody())
		return;

	Vector2f start = pMovingCollider->GetShape()->GetPosition(0.5f, 0.5f);
	Vector2f velocity = movingEntity->GetRigidBody()->GetVelocity();
	Vector2f motion = velocity * static_cast<float32>(dt);

	float32 safeTime = std::max(0.0f, hit.time - 0.001f);
	Vector2f safeCenter = start + motion * safeTime;

	Vector2f ownerCenter = movingEntity->GetPosition();
	Vector2f colliderCenter = pMovingCollider->GetShape()->GetPosition(0.5f, 0.5f);
	Vector2f ownerToCollider = colliderCenter - ownerCenter;
	Vector2f safeOwnerCenter = safeCenter - ownerToCollider;

	movingEntity->SetPosition(safeOwnerCenter.x, safeOwnerCenter.y);
	movingEntity->GetRigidBody()->RemoveVelocityAlongNormal(hit.normal);
	pMovingCollider->CollidingOn(hit.normal);
}

#pragma endregion

#pragma endregion

#pragma region Checks

#pragma region ThrowChecks

bool PhysicsManager::ThrowCheckRectRect(gcle::Shape* pA, gcle::Shape* pB)
{
	int16 angleA = static_cast<int16>(pA->GetTransform()->GetDegAngle());
	int16 angleB = static_cast<int16>(pB->GetTransform()->GetDegAngle());


	if (angleA != 0) {
		if (angleB != 0)
			return CheckOBBOBBCollision(static_cast<gcle::Rectangle*>(pA), static_cast<gcle::Rectangle*>(pB));
		else
			return CheckOBBAABBCollision(static_cast<gcle::Rectangle*>(pA), static_cast<gcle::Rectangle*>(pB));
	}
	else if (angleB != 0) {
		bool hit = CheckOBBAABBCollision(static_cast<gcle::Rectangle*>(pB), static_cast<gcle::Rectangle*>(pA));
		if (hit) {
			m_ColDatas.orientation = -m_ColDatas.orientation;
		}
		return hit;
	}


	return CheckAABBAABBCollision(static_cast<gcle::Rectangle*>(pA), static_cast<gcle::Rectangle*>(pB));
}

bool PhysicsManager::ThrowCheckCircleCircle(gcle::Shape* pA, gcle::Shape* pB)
{
	return CheckCircleCircleCollision(static_cast<gcle::Circle*>(pA), static_cast<gcle::Circle*>(pB));
}

bool PhysicsManager::ThrowCheckRectCircle(gcle::Shape* pA, gcle::Shape* pB)
{
	int16 angle = static_cast<int16>(pA->GetTransform()->GetDegAngle()) % 180;

	if (angle != 0)
	{
		return CheckOBBCircleCollision(static_cast<gcle::Rectangle*>(pA), static_cast<gcle::Circle*>(pB));
	}
	else
	{
		return CheckAABBCircleCollision(static_cast<gcle::Rectangle*>(pA), static_cast<gcle::Circle*>(pB));
	}
}

bool PhysicsManager::ThrowCheckCircleRect(gcle::Shape* pA, gcle::Shape* pB)
{
	int16 angle = static_cast<int16>(pB->GetTransform()->GetDegAngle()) % 180;
	if (angle != 0) {
		bool hit = CheckOBBCircleCollision(static_cast<gcle::Rectangle*>(pB), static_cast<gcle::Circle*>(pA));
		if (hit)
			m_ColDatas.orientation = -m_ColDatas.orientation;
		return hit;

	}
	return CheckAABBCircleCollision(static_cast<gcle::Rectangle*>(pB), static_cast<gcle::Circle*>(pA));
}

#pragma endregion

#pragma region ActualsChecks

bool PhysicsManager::CheckAABBAABBCollision(gcle::Rectangle* pRect1, gcle::Rectangle* pRect2)
{
	float32 x1 = pRect1->GetPosition(0, 0).x;
	float32 y1 = pRect1->GetPosition(0, 0).y;
	float32 w1 = pRect1->GetWidth();
	float32 h1 = pRect1->GetHeight();

	float32 x2 = pRect2->GetPosition(0, 0).x;
	float32 y2 = pRect2->GetPosition(0, 0).y;
	float32 w2 = pRect2->GetWidth();
	float32 h2 = pRect2->GetHeight();


	return x1 + w1 >= x2 &&
		x1 <= x2 + w2 &&
		y1 + h1 >= y2 &&
		y1 <= y2 + h2;
}

bool PhysicsManager::CheckAABBCircleCollision(gcle::Rectangle* pRect, gcle::Circle* pCircle)
{
	float32 rx = pRect->GetPosition(0.0f, 0.0f).x;
	float32 ry = pRect->GetPosition(0.0f, 0.0f).y;
	float32 rw = pRect->GetWidth();
	float32 rh = pRect->GetHeight();

	float32 cx = pCircle->GetPosition().x;
	float32 cy = pCircle->GetPosition().y;

	float32 testX = cx;
	float32 testY = cy;

	if (cx < rx)
		testX = rx;
	else if (cx > rx + rw)
		testX = rx + rw;

	if (cy < ry)
		testY = ry;
	else if (cy > ry + rh)
		testY = ry + rh;

	Vector2f test({ testX, testY });

	float32 dist = pCircle->GetPosition().GetDistance(test);

	return (dist <= pCircle->GetRadius());
}

bool PhysicsManager::CheckCircleCircleCollision(gcle::Circle* pCircle1, gcle::Circle* pCircle2)
{
	Vector2f pos1 = pCircle1->GetPosition();
	Vector2f pos2 = pCircle2->GetPosition();
	float32 distance = pos1.GetDistance(pos2);
	return (distance <= (pCircle1->GetRadius() + pCircle2->GetRadius()));
}

bool PhysicsManager::CheckOBBAABBCollision(gcle::Rectangle* pRect1, gcle::Rectangle* pRect2) {

	Vector2f axes[2];

	float32 radRotation = pRect1->GetTransform()->GetRadAngle();
	axes[0] = { std::cos(radRotation), std::sin(radRotation) };
	axes[1] = { -std::sin(radRotation), std::cos(radRotation) };


	//OBB values
	Vector2f obbExtents{ pRect1->GetWidth() / 2, pRect1->GetHeight() / 2 };

	//AABB values
	Vector2f aabbExtents{ pRect2->GetWidth() / 2, pRect2->GetHeight() / 2 };

	Vector2f T = pRect2->GetPosition() - pRect1->GetPosition();

	Matrix relativeRotation(2, 2);
	Matrix absoluteRotation(2, 2);


	const float64 Epsilon = 1e-16;

	for (int i = 0; i < 2; i++) {
		relativeRotation.matrix[0][i] = axes[i].x;
		relativeRotation.matrix[1][i] = axes[i].y;

		for (int j = 0; j < 2; j++) {
			absoluteRotation.matrix[i][j] = std::abs(relativeRotation.matrix[j][i]) + static_cast<float32>(Epsilon);
		}
	}


	float ra = 0.f, rb = 0.f;

	float32 minOverlap = std::numeric_limits<float32>::max();
	Vector2f collisionNormal;


	ra = aabbExtents.x;
	rb = obbExtents.x * absoluteRotation.matrix[0][0] + obbExtents.y * absoluteRotation.matrix[0][1];
	float32 overlapX = (ra + rb) - std::abs(T.x);
	if (overlapX <= 0.f)
		return false;

	if (overlapX < minOverlap) {
		minOverlap = overlapX;
		collisionNormal = { (T.x > 0.f ? 1.f : -1.f), 0.f };
	}

	ra = aabbExtents.y;
	rb = obbExtents.x * absoluteRotation.matrix[1][0] + obbExtents.y * absoluteRotation.matrix[1][1];
	float32 overlapY = (ra + rb) - std::abs(T.y);
	if (overlapY <= 0.f)
		return false;

	if (overlapY < minOverlap) {
		minOverlap = overlapY;
		collisionNormal = { 0.f, (T.y > 0.f ? 1.f : -1.f) };
	}




	ra = aabbExtents.x * absoluteRotation.matrix[0][0] + aabbExtents.y * absoluteRotation.matrix[0][1];
	rb = obbExtents.x;
	float32 t_obbX = T.x * relativeRotation.matrix[0][0] + T.y * relativeRotation.matrix[1][0];
	float32 overlapObbX = (ra + rb) - std::abs(t_obbX);
	if (overlapObbX <= 0.f)
		return false;

	if (overlapObbX < minOverlap) {
		minOverlap = overlapObbX;
		float32 sign = (t_obbX > 0.f) ? 1.f : -1.f;
		collisionNormal = { axes[0].x * sign, axes[0].y * sign };
	}

	ra = aabbExtents.x * absoluteRotation.matrix[1][0] + aabbExtents.y * absoluteRotation.matrix[1][1];
	rb = obbExtents.y;
	float32 t_obbY = T.x * relativeRotation.matrix[0][1] + T.y * relativeRotation.matrix[1][1];
	float32 overlapObbY = (ra + rb) - std::abs(t_obbY);
	if (overlapObbY <= 0.f)
		return false;

	if (overlapObbY < minOverlap) {
		minOverlap = overlapObbY;
		float32 sign = (t_obbY > 0.f) ? 1.f : -1.f;
		collisionNormal = { axes[1].x * sign, axes[1].y * sign };
	}

	m_ColDatas.orientation = collisionNormal;
	m_ColDatas.penetration = minOverlap;

	return true;
}

bool PhysicsManager::CheckOBBOBBCollision(gcle::Rectangle* pRect1, gcle::Rectangle* pRect2) {
	Vector2f axes1[2];
	Vector2f axes2[2];

	float32 radRotation1 = pRect1->GetTransform()->GetRadAngle();
	float32 radRotation2 = pRect2->GetTransform()->GetRadAngle();

	axes1[0] = { std::cos(radRotation1), std::sin(radRotation1) };
	axes1[1] = { -std::sin(radRotation1), std::cos(radRotation1) };

	axes2[0] = { std::cos(radRotation2), std::sin(radRotation2) };
	axes2[1] = { -std::sin(radRotation2), std::cos(radRotation2) };


	Vector2f extents1{ pRect1->GetWidth() / 2, pRect1->GetHeight() / 2 };
	Vector2f extents2{ pRect2->GetWidth() / 2, pRect2->GetHeight() / 2 };

	Vector2f T = pRect2->GetPosition() - pRect1->GetPosition();

	Matrix relativeRotation(2, 2);
	Matrix absoluteRotation(2, 2);

	const float64 Epsilon = 1e-16;

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			relativeRotation.matrix[j][i] = axes1[i].x * axes2[j].x + axes1[i].y * axes2[j].y;
			absoluteRotation.matrix[i][j] = std::abs(relativeRotation.matrix[j][i]) + static_cast<float32>(Epsilon);
		}
	}


	float32 minOverlap = std::numeric_limits<float32>::max();
	Vector2f collisionNormal;


	float32 ra = 0.f, rb = 0.f;
	float32 sign;


	ra = extents1.x;
	rb = extents2.x * absoluteRotation.matrix[0][0] + extents2.y * absoluteRotation.matrix[0][1];
	float32 t1_X = T.x * axes1[0].x + T.y * axes1[0].y;
	float32 overlap = (ra + rb) - std::abs(t1_X);
	if (overlap <= 0.f)
		return false;

	if (overlap < minOverlap) {
		minOverlap = overlap;
		sign = (t1_X > 0.f) ? 1.f : -1.f;
		collisionNormal = { axes1[0].x * sign, axes1[0].y * sign };
	}

	ra = extents1.y;
	rb = extents2.x * absoluteRotation.matrix[1][0] + extents2.y * absoluteRotation.matrix[1][1];
	float32 t1_Y = T.x * axes1[1].x + T.y * axes1[1].y;
	overlap = (ra + rb) - std::abs(t1_Y);
	if (overlap <= 0.f)
		return false;

	if (overlap < minOverlap) {
		minOverlap = overlap;
		sign = (t1_Y > 0.f) ? 1.f : -1.f;
		collisionNormal = { axes1[1].x * sign, axes1[1].y * sign };
	}

	ra = extents1.x * absoluteRotation.matrix[0][0] + extents1.y * absoluteRotation.matrix[1][0];
	rb = extents2.x;
	float32 t2_X = T.x * axes2[0].x + T.y * axes2[0].y;
	overlap = (ra + rb) - std::abs(t2_X);
	if (overlap <= 0.f)
		return false;

	if (overlap < minOverlap) {
		minOverlap = overlap;
		sign = (t2_X > 0.f) ? 1.f : -1.f;
		collisionNormal = { axes2[0].x * sign, axes2[0].y * sign };
	}


	ra = extents1.x * absoluteRotation.matrix[0][1] + extents1.y * absoluteRotation.matrix[1][1];
	rb = extents2.y;
	float32 t2_Y = T.x * axes2[1].x + T.y * axes2[1].y;
	overlap = (ra + rb) - std::abs(t2_Y);
	if (overlap <= 0.f)
		return false;

	if (overlap < minOverlap) {
		minOverlap = overlap;
		sign = (t2_Y > 0.f) ? 1.f : -1.f;
		collisionNormal = { axes2[1].x * sign, axes2[1].y * sign };
	}

	m_ColDatas.penetration = minOverlap;
	m_ColDatas.orientation = collisionNormal;

	return true;
}

bool PhysicsManager::CheckOBBCircleCollision(gcle::Rectangle* pRect, gcle::Circle* pCircle) {

	Vector2f rectPos = pRect->GetPosition();
	Vector2f extents{ pRect->GetWidth() / 2 , pRect->GetHeight() / 2 };
	float32 angle = pRect->GetTransform()->GetRadAngle();

	Vector2f circlePos = pCircle->GetPosition();
	float32 radius = pCircle->GetRadius();

	Vector2f T = circlePos - rectPos;

	float32 cos = std::cos(angle);
	float32 sin = std::sin(angle);

	Vector2f localCirclePos;
	localCirclePos.x = T.x * cos + T.y * sin;
	localCirclePos.y = -T.x * sin + T.y * cos;

	Vector2f closestPoint;
	closestPoint.x = std::max(-extents.x, std::min(localCirclePos.x, extents.x));
	closestPoint.y = std::max(-extents.y, std::min(localCirclePos.y, extents.y));

	float32 deltaX = localCirclePos.x - closestPoint.x;
	float32 deltaY = localCirclePos.y - closestPoint.y;

	float32 distanceCarree = (deltaX * deltaX) + (deltaY * deltaY);

	if (distanceCarree > (radius * radius))
		return false;

	float32 distance = (distanceCarree == 0.f) ? 0.f : std::sqrt(distanceCarree);
	Vector2f localNormal;
	m_ColDatas.penetration = radius - distance;

	if (distance == 0.f) {
		localNormal = { 1.f, 1.f };
		m_ColDatas.penetration = radius;
	}
	else {
		localNormal = { deltaX / distance, deltaY / distance };
	}

	Vector2f worldNormal;
	worldNormal.x = localNormal.x * cos - localNormal.y * sin;
	worldNormal.y = localNormal.x * sin + localNormal.y * cos;

	m_ColDatas.orientation = worldNormal;

	if (distanceCarree <= (radius * radius)) {
		return true;
	}
	return false;

}

#pragma endregion

#pragma endregion

#pragma region Repulse

#pragma region ThrowRepulse

void PhysicsManager::ThrowRepulse(Collider* pCollider1, Collider* pCollider2)
{
	auto* shapeA = pCollider1->GetShape();
	auto* shapeB = pCollider2->GetShape();

	mp_CurrentColliderA = pCollider1;
	mp_CurrentColliderB = pCollider2;

	RepulseTypes typeA;
	RepulseTypes typeB;

	switch (shapeA->GetShape()) {
	case gcle::Shapes::Circle:
		typeA = RepulseTypes::Circle;
		break;
	case gcle::Shapes::Rectangle:
		if (static_cast<int32>(shapeA->GetTransform()->GetDegAngle()) % 180 != 0)
			typeA = RepulseTypes::OOB;
		else
			typeA = RepulseTypes::AABB;
		break;
	case gcle::Shapes::Triangle:
		return;
		break;
	}

	switch (shapeB->GetShape()) {
	case gcle::Shapes::Circle:
		typeB = RepulseTypes::Circle;
		break;
	case gcle::Shapes::Rectangle:
		if (static_cast<int32>(shapeB->GetTransform()->GetDegAngle()) % 180 != 0)
			typeB = RepulseTypes::OOB;
		else
			typeB = RepulseTypes::AABB;
		break;
	case gcle::Shapes::Triangle:
		return;
		break;
	}


	(this->*m_RepulseTable[static_cast<int32>(typeA)][static_cast<int32>(typeB)])(pCollider1, pCollider2);
}

#pragma endregion

#pragma region ActualRepulse

void PhysicsManager::RepulseRectRect(Collider* pColA, Collider* pColB)
{
	Entity* a = pColA->GetOwner();
	Entity* b = pColB->GetOwner();

	gcle::Rectangle* pRect1 = static_cast<gcle::Rectangle*>(pColA->GetShape());
	gcle::Rectangle* pRect2 = static_cast<gcle::Rectangle*>(pColB->GetShape());

	float32 x1 = pRect1->GetPosition(0.0f, 0.0f).x;
	float32 y1 = pRect1->GetPosition(0.0f, 0.0f).y;
	float32 w1 = pRect1->GetWidth();
	float32 h1 = pRect1->GetHeight();

	float32 x2 = pRect2->GetPosition(0.0f, 0.0f).x;
	float32 y2 = pRect2->GetPosition(0.0f, 0.0f).y;
	float32 w2 = pRect2->GetWidth();
	float32 h2 = pRect2->GetHeight();

	float32 overlapX = std::min(x1 + w1, x2 + w2) - std::max(x1, x2);
	float32 overlapY = std::min(y1 + h1, y2 + h2) - std::max(y1, y2);

	if (overlapX <= 0.0f || overlapY <= 0.0f)
		return;

	Vector2f delta1 = { 0.f, 0.f };
	Vector2f delta2 = { 0.f, 0.f };

	float32 correctionMultiplyer = GetRepulseCorrectionMultiplyer(pColA, pColB);

	if (overlapX < overlapY)
	{
		float32 correction = overlapX * correctionMultiplyer;

		if (x1 < x2)
		{
			delta1.x -= correction * !a->IsStatic();
			delta2.x += correction * !b->IsStatic();

		}
		else
		{
			delta1.x += correction * !a->IsStatic();
			delta2.x -= correction * !b->IsStatic();

		}

		if (pColA->GetOwner()->IsStatic() || pColB->GetOwner()->IsStatic())
		{
			a->GetRigidBody()->ZeroVelocityX();
			b->GetRigidBody()->ZeroVelocityX();
		}
	}
	else
	{
		float32 correction = overlapY * correctionMultiplyer;

		if (y1 < y2)
		{
			delta1.y -= correction * !a->IsStatic();
			delta2.y += correction * !b->IsStatic();

		}
		else
		{
			delta1.y += correction * !a->IsStatic();
			delta2.y -= correction * !b->IsStatic();

		}

		if (a->IsStatic() || b->IsStatic())
		{
			a->GetRigidBody()->ZeroVelocityY();
			b->GetRigidBody()->ZeroVelocityY();
		}
	}

	ApplyBlockingResponse(pColA, pColB, delta1);
	ApplyBlockingResponse(pColB, pColA, delta2);

	AccumulateCorrection(a, delta1);
	AccumulateCorrection(b, delta2);

}

void PhysicsManager::RepulseCircleCircle(Collider* pColA, Collider* pColB)
{
	gcle::Shape* a = pColA->GetShape();
	gcle::Shape* b = pColB->GetShape();

	Vector2f distance = a->GetPosition(0.5f, 0.5f) - b->GetPosition(0.5f, 0.5f);
	float32 sqrLength = (distance.x * distance.x) + (distance.y * distance.y);
	float32 length = std::sqrt(sqrLength);

	float32 radius1 = a->GetRadius();
	float32 radius2 = b->GetRadius();
	float32 penetration = (radius1 + radius2) - length;

	if (penetration <= 0.0f)
		return;

	Vector2f normal = SafeNormal(distance, { 1.0f, 0.0f }); // direction B -> A
	Vector2f translation = normal * penetration * GetRepulseCorrectionMultiplyer(pColA, pColB);

	Vector2f delta1 = translation * StaticFactor(pColA->GetOwner());
	Vector2f delta2 = -translation * StaticFactor(pColB->GetOwner());

	AccumulateCorrection(pColA->GetOwner(), delta1);
	AccumulateCorrection(pColB->GetOwner(), delta2);

	ApplyBlockingResponse(pColA, pColB, delta1);
	ApplyBlockingResponse(pColB, pColA, delta2);
}

void PhysicsManager::RepulseRectCircle(Collider* pColA, Collider* pColB)
{
	gcle::Shape* a = pColA->GetShape();
	gcle::Shape* b = pColB->GetShape();

	gcle::Rectangle* pRect = static_cast<gcle::Rectangle*>(a);
	gcle::Circle* pCircle = static_cast<gcle::Circle*>(b);

	float32 rx = pRect->GetPosition(0.0f, 0.0f).x;
	float32 ry = pRect->GetPosition(0.0f, 0.0f).y;
	float32 rw = pRect->GetWidth();
	float32 rh = pRect->GetHeight();

	Vector2f circlePos = pCircle->GetPosition(0.5f, 0.5f);
	Vector2f rectPos = pRect->GetPosition(0.5f, 0.5f);

	float32 nearestX = std::max(rx, std::min(circlePos.x, rx + rw));
	float32 nearestY = std::max(ry, std::min(circlePos.y, ry + rh));
	Vector2f nearest({ nearestX, nearestY });

	Vector2f delta = circlePos - nearest;
	float32 dist = delta.x * delta.x + delta.y * delta.y;

	float32 correctionMultiplyer = GetRepulseCorrectionMultiplyer(pColA, pColB);

	if (dist == 0.0f)
	{
		float32 overlapL = circlePos.x - rx;
		float32 overlapR = (rx + rw) - circlePos.x;
		float32 overlapT = circlePos.y - ry;
		float32 overlapB = (ry + rh) - circlePos.y;

		float32 minOverlap = std::min({ overlapL, overlapR, overlapT, overlapB });
		Vector2f newPos = circlePos;
		Vector2f normal({ 0.0f, 0.0f });

		if (minOverlap == overlapL) { newPos.x = rx - pCircle->GetRadius();       normal = { -1.0f,  0.0f }; }
		else if (minOverlap == overlapR) { newPos.x = rx + rw + pCircle->GetRadius();  normal = { 1.0f,  0.0f }; }
		else if (minOverlap == overlapT) { newPos.y = ry - pCircle->GetRadius();       normal = { 0.0f, -1.0f }; }
		else { newPos.y = ry + rh + pCircle->GetRadius(); normal = { 0.0f,  1.0f }; }

		Vector2f translation = (newPos - circlePos) * correctionMultiplyer;
		Vector2f deltaRect = -translation * StaticFactor(pColA->GetOwner());
		Vector2f deltaCircle = translation * StaticFactor(pColB->GetOwner());

		ApplyBlockingResponse(pColA, pColB, deltaRect);
		ApplyBlockingResponse(pColB, pColA, deltaCircle);

		AccumulateCorrection(pColA->GetOwner(), deltaRect);
		AccumulateCorrection(pColB->GetOwner(), deltaCircle);

		return;
	}

	float32 length = std::sqrt(dist);
	Vector2f normal = delta / length;
	float32  overlap = (pCircle->GetRadius() - length) * correctionMultiplyer;

	Vector2f translation = normal * overlap;
	Vector2f deltaRect = -translation * StaticFactor(pColA->GetOwner());
	Vector2f deltaCircle = translation * StaticFactor(pColB->GetOwner());

	ApplyBlockingResponse(pColA, pColB, deltaRect);
	ApplyBlockingResponse(pColB, pColA, deltaCircle);

	AccumulateCorrection(pColA->GetOwner(), deltaRect);
	AccumulateCorrection(pColB->GetOwner(), deltaCircle);
}

void PhysicsManager::RepulseCircleRect(Collider* pColA, Collider* pColB)
{
	std::swap(mp_CurrentColliderA, mp_CurrentColliderB);
	RepulseRectCircle(pColB, pColA);
	std::swap(mp_CurrentColliderA, mp_CurrentColliderB);
}

void PhysicsManager::RepulseOBB(Collider* pColA, Collider* pColB)
{
	gcle::Shape* a = pColA->GetShape();
	gcle::Shape* b = pColB->GetShape();

	Vector2f normal = SafeNormal(m_ColDatas.orientation, { 1.0f, 0.0f });
	float32 penetration = std::max(static_cast<float32>(m_ColDatas.penetration), 0.0f);

	if (penetration <= 0.0f)
		return;

	Vector2f correction = normal * penetration * GetRepulseCorrectionMultiplyer(pColA, pColB);

	Vector2f deltaA = -correction * StaticFactor(pColA->GetOwner());
	Vector2f deltaB = correction * StaticFactor(pColB->GetOwner());

	ApplyBlockingResponse(pColA, pColB, deltaA);
	ApplyBlockingResponse(pColB, pColA, deltaB);

	AccumulateCorrection(pColA->GetOwner(), deltaA);
	AccumulateCorrection(pColB->GetOwner(), deltaB);
}

#pragma endregion

#pragma endregion

float32 PhysicsManager::GetRepulseCorrectionMultiplyer(Collider* pColA, Collider* pColB)
{
	gcle::Shape* a = pColA->GetShape();
	gcle::Shape* b = pColB->GetShape();

	if (!pColA->GetOwner()->IsStatic() && !pColB->GetOwner()->IsStatic())
		return 0.5;
	else
		return 1.0;
}

#pragma region Sets

void PhysicsManager::SetActivateQuadTree(bool activate) {
	m_ActivateQuadTree = activate;
}

void PhysicsManager::SetDynamicQuadTreeSize(bool activate) {
	m_DynamicQuadTreeSize = activate;
}

void PhysicsManager::SetQuadTreePos1(Vector2f pos1) {
	m_QuadTreePos1 = pos1;
	RebuildQuadTree();
}

void PhysicsManager::SetQuadTreePos2(Vector2f pos2) {
	m_QuadTreePos2 = pos2;
	RebuildQuadTree();
}

void PhysicsManager::RebuildQuadTree()
{
	delete mp_QuadTree;
	mp_QuadTree = new QuadTree(m_QuadTreePos1.x, m_QuadTreePos1.y, m_QuadTreePos2.x, m_QuadTreePos2.y);
	m_ForceQuadTreeRegen = true;
}

void PhysicsManager::SetFrameBetweenQuadTreeRegenerations(int8 nbrFrame) {
	m_FrameBetweenQuadTreeRegenerations = nbrFrame;
}

bool PhysicsManager::IsQuadTreeActive() const
{
	return m_ActivateQuadTree;
}

#pragma endregion

PhysicsManager::~PhysicsManager() {
	delete mp_QuadTree;
}

