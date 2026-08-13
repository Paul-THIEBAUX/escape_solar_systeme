#include "Shape.h" 
#include <SDL3/SDL.h>
#include "Engine/Entity.h"

const Color Color::White = { 255,255,255,255 };
const Color Color::Black = { 0,0,0,255 };
const Color Color::Red = { 255,0,0,255 };
const Color Color::Green = { 0,255,0,255 };
const Color Color::Blue = { 0,0,255,255 };
const Color Color::Yellow = { 255,255,0,255 };
const Color Color::Cyan = { 0,255,255,255 };
const Color Color::Magenta = { 255,0,255,255 };
const Color Color::Transparent = { 0,0,0,0 };

namespace gcle
{
#pragma region Shape

	SDL_FColor ToSDLColor(Color c)
	{
		return SDL_FColor{ c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, c.a / 255.0f };
	}

	Shape::~Shape()
	{
		for (SDL_Vertex* vertex : m_Verticies)
		{
			delete vertex;
		}
		m_Verticies.clear();

		for (SDL_FPoint* point : m_HollowPoints)
		{
			delete point;
		}
		m_HollowPoints.clear();

		m_DebugContour.clear();
		m_LocalPositions.clear();
		m_Indicies.clear();
		m_TrianglePoints.clear();
	}

	Shape::Shape(GameObject* owner) : mp_Owner(owner)
	{
	}

	Shape::Shape(const Shape& pShape) : mp_Texture(pShape.mp_Texture), m_Transform(pShape.m_Transform), m_Radius(pShape.m_Radius), m_Center(pShape.m_Center),
		m_Smoothness(pShape.m_Smoothness), m_Height(pShape.m_Height), m_Width(pShape.m_Width), m_Origin(pShape.m_Origin), m_TrianglePoints(pShape.m_TrianglePoints),
		m_Shape(pShape.m_Shape), m_LocalPositions(pShape.m_LocalPositions), m_Indicies(pShape.m_Indicies), mp_Owner(pShape.mp_Owner)
	{

		m_Verticies.resize(pShape.m_Verticies.size());
		for (uint64 i = 0; i < pShape.m_Verticies.size(); ++i)
		{
			if (pShape.m_Verticies[i])
				m_Verticies[i] = GCLE_NEW SDL_Vertex(*pShape.m_Verticies[i]);
			else
				m_Verticies[i] = nullptr;
		}

		m_HollowPoints.resize(pShape.m_HollowPoints.size());
		for (uint64 i = 0; i < pShape.m_HollowPoints.size(); ++i)
		{
			if (pShape.m_HollowPoints[i])
				m_HollowPoints[i] = GCLE_NEW SDL_FPoint(*pShape.m_HollowPoints[i]);
			else
				m_HollowPoints[i] = nullptr;
		}

		m_Transform.SetDirty();
	}

	void Shape::Move(Vector2f translation)
	{
		Vector2f pivot = m_Transform.GetPosition();
		m_Transform.SetPosition({ pivot.x + translation.x, pivot.y + translation.y });
		m_Center.x += translation.x;
		m_Center.y += translation.y;
	}

	void Shape::ScaleBy(Vector2f factor)
	{
		Vector2f current = m_Transform.GetScale();
		m_Transform.SetScale({ current.x * factor.x, current.y * factor.y });
	}

	void Shape::Rotate(Degrees delta)
	{
		SetRotation(m_Transform.GetDegAngle() + delta);
	}

	void Shape::SetColor(Color color)
	{
		SDL_FColor sdlColor = ToSDLColor(color);

		for (SDL_Vertex* vertex : m_Verticies)
		{
			if (vertex != nullptr)
				vertex->color = sdlColor;
		}
	}

	void Shape::UpdateRenderVertices()
	{
		if (!m_Transform.IsDirty())
			return;

		Matrix3x3 mat = m_Transform.GetMatrix();

		for (size_t i = 0; i < m_LocalPositions.size() && i < m_Verticies.size(); i++)
		{
			Vector2f world = mat.TransformPoint(m_LocalPositions[i]);
			m_Verticies[i]->position.x = world.x;
			m_Verticies[i]->position.y = world.y;

			m_HollowPoints[i]->x = world.x;
			m_HollowPoints[i]->y = world.y;
		}

		m_Transform.ClearDirty();
	}

#pragma endregion

#pragma region Rectangle

