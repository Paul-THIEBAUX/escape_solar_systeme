#include "MathGC.h" 

float32 MathGC::RadToDeg(Radians rad)
{
    return rad * RAD_TO_DEG;
}

float32 MathGC::DegToRad(Degrees deg)
{
    return deg * DEG_TO_RAD;
}

Vector2f MathGC::AngleToVec(Radians rad)
{
    float x = std::cos(rad);
    float y = std::sin(rad);

    Vector2f vec({ x, y });

    return vec;
}

Radians MathGC::VecToAngle(Vector2f& vec)
{
    Radians rad = std::atan2(vec.y, vec.x);
    return rad;
}

std::ostream& operator<<(std::ostream& stream, const Matrix3x3& mat)
{
	stream << std::endl;
	for (uint32 j = 0; j < mat.HEIGHT; j++) {
		stream << " | ";
		for (uint32 i = 0; i < mat.WIDTH; i++) {
			stream << mat[i][j] << " | ";
		}
		stream << std::endl;
	}
	return stream;
}


Matrix::Matrix(int width, int height)
{
	InitMatrix(width, height);
}


void Matrix::InitSize()
{
	height = static_cast<int32>(matrix.size());


	if (height == 0)
	{
		width = 0;
	}

	else
	{
		width = static_cast<int32>(matrix[0].size());
	}


}

void Matrix::InitMatrix(int width, int height)
{
	for (int i = 0; i < height; i++)
	{
		std::vector<float32> vect;

		for (int j = 0; j < width; j++)
		{
			float32 number = 0;

			vect.push_back(number);
		}

		matrix.push_back(vect);
	}

	InitSize();
}

void Matrix::PrintMatrix()
{
	InitSize();

	if (matrix.size() == 0)
	{
		std::cout << "The matrix is empty !" << std::endl;
		return;
	}


	for (int i = 0; i < height; i++)
	{
		std::cout << "[";

		for (int j = 0; j < width - 1; j++)
		{
			std::cout << matrix[i][j] << ", ";
		}

		std::cout << matrix[i][width - 1] << "]" << std::endl;
	}
}

const float32* Matrix3x3::operator[](uint32 x) const { return m[x]; }


void Matrix3x3::CutMatrixRowLine(float32 output[2][2], uint32 column, uint32 row)
{
	for (uint32 x = 0; x < 2; ++x) {
		for (uint32 y = 0; y < 2; ++y) {
			output[x][y] = m[x + (x >= column ? 1 : 0)][y + (y >= row ? 1 : 0)];
		}
	}
}

constexpr Matrix3x3::Matrix3x3(float32(*func)(uint32, uint32))
{
	for (uint32 x = 0; x < WIDTH; ++x) {
		for (uint32 y = 0; y < HEIGHT; ++y) {
			m[x][y] = func(x, y);
		}
	}
}

constexpr Matrix3x3::Matrix3x3(): Matrix3x3(+[](uint32 x, uint32 y) { if (x == y) { return 1.0f; } else { return 0.0f; }}) {
}

float32* Matrix3x3::operator[](uint32 x) { return m[x]; }

Matrix3x3 Matrix3x3::operator* (const Matrix3x3& other) const
{
	Matrix3x3 r(+[](uint32, uint32) {return 0.0f; });

	for (uint32 i = 0; i < WIDTH; i++) {
		for (uint32 j = 0; j < HEIGHT; j++) {

			for (uint32 k = 0; k < WIDTH; k++) {
				r[i][j] += m[k][j] * other[i][k];
			}

		}
	}

	return r;
}

Matrix Matrix::operator*(Matrix& matrix2)
{
	InitSize();
	matrix2.InitSize();

	GCLE_ASSERT(width == matrix2.height);

	Matrix result;
	result.matrix.resize(height);

	for (auto& m : result.matrix)
		m.resize(matrix2.width);

	result.InitSize();

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < matrix2.width; j++)
		{
			for (int k = 0; k < width; k++)
			{
				result.matrix[i][j] += (matrix[i][k] * matrix2.matrix[k][j]);
			}
		}
	}

	return result;
}

Matrix3x3 Matrix3x3::operator+ (const Matrix3x3& other) const {
	Matrix3x3 r(+[](uint32, uint32) {return 0.0f; });

	for (uint32 i = 0; i < WIDTH; i++) {
		for (uint32 j = 0; j < HEIGHT; j++) {
			r[i][j] = m[i][j] + other[i][j];
		}
	}

	return r;
}

Matrix3x3 Matrix3x3::operator*(float32 other) const
{
	Matrix3x3 r(+[](uint32, uint32) {return 0.0f; });

	for (uint32 i = 0; i < WIDTH; i++) {
		for (uint32 j = 0; j < HEIGHT; j++) {
			r[i][j] = m[i][j] * other;
		}
	}

	return r;
}

float32 Matrix3x3::determinant() const
{

	return m[0][0] * m[1][1] * m[2][2]
		+ m[0][1] * m[1][2] * m[2][0]
		+ m[0][2] * m[1][0] * m[2][1]
		- m[0][2] * m[1][1] * m[2][0]
		- m[0][0] * m[1][2] * m[2][1]
		- m[0][1] * m[1][0] * m[2][2];
}

Matrix3x3 Matrix3x3::Inverse() const
{
	float32 det = determinant();
	if (abs(det) < FLT_EPSILON) {
		return *this;
	}

	Matrix3x3 r(+[](uint32, uint32) {return 0.0f; });
	float32 inv_det = 1 / det;

	r[0][0] = (m[1][1] * m[2][2] - m[2][1] * m[1][2]) * inv_det;		r[1][0] = (m[2][0] * m[1][2] - m[1][0] * m[2][2]) * inv_det;	r[2][0] = (m[1][0] * m[2][1] - m[2][0] * m[1][1]) * inv_det;
	r[0][1] = (m[2][1] * m[0][2] - m[0][1] * m[2][2]) * inv_det;		r[1][1] = (m[0][0] * m[2][2] - m[2][0] * m[0][2]) * inv_det;	r[2][1] = (m[2][0] * m[0][1] - m[0][0] * m[2][1]) * inv_det;
	r[0][2] = (m[0][1] * m[1][2] - m[1][1] * m[0][2]) * inv_det;		r[1][2] = (m[1][0] * m[0][2] - m[0][0] * m[1][2]) * inv_det;	r[2][2] = (m[0][0] * m[1][1] - m[1][0] * m[0][1]) * inv_det;

	return r;
}

Matrix3x3 Matrix3x3::Scale(Vector2f scale)
{
	Matrix3x3 m;
	m[0][0] = scale.x;
	m[1][1] = scale.y;
	return m;
}

Matrix3x3 Matrix3x3::Rotation(Radians angle)
{
	float c = cos(angle);
	float s = sin(angle);
	Matrix3x3 m;
	m[0][0] = c; m[1][0] = -s;
	m[0][1] = s; m[1][1] = c;
	return m;
}

Matrix3x3 Matrix3x3::Translation(Vector2f position)
{
	Matrix3x3 m;
	m[2][0] = position.x;
	m[2][1] = position.y;
	return m;
}

Vector2f Matrix3x3::TransformPoint(Vector2f point) const
{
	Vector2f result;
	result.x = m[0][0] * point.x + m[1][0] * point.y + m[2][0];
	result.y = m[0][1] * point.x + m[1][1] * point.y + m[2][1];
	return result;
}