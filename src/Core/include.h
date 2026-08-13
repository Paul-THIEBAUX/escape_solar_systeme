#pragma once
#include "PrimitiveTypes.h"
#include "Vector2.hpp"
#include "Debugger.h"
#include "Profiler.h"
#include "Utils.h"
#include "Timer.h"

using Vector2u = Vector2<int32>;
using Vector2f = Vector2<float32>;
using Vector2s = Vector2<int16>;

constexpr float32 RENDER_TARGET_WIDTH = 1920.f;
constexpr float32 RENDER_TARGET_HEIGHT = 1080.f;

constexpr float32 PI			= 3.1415927f;
constexpr float32 RAD_TO_DEG	= float32(180.0f / PI);
constexpr float32 DEG_TO_RAD	= float32(PI / 180.0f);
constexpr float32 GRAVITY		= 9.80665f;

class Entity;
using EntityPtr = SmartPtr<Entity>;

using Radians = float32;
using Degrees = float32;