	Rectangle::Rectangle(float32 x, float32 y, float32 height, float32 width, Color color, GameObject* owner) : Shape(owner)
	{
		{
			m_Shape = Shapes::Rectangle;

			m_Verticies.resize(4);
			m_HollowPoints.resize(4);

			m_Indicies.resize(6);

			m_Height = height;
			m_Width = width;

			SDL_FColor sdl_color = ToSDLColor(color);

			m_Verticies[0] = GCLE_NEW SDL_Vertex{ {x, y}, sdl_color, {0.f, 0.f} };
			m_Verticies[1] = GCLE_NEW SDL_Vertex{ {x + m_Width, y}, sdl_color, {1.f, 0.f} };
			m_Verticies[2] = GCLE_NEW SDL_Vertex{ {x, y + m_Height}, sdl_color, {0.f, 1.f} };
			m_Verticies[3] = GCLE_NEW SDL_Vertex{ {x + m_Width, y + m_Height}, sdl_color, {1.f, 1.f} };

			for (int32 i = 0; i < static_cast<int32>(m_Verticies.size()); i++)
			{
				m_HollowPoints[i] = GCLE_NEW SDL_FPoint{ m_Verticies[i]->position.x, m_Verticies[i]->position.y };
			}

			m_Indicies = {
				0, 1, 2,
				2, 3, 1
			};

			m_Origin = { x, y };

			Vector2f pivot = { x + m_Width * 0.5f, y + m_Height * 0.5f };
			m_Transform.Initialize(pivot, 0);

			m_LocalPositions = {
				{ -m_Width * 0.5f, -m_Height * 0.5f },
				{  m_Width * 0.5f, -m_Height * 0.5f },
				{ -m_Width * 0.5f,  m_Height * 0.5f },
				{  m_Width * 0.5f,  m_Height * 0.5f }
			};

			m_Transform.SetDirty();
		}
	}

	Shape* Rectangle::Clone() const
	{
		return GCLE_NEW Rectangle(*this);
	}

#pragma endregion

#pragma region Circle

	Triangle::Triangle(float32 x1, float32 y1, float32 x2, float32 y2, float32 x3, float32 y3, Color color, GameObject* owner) : Shape(owner)
	{
		m_TrianglePoints.push_back({ x1, y1 });
		m_TrianglePoints.push_back({ x2, y2 });
		m_TrianglePoints.push_back({ x3, y3 });


		m_Shape = Shapes::Triangle;

		m_Verticies.resize(3);
		m_HollowPoints.resize(3);
		m_Indicies.resize(3);

		SDL_FColor sdl_color = ToSDLColor(color);

		m_Verticies[0] = GCLE_NEW SDL_Vertex{ {x1, y1}, sdl_color, {0.f, 0.f} };
		m_Verticies[1] = GCLE_NEW SDL_Vertex{ {x2, y2}, sdl_color, {1.f, 0.f} };
		m_Verticies[2] = GCLE_NEW SDL_Vertex{ {x3, y3}, sdl_color, {1.f, 1.f} };


		for (int i = 0; i < 3; ++i)
		{
			m_HollowPoints[i] = GCLE_NEW SDL_FPoint{};
		}

		m_Indicies = { 0, 1, 2 };

		m_Origin = { x1, y1 };

		float32 minX = std::min({ x1, x2, x3 });
		float32 maxX = std::max({ x1, x2, x3 });
		float32 minY = std::min({ y1, y2, y3 });
		float32 maxY = std::max({ y1, y2, y3 });
		Vector2f pivot = { (minX + maxX) * 0.5f, (minY + maxY) * 0.5f };

		m_Transform.Initialize(pivot, 0);
		m_Center = pivot;

		m_LocalPositions = {
			{ x1 - pivot.x, y1 - pivot.y },
			{ x2 - pivot.x, y2 - pivot.y },
			{ x3 - pivot.x, y3 - pivot.y }
		};

		m_Transform.SetDirty();
	}

	Shape* Triangle::Clone() const
	{
		return GCLE_NEW Triangle(*this);
	}

#pragma endregion

#pragma region Triangle

