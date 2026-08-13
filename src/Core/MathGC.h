#pragma once

#include "include.h"
#include <iostream>

using Radians = float32;
using Degrees = float32;

namespace MathGC
{
	float32 RadToDeg(Radians rad);
	float32 DegToRad(Degrees deg);
	Vector2f AngleToVec(Radians rad);
	Radians VecToAngle(Vector2f& vec);

	template<typename T>
	T Lerp(T minValue, T maxValue, float t);
};

template<typename T>
T MathGC::Lerp(T minValue, T maxValue, float t)
{
	return (t - 1) * minValue + t * maxValue;
}


class Matrix3x3
{
	float32 m[3][3];

	void CutMatrixRowLine(float32 output[2][2], uint32 column, uint32 row);

public:
	const unsigned WIDTH = 3;
	const unsigned HEIGHT = 3;

	constexpr Matrix3x3(float32(*func)(uint32, uint32));

	constexpr Matrix3x3();

	float32* operator[](uint32 x);
	const float32* operator[](uint32 x) const;
	
	Matrix3x3 operator* (const Matrix3x3& other) const;

	Matrix3x3 operator+ (const Matrix3x3& other) const;

	Matrix3x3 operator* (float32 other) const;

	float32 determinant() const;

	Matrix3x3 Inverse() const;

	static Matrix3x3 Scale(Vector2f scale);

	static Matrix3x3 Rotation(Radians angle);

	static Matrix3x3 Translation(Vector2f position);
	 
	Vector2f TransformPoint(Vector2f point) const;

};

std::ostream& operator<< (std::ostream& stream, const Matrix3x3& mat);

class Matrix
{
	public:
		std::vector<std::vector<float>> matrix;

		int16 width = 0;
		int16 height = 0;

		Matrix() = default;

		Matrix(int width, int height);

		void InitSize();
		void InitMatrix(int width, int height);

		void PrintMatrix();

		Matrix operator*(Matrix& matrix2);


};
