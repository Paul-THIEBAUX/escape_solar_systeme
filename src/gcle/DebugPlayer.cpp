#include "DebugPlayer.h"
#include "GameManager.h"
#include "Tag.h"

void DebugPlayer::OnInitialize() {

    RigidBody2D* rb = GetRigidBody();
    rb->SetFriction({ 0.5, 0});
    rb->ActivateFriction(true);
}

void DebugPlayer::OnUpdate() {
    InputManager& im = InputManager::GetInstance();
    RigidBody2D* rb = GetRigidBody();
    float32 dt = static_cast<float32>(GameManager::GetInstance().GetTime()->GetDeltaTime());


    if (im.IsHeld('Q')) {
        rb->AddForce({ -1, 0 }, 100, dt);
    }
    if (im.IsHeld('D')) {
        rb->AddForce({ 1, 0 }, 100, dt);
    }
    if (im.IsHeld('S')) {
        rb->AddForce({ 0, 1 }, 100, dt);
    }
    if (im.IsHeld('Z')) {
        rb->AddForce({ 0, -1 }, 100, dt);
    }

    if (IsGrounded) 
    { 
        rb->Brake(dt);

        if (im.IsHeld(' ')) {
            rb->AddImpulse({ 0, -1 }, 100000);
        }
    }
}

void DebugPlayer::OnCollisionEnter(Entity* collidedWith)
{
    if (collidedWith->IsTag(Tag::Ground)) {
        IsGrounded = true;
    }
}

void DebugPlayer::OnCollision(Entity* collidedWith)
{
    std::cout << "collision" << std::endl;
}

void DebugPlayer::OnCollisionExit(Entity* collidedWith)
{
    if (collidedWith->IsTag(Tag::Ground)) {
        IsGrounded = false;
    }
}