	Circle::Circle(float32 x, float32 y, float32 radius, int _smoothness, Color color, GameObject* owner) : Shape(owner)
	{
		if (_smoothness < 3) {
			return;
		}

		m_Smoothness = _smoothness;

		m_Center.x = x + radius;
		m_Center.y = y + radius;

		m_Radius = radius;

		m_Shape = Shapes::Circle;

		m_Verticies.resize(_smoothness + 1);
		m_HollowPoints.resize(_smoothness + 1);
		m_Indicies.resize(_smoothness * 3);

		m_Origin = { x, y };

		m_Transform.Initialize(m_Center, 0);

		Degrees degreesBetweenPoints = 360.0f / _smoothness;

		SDL_FColor sdl_color = ToSDLColor(color);

		m_Verticies[0] = new SDL_Vertex{ {x + radius, y + radius}, sdl_color, {0.5f, 0.5f} };
		m_LocalPositions.push_back({ 0.f, 0.f });

		for (int i = 0; i < _smoothness; i++)
		{
			Radians radian = MathGC::DegToRad(degreesBetweenPoints * i);

			float32 cx = sin(radian) * radius;
			float32 cy = cos(radian) * radius;

			float32 u = 0.5f + (cx / radius) * 0.5f;
			float32 v = 0.5f + (cy / radius) * 0.5f;

			m_Verticies[i + 1] = new SDL_Vertex{
				{x + radius + cx, y + radius + cy},
				sdl_color,
				{u, v}
			};

			m_LocalPositions.push_back({ cx, cy });

			m_Indicies[i * 3] = 0;
			m_Indicies[i * 3 + 1] = i + 1;
			m_Indicies[i * 3 + 2] = (i + 1) % _smoothness + 1;
		}


		for (int i = 0; i < _smoothness + 1; ++i)
		{
			m_HollowPoints[i] = GCLE_NEW SDL_FPoint{};
		}

		m_Transform.SetDirty();
	}

	Shape* Circle::Clone() const
	{
		return GCLE_NEW Circle(*this);
	}

#pragma endregion


#pragma region Sets

#pragma region Shape

	void Shape::SetTextureRect(int16 x, int16 y, int16 w, int16 h, int16 textW, int16 textH)
	{
		if (m_Shape == Shapes::Rectangle)
		{
			float32 u0 = x / static_cast<float32>(textW);
			float32 v0 = y / static_cast<float32>(textH);

			float32 u1 = (x + w) / static_cast<float32>(textW);
			float32 v1 = (y + h) / static_cast<float32>(textH);

			if (HasFlipFlag(m_FlipMode, TextureFlipMode::FLIP_HORIZONTAL))
				std::swap(u0, u1);

			if (HasFlipFlag(m_FlipMode, TextureFlipMode::FLIP_VERTICAL))
				std::swap(v0, v1);

			m_Verticies[0]->tex_coord = { u0, v0 };
			m_Verticies[1]->tex_coord = { u1, v0 };
			m_Verticies[3]->tex_coord = { u1, v1 };
			m_Verticies[2]->tex_coord = { u0, v1 };
		}
		else if (m_Shape == Shapes::Triangle)
		{
			float32 u0 = x / static_cast<float32>(textW);
			float32 v0 = y / static_cast<float32>(textH);

			float32 u1 = (x + w) / static_cast<float32>(textW);
			float32 v1 = (y + h) / static_cast<float32>(textH);

			if (HasFlipFlag(m_FlipMode, TextureFlipMode::FLIP_HORIZONTAL))
				std::swap(u0, u1);

			if (HasFlipFlag(m_FlipMode, TextureFlipMode::FLIP_VERTICAL))
				std::swap(v0, v1);

			m_Verticies[0]->tex_coord = { u0, v0 };
			m_Verticies[1]->tex_coord = { u1, v0 };
			m_Verticies[2]->tex_coord = { u1, v1 };
		}
		else if (m_Shape == Shapes::Circle)
		{
			float32 u0 = x / static_cast<float32>(textW);
			float32 v0 = y / static_cast<float32>(textH);
			float32 u1 = (x + w) / static_cast<float32>(textW);
			float32 v1 = (y + h) / static_cast<float32>(textH);

			if (HasFlipFlag(m_FlipMode, TextureFlipMode::FLIP_HORIZONTAL))
				std::swap(u0, u1);

			if (HasFlipFlag(m_FlipMode, TextureFlipMode::FLIP_VERTICAL))
				std::swap(v0, v1);
			m_Verticies[0]->tex_coord = { (u0 + u1) * 0.5f, (v0 + v1) * 0.5f };
			for (int i = 1; i < m_Smoothness + 1; i++)
			{
				m_Verticies[i]->tex_coord = { u0 + (u1 - u0) * 0.5f * (1.f + m_LocalPositions[i].x / m_Radius),
											  v0 + (v1 - v0) * 0.5f * (1.f + m_LocalPositions[i].y / m_Radius) };
			}
		}
	}

	void Shape::SetTexture(Window* window, SurfaceStruct* pSurface)
	{ 
		mp_Texture = new Sprite(window, pSurface->mp_surface);
	}

	void Shape::SetOrigin(Vector2f origin)
	{
		m_Origin = origin;
	}

