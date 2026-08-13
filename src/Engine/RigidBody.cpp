#include <string>
#include <iostream>
#include <cstdlib>

#include "RigidBody.h"

RigidBody2D::RigidBody2D()
{
}

RigidBody2D::~RigidBody2D()
{
}

void RigidBody2D::Initialize(Transform2D* pTransform)
{
	mp_Transform = pTransform;
}

void RigidBody2D::Update(float32 dt)
{
	if (m_TempVelHasChanged)
	{
		m_Velocity = m_TempVelocity;
		m_TempVelHasChanged = false;
	}

	ApplyGravity(dt);

	if (m_UseFriction)
		ApplyFriction(dt);

	ClampVelocity();
	ApplyVelocity(dt);

	m_TempVelocity = m_Velocity;

	mp_Transform->UpdateChildPosition();
}

bool RigidBody2D::UseContinuousCollision() const
{
	return m_CollisionDetectionMode == CollisionDetectionMode::Continuous;
}

void RigidBody2D::AddForce(Vector2f direction, float32 strength, float32 dt)
{
	m_Velocity += direction.Normalized() * (strength / m_Mass) * dt;
}

void RigidBody2D::SetForce(Vector2f direction, float32 strenght)
{
	Vector2f dir = direction.Normalized();
	m_Velocity = dir * (strenght / m_Mass);
}

void RigidBody2D::AddImpulse(Vector2f direction, float32 strength)
{
	Vector2f dir = direction.Normalized();
	m_Velocity += dir * strength / m_Mass;
}

void RigidBody2D::SetMass(float32 mass)
{
	m_Mass = std::max(0.0001f, mass);
}

void RigidBody2D::ClampVelocity()
{
	float32 speedSq = m_Velocity.x * m_Velocity.x + m_Velocity.y * m_Velocity.y;
	float32 maxSpeedSq = m_MaxSpeed * m_MaxSpeed;

	if (speedSq > maxSpeedSq)
	{
		float32 scale = m_MaxSpeed / std::sqrt(speedSq);
		m_Velocity.x *= scale;
		m_Velocity.y *= scale;
	}
}

void RigidBody2D::Stop()
{
	m_Velocity = { 0,0 };

}

void RigidBody2D::SetMaxSpeed(float32 speed)
{
	m_MaxSpeed = speed;
}

void RigidBody2D::SetGravity(float32 strenght, bool isActive)
{
	m_Gravity = strenght; m_UseGravity = isActive;
}

void RigidBody2D::SetGravity(bool isActive)
{
	m_UseGravity = isActive;
}

void RigidBody2D::SetGravity(float32 strenght)
{
	m_Gravity = strenght;
}

float32 RigidBody2D::GetSpeed() const
{
	return std::sqrt(m_Velocity.x * m_Velocity.x + m_Velocity.y * m_Velocity.y);
}

Vector2f RigidBody2D::GetVelocity() const
{
	return m_Velocity;
}

void RigidBody2D::SetVelocity(Vector2f velocity)
{
	m_TempVelocity = velocity; m_TempVelHasChanged = true;
}

void RigidBody2D::ZeroVelocityX(bool right)
{
	if (right && m_Velocity.x < 0)
		m_Velocity.x = 0;
	if (!right && m_Velocity.x > 0)
		m_Velocity.x = 0;
}

void RigidBody2D::ZeroVelocityY(bool down)
{
	if (down && m_Velocity.y < 0)
		m_Velocity.y = 0;
	if (!down && m_Velocity.y > 0)
		m_Velocity.y = 0;
}

void RigidBody2D::RemoveVelocityAlongNormal(const Vector2f& normal)
{
	float32 lengthSq = normal.x * normal.x + normal.y * normal.y;

	if (lengthSq <= 0.0001f)
		return;

	Vector2f n = normal / std::sqrt(lengthSq);

	float32 dot = m_Velocity.Dot(n);

	if (dot < 0.0f)
	{
		m_Velocity -= n * dot;
	}
}

Vector2f RigidBody2D::CalculateNextPosition(float32 dt)
{
	Vector2f pos = mp_Transform->GetPosition();
	Vector2f nextPos = pos + m_Velocity * dt;
	return nextPos;
}

void RigidBody2D::ApplyVelocity(float32 dt)
{
	Vector2f pos = mp_Transform->GetPosition();
	pos += m_Velocity * dt;
	mp_Transform->SetPosition(pos);
}

void RigidBody2D::ApplyFriction(float32 dt)
{
	m_Velocity.x *= std::pow(1.0f - m_Friction.x, dt);
	m_Velocity.y *= std::pow(1.0f - m_Friction.y, dt);

	if (std::abs(m_Velocity.x) < 0.01f)
		m_Velocity.x = 0.f;

	if (std::abs(m_Velocity.y) < 0.01f)
		m_Velocity.y = 0.f;
}

void RigidBody2D::ApplyGravity(float32 dt)
{
	if (!m_UseGravity) return;
	m_Velocity.y += m_Gravity * dt;
}

void RigidBody2D::Brake(float32 dt)
{
	float32 speed = GetSpeed();

	if (speed <= 0.01f)
	{
		m_Velocity = { 0.f, 0.f };
		return;
	}

	float32 newSpeed = std::max(0.f, speed - m_BrakeDeceleration * dt);
	float32 ratio = newSpeed / speed;

	m_Velocity.x *= ratio;
	m_Velocity.y *= ratio;
}

void RigidBody2D::SetBrakeDeceleration(float32 deceleration)
{
	m_BrakeDeceleration = deceleration; 
}

void RigidBody2D::SetCollisionDetectionMode(CollisionDetectionMode mode)
{
	m_CollisionDetectionMode = mode;
}

void RigidBody2D::SetCollisionOnContinuous()
{
	m_CollisionDetectionMode = CollisionDetectionMode::Continuous;
}

bool RigidBody2D::IsActive() const { 
	return IsRigidBody; 
}

void RigidBody2D::SetActive(bool Active) { 
	IsRigidBody = Active; 
}

void RigidBody2D::ZeroVelocityX() { 
	m_Velocity.x = 0; 
}

void RigidBody2D::ZeroVelocityY() { 
	m_Velocity.y = 0; 
}

void RigidBody2D::SetFriction(Vector2f velocityFrictionFactor)
{
	m_Friction = velocityFrictionFactor;
}

void RigidBody2D::SetFrictionOnXAxis(float32 strenght) 
{
	m_Friction.x = strenght; 
}

void RigidBody2D::SetFrictionOnYAxis(float32 strenght) 
{ 
	m_Friction.y = strenght; 
}

void RigidBody2D::ActivateFriction(bool isActive)
{
	m_UseFriction = isActive;
}
