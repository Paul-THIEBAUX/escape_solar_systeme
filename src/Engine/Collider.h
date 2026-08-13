#pragma once
#include "include.h"
#include "Render/Shape.h"

using namespace gcle;

class Entity;   // <-- AJOUT : forward declaration

struct CollisionDirection
{
	bool isCollidingOnTop = false;
	bool isCollidingOnBottom = false;
	bool isCollidingOnRight = false;
	bool isCollidingOnLeft = false;
};


class Collider
{
public:
	Collider() = default;
	~Collider();

	void Initialize(Shape* pShape, Vector2f position, float32 rotation, Entity* pOwner);

	void CollidingOn(Vector2f direction);
	void CollidingOnX(float32 direction);
	void CollidingOnY(float32 direction);

	void StoppedColliding();

	bool IsActive();
	void SetActive(bool activate) ;

	Shape* GetShape()			;
	const Shape* GetShape() const;

	const CollisionDirection& GetCollisionDirection() const;

	Entity* GetOwner() const	;
	void SetOwner(Entity* pOwner);

	bool IsTrigger() const;
	void SetTrigger(bool isTrigger);

private:
	Shape* mp_Shape = nullptr;

	Entity* mp_Owner = nullptr;

	bool m_IsActive = false;
	bool m_IsTrigger = false;

	CollisionDirection m_CollisionDirection;

private:
	bool m_InQuerySeen = false;
	AABB m_Aabb{};

public:
	bool GetInQuerySeen();
	void SetInQuerySeen(bool seen);

	AABB GetAABB();
	void SetAABB(AABB aabb);
};