	void Shape::SetScale(float32 scale)
	{
		m_Transform.SetScale({ scale, scale });
	}

	void Shape::SetScale(Vector2f scale)
	{
		m_Transform.SetScale(scale);
	}

	void Shape::SetRotation(Degrees angle)
	{
		m_Transform.SetDegAngle(angle);
	}

	void Shape::SetPosition(float32 x, float32 y, float32 ratioX, float32 ratioY)
	{
		Vector2f newPivot = { x, y };

		if (m_Shape == Shapes::Circle)
		{
			float32 size = GetRadius() * 2.f;
			newPivot.x = x - size * ratioX + size * 0.5f;
			newPivot.y = y - size * ratioY + size * 0.5f;
		}
		else if (m_Shape == Shapes::Rectangle)
		{
			float32 sizeX = GetWidth();
			float32 sizeY = GetHeight();
			newPivot.x = x - sizeX * ratioX + sizeX * 0.5f;
			newPivot.y = y - sizeY * ratioY + sizeY * 0.5f;
		}

		m_Transform.SetPosition(newPivot);
		m_Center = newPivot;
	}

	void Shape::SetFlip(TextureFlipMode mode)
	{
		m_FlipMode = mode;
	}

	TextureFlipMode Shape::GetFlip() const
	{
		return m_FlipMode;
	}

#pragma endregion

#pragma region Rectangle

	void Rectangle::SetHeight(float32 height)
	{
		m_Height = height;
		m_LocalPositions = {
			{ -m_Width * 0.5f, -m_Height * 0.5f },
			{  m_Width * 0.5f, -m_Height * 0.5f },
			{ -m_Width * 0.5f,  m_Height * 0.5f },
			{  m_Width * 0.5f,  m_Height * 0.5f }
		};
		m_Transform.SetDirty();
	}

	void Rectangle::SetWidth(float32 width)
	{
		m_Width = width;
		m_LocalPositions = {
			{ -m_Width * 0.5f, -m_Height * 0.5f },
			{  m_Width * 0.5f, -m_Height * 0.5f },
			{ -m_Width * 0.5f,  m_Height * 0.5f },
			{  m_Width * 0.5f,  m_Height * 0.5f }
		};
		m_Transform.SetDirty();
	}

#pragma endregion

#pragma region Circle

	void Circle::SetRadius(float32 radius)
	{
		m_Radius = radius;

		Degrees degreesBetweenPoints = 360.0f / m_Smoothness;

		for (int32 i = 0; i < m_Smoothness; i++)
		{
			Radians rad = MathGC::DegToRad(degreesBetweenPoints * i);
			m_LocalPositions[i + 1] = { sin(rad) * radius, cos(rad) * radius };
		}

		m_Transform.SetDirty();
	}

#pragma endregion

#pragma region Triangle

	void Triangle::SetTrianglePoints(std::vector<Vector2f> newTrianglePoints)
	{
		if (newTrianglePoints.size() < 3)
			return;

		m_TrianglePoints = newTrianglePoints;

		float32 minX = std::min({ newTrianglePoints[0].x, newTrianglePoints[1].x, newTrianglePoints[2].x });
		float32 maxX = std::max({ newTrianglePoints[0].x, newTrianglePoints[1].x, newTrianglePoints[2].x });
		float32 minY = std::min({ newTrianglePoints[0].y, newTrianglePoints[1].y, newTrianglePoints[2].y });
		float32 maxY = std::max({ newTrianglePoints[0].y, newTrianglePoints[1].y, newTrianglePoints[2].y });
		Vector2f pivot = { (minX + maxX) * 0.5f, (minY + maxY) * 0.5f };

		m_Transform.SetPosition(pivot);
		m_Center = pivot;

		m_LocalPositions.clear();
		for (int i = 0; i < 3; i++)
		{
			m_LocalPositions.push_back({ newTrianglePoints[i].x - pivot.x, newTrianglePoints[i].y - pivot.y });
		}

		m_Transform.SetDirty();
	}

#pragma endregion

#pragma endregion

#pragma region Gets

#pragma region Shape

	Shapes Shape::GetShape()
	{
		return m_Shape;
	};

	Vector2f  Shape::GetOrigin()
	{
		return m_Origin;
	}

