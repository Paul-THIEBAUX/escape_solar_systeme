#pragma once
#include <vector>
#include <iostream> 
#include "MathGC.h"
#include "Engine/Transform.h"
#include "Texture.h"
#include "Engine/RessourceManager.h"
#include "Sprite.h"

class GameObject;
class UI;
class Entity;
class Window;
class Surface;
struct SDL_Vertex;
struct SDL_FPoint;

struct Color
{
	uint8 r = 0;
	uint8 g = 0;
	uint8 b = 0;
	uint8 a = 0;

	constexpr Color() = default;
	constexpr Color(uint8 _r, uint8 _g, uint8 _b, uint8 _a) : r(_r), g(_g), b(_b), a(_a)
	{
	}

	static const Color White;
	static const Color Black;
	static const Color Red;
	static const Color Green;
	static const Color Blue;
	static const Color Yellow;
	static const Color Cyan;
	static const Color Magenta;
	static const Color Transparent;

	

};

enum class TextureFlipMode : int32
{
	FLIP_NONE,                                                                  /*	Do not flip			*/
	FLIP_HORIZONTAL,                                                            /*	Flip horizontally	*/
	FLIP_VERTICAL,                                                              /*	Flip vertically		*/
	FLIP_HORIZONTAL_AND_VERTICAL = (FLIP_HORIZONTAL | FLIP_VERTICAL)
};

inline TextureFlipMode operator|(TextureFlipMode a, TextureFlipMode b)
{
	return static_cast<TextureFlipMode>(static_cast<int32>(a) | static_cast<int32>(b));
}

inline bool HasFlipFlag(TextureFlipMode value, TextureFlipMode flag)
{
	return (static_cast<int32>(value) & static_cast<int32>(flag)) != 0;
}

namespace gcle
{
	enum class Shapes {
		Triangle,
		Rectangle,
		Circle,
		Count
	};

	class Shape {

		Sprite* mp_Texture = nullptr;

	protected:
		Transform2D m_Transform;
		GameObject* mp_Owner;

		//circle
		float32 m_Radius = 0.0f;
		Vector2f m_Center = { 0.0f, 0.0f };
		int32 m_Smoothness = 16;

		//rectangle
		float32 m_Height = 0.0f;
		float32 m_Width = 0.0f;
		Vector2f m_Origin = { 0.0f, 0.0f };

		//triangle
		std::vector<Vector2f> m_TrianglePoints;

		Shapes m_Shape = Shapes::Triangle;
		 
		std::vector<Vector2f> m_LocalPositions;

		std::vector<SDL_Vertex*> m_Verticies;
		std::vector<int32> m_Indicies;

		std::vector<SDL_FPoint*> m_HollowPoints;
		std::vector<SDL_FPoint*> m_DebugContour;
		 
		void UpdateRenderVertices();

		//Constructors 

		Shape(GameObject* owner);
		Shape(const Shape& pShape);
		Shape& operator=(const Shape&) = delete;

	public:
		virtual ~Shape();

	public:

		//Getters 
		Shapes GetShape();
		Vector2f GetOrigin();
		Sprite* GetTexture();
		std::vector<int32>& GetIndicies(); 
		std::vector<SDL_Vertex*>& GetVerticies();
		Vector2f GetPosition(float32 ratioX = 0.5f, float32 ratioY = 0.5f);

		Vector2f GetScale();
		Degrees GetRotation();

		Transform2D* GetTransform();
		Color GetColor() const;
 
		public:

		void SetTexture(Window* window, SurfaceStruct* pSurface);
		void SetOrigin(Vector2f origin);
		void SetPosition(float32 x, float32 y, float32 ratioX = 0.5f, float32 ratioY = 0.5f);
		 
		void SetScale(Vector2f scale);
		void SetScale(float32 scale);
		void ScaleBy(Vector2f factor);
		 
		void SetRotation(Degrees angle); 
		void Rotate(Degrees delta);
		void SetColor(Color color);


	public:
		virtual float32 GetWidth();
		virtual float32 GetHeight();
		virtual float32 GetRadius();
		virtual int32 GetSmoothness();
		virtual GameObject* GetOwner();
		virtual Vector2f GetCenter();
		virtual std::vector<SDL_FPoint*>& GetHollow();
		virtual std::vector<Vector2f> GetTrianglePoints();


	public:
		virtual void SetRadius(float32 radius) {};
		virtual void SetHeight(float32 height) {};
		virtual void SetWidth(float32 width) {};
		virtual void SetCenter(Vector2f center) {};
		virtual void SetTrianglePoints(std::vector<Vector2f> newTrianglePoints) {};

	public:
		void Move(Vector2f translation);

		virtual void SetTextureRect(int16 x, int16 y, int16 w, int16 h, int16 textW, int16 textH);

		virtual Shape* Clone() const = 0;

		protected:
			TextureFlipMode m_FlipMode = TextureFlipMode::FLIP_NONE;

	public:
		void SetFlip(TextureFlipMode mode);
		TextureFlipMode GetFlip() const;
	};


	class Rectangle : public Shape {

	public:
		Rectangle(float32 x, float32 y, float32 height, float32 width, Color color, GameObject* owner);
	
		Shape* Clone() const override;

		//Getters
		 
		float32 GetHeight() override;
		float32 GetWidth() override;

		std::vector<SDL_FPoint*>& GetHollow() override;


		//Setters

		void SetHeight(float32 height) override;
		void SetWidth(float32 width) override;

	};

	class Triangle : public Shape {
	public:

		//Contructors

		Triangle(float32 x1, float32 y1, float32 x2, float32 y2, float32 x3, float32 y3, Color color, GameObject* owner);

		Shape* Clone() const override;

		void SetTrianglePoints(std::vector<Vector2f> newTrianglePoints) override;

		std::vector<SDL_FPoint*>& GetHollow() override;
	};

	class Circle : public Shape {
	public:

		//Contructors

		Circle(float32 x, float32 y, float32 radius, int _smoothness, Color color, GameObject* owner);

		Shape* Clone() const override;

		float32 GetRadius() override;
		int32 GetSmoothness() override;
		Vector2f GetCenter() override;

		std::vector<SDL_FPoint*>& GetHollow() override;

		void SetRadius(float32 radius) override;

	};
}