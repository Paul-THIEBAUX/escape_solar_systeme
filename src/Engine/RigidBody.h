#pragma once
#include <string>

#include "Include.h"
#include "Transform.h"

#undef max

enum class CollisionDetectionMode
{
	Discrete,
	Continuous
};

class RigidBody2D
{
public:
	RigidBody2D();
	~RigidBody2D();

	void Initialize(Transform2D* mp_Transform);

	void Update(float32 dt);

	void AddForce(Vector2f direction, float32 strength, float32 dt);

	void SetForce(Vector2f direction, float32 speed);
	void AddImpulse(Vector2f direction, float32 strength);
	void SetMass(float32 mass);

	bool IsActive() const;
	void SetActive(bool Active);

	void ClampVelocity();
	void Stop();
	void SetMaxSpeed(float32 speed);

	void SetGravity(float32 strenght, bool isActive);
	void SetGravity(bool isActive);
	void SetGravity(float32 strenght);

	float32 GetSpeed() const;
	Vector2f GetVelocity() const;

	void SetVelocity(Vector2f velocity);

	void ZeroVelocityX(bool right);
	void ZeroVelocityY(bool down);

	void ZeroVelocityX();
	void ZeroVelocityY();

	void RemoveVelocityAlongNormal(const Vector2f& normal);

	void Brake(float32 dt);
	void SetBrakeDeceleration(float32 deceleration);

	void SetFriction(Vector2f velocityFrictionFactor);
	void SetFrictionOnXAxis(float32 strenght);
	void SetFrictionOnYAxis(float32 strenght);

	void ActivateFriction(bool isActive);

	Vector2f CalculateNextPosition(float32 dt);
	bool UseContinuousCollision() const;

private:
	void ApplyVelocity(float32 dt);
	void ApplyFriction(float32 dt);
	void ApplyGravity(float32 dt);

private:
	CollisionDetectionMode m_CollisionDetectionMode = CollisionDetectionMode::Discrete;

public:
	void SetCollisionDetectionMode(CollisionDetectionMode mode);
	void SetCollisionOnContinuous();

private:
	Vector2f m_Position;
	Vector2f m_Direction;
	Vector2f m_Velocity;
	Vector2f m_TempVelocity;
	float32 m_Mass = 1.0f;
	Vector2f m_Friction = {0.1f, 0.1f};

	bool m_UseFriction = false;

	float32 m_MaxSpeed = 500.0f;
	float32 m_BrakeDeceleration = 800.f;

	float32 m_Gravity = GRAVITY;
	bool m_UseGravity = false;

	float32 m_Restitution = 0.5f;

	bool IsRigidBody = false;

	bool m_TempVelHasChanged = false;

	Transform2D* mp_Transform = nullptr;

	float64 m_Dt = 0.0f;

};