	Vector2f Shape::GetPosition(float32 ratioX, float32 ratioY)
	{
		Vector2f pivot = m_Transform.GetPosition();

		if (m_Shape == Shapes::Circle)
		{
			float32 size = GetRadius() * 2.f;
			Vector2f topLeft = { pivot.x - size * 0.5f, pivot.y - size * 0.5f };
			return { topLeft.x + size * ratioX, topLeft.y + size * ratioY };
		}
		else if (m_Shape == Shapes::Rectangle)
		{
			float32 sizeX = GetWidth();
			float32 sizeY = GetHeight();
			Vector2f topLeft = { pivot.x - sizeX * 0.5f, pivot.y - sizeY * 0.5f };
			return { topLeft.x + sizeX * ratioX, topLeft.y + sizeY * ratioY };
		}

		return pivot;
	}


	Color Shape::GetColor() const
	{
		if (m_Verticies.empty() || m_Verticies[0] == nullptr)
			return { 255, 255, 255, 255 };

		const SDL_FColor& c = m_Verticies[0]->color;
		return
		{
			static_cast<uint8>(c.r * 255.f),
			static_cast<uint8>(c.g * 255.f),
			static_cast<uint8>(c.b * 255.f),
			static_cast<uint8>(c.a * 255.f)
		};
	}

	Vector2f Shape::GetScale()
	{
		return m_Transform.GetScale();
	}

	Sprite* Shape::GetTexture()
	{
		return (mp_Texture == nullptr ? nullptr : mp_Texture);
	};

	Degrees Shape::GetRotation()
	{
		return m_Transform.GetDegAngle();
	}

	Transform2D* Shape::GetTransform()
	{
		return &m_Transform;
	}

	GameObject* Shape::GetOwner()
	{
		return mp_Owner;
	};

	float32 Shape::GetWidth()
	{
		return 0.f;
	};

	float32 Shape::GetHeight()
	{
		return 0.f;
	};

	float32 Shape::GetRadius()
	{
		return 0.f;
	};

	int32 Shape::GetSmoothness()
	{
		return 0;
	};

	Vector2f Shape::GetCenter()
	{
		return { 0, 0 };
	};

	std::vector<int32>& Shape::GetIndicies()
	{
		return m_Indicies;
	};

	std::vector<Vector2f> Shape::GetTrianglePoints()
	{
		return m_TrianglePoints;
	};

	std::vector<SDL_FPoint*>& Shape::GetHollow()
	{
		UpdateRenderVertices();
		return m_HollowPoints;
	}

	std::vector<SDL_Vertex*>& Shape::GetVerticies()
	{
		UpdateRenderVertices();
		return m_Verticies;
	}

#pragma endregion

#pragma region Rectangle

	float32 Rectangle::GetHeight() {
		return m_Height * m_Transform.GetScale().y;
	}

	float32 Rectangle::GetWidth()
	{
		return m_Width * m_Transform.GetScale().x;
	}

	std::vector<SDL_FPoint*>& Rectangle::GetHollow()
	{
		UpdateRenderVertices();

		if (m_DebugContour.size() != 5)
			m_DebugContour.resize(5);

		m_DebugContour[0] = m_HollowPoints[0];
		m_DebugContour[1] = m_HollowPoints[1];
		m_DebugContour[2] = m_HollowPoints[3];
		m_DebugContour[3] = m_HollowPoints[2];
		m_DebugContour[4] = m_HollowPoints[0];

		return m_DebugContour;
	}

#pragma endregion

#pragma region Circle

	float32 Circle::GetRadius()
	{
		return m_Radius * m_Transform.GetScale().x;
	};

	int32 Circle::GetSmoothness()
	{
		return m_Smoothness;
	};

	Vector2f Circle::GetCenter()
	{
		return m_Center;
	};

	std::vector<SDL_FPoint*>& Circle::GetHollow()
	{
		UpdateRenderVertices();

		int32 count = m_Smoothness + 1;

		if (static_cast<int32>(m_DebugContour.size()) != count)
			m_DebugContour.resize(count);

		for (int32 i = 0; i < m_Smoothness; i++)
			m_DebugContour[i] = m_HollowPoints[i + 1];

		m_DebugContour[m_Smoothness] = m_HollowPoints[1];

		return m_DebugContour;
	}

#pragma endregion

#pragma region Triangle

	std::vector<SDL_FPoint*>& Triangle::GetHollow()
	{
		UpdateRenderVertices();

		if (m_DebugContour.size() != 4)
			m_DebugContour.resize(4);

		m_DebugContour[0] = m_HollowPoints[0];
		m_DebugContour[1] = m_HollowPoints[1];
		m_DebugContour[2] = m_HollowPoints[2];
		m_DebugContour[3] = m_HollowPoints[0];

		return m_DebugContour;
	}

#pragma endregion

#pragma endregion

}