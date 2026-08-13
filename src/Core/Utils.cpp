#include "Utils.h"
#include <string>



Vector2f ComputeRatioPositionFromPivot(Vector2f pivot, Vector2f size, float32 ratioX, float32 ratioY)
{
	Vector2f topLeft = { pivot.x - size.x * 0.5f, pivot.y - size.y * 0.5f };
	return { topLeft.x + size.x * ratioX, topLeft.y + size.y * ratioY };
}

Vector2f ComputePivotFromRatioPosition(Vector2f ratioPosition, Vector2f size, float32 ratioX, float32 ratioY)
{
	return
	{
		ratioPosition.x - size.x * ratioX + size.x * 0.5f,
		ratioPosition.y - size.y * ratioY + size.y * 0.5f
	};
}

int8 GenerateRandomNumber(int8 min, int8 max){
	if (max == 0) {
		max += 1;
	}
	return (rand() % max) + min;
}
 
std::string MoveElementInVector(std::string string, int32 startPos, int32 endPos)
{
	string.insert(string.begin() + endPos, string[startPos]);

	string;

	if (startPos < endPos)
	{
		string.erase(startPos, startPos);
	}

	else
	{
		string.erase(startPos + 1, startPos + 1);
	}

	return string;
}

std::string NumberToString(float64 value)
{
	std::string string = std::to_string(value);

	return string;
}

std::string RemoveZeroes(std::string string)
{
	bool isInteger = true;

	for (int32 i = 0; (i < string.size() - 1) && (isInteger == true); i++)
	{
		if (string[i] == '.')
		{
			isInteger = false;
		}
	}

	if (isInteger == false)
	{
		for (int32 j = static_cast<int32>(string.size()) - 2; string[j] == '0'; j--)
		{
			string.erase(j);
		}

		if (string[string.size() - 1] == '.')
		{
			string.erase(string.size() - 1);
		}
	}

	string;

	return string;
}

AABB GetRotatedAABB(Vector2<float32> center, Vector2<float32> halfSize, Radians rotation) {

	float32 c = std::cos(rotation);
	float32 s = std::sin(rotation);

	Vector2<float32> localCorners[4]{
		{-halfSize.x, -halfSize.y},
		{halfSize.x, -halfSize.y},
		{halfSize.x, halfSize.y},
		{-halfSize.x, halfSize.y}
	};

	float32 minX = FLT_MAX, maxX = -FLT_MAX;
	float32 minY = FLT_MAX, maxY = -FLT_MAX;

	for (auto& corner : localCorners) {
		float32 worldX = center.x + (corner.x * c - corner.y * s);
		float32 worldY = center.y + (corner.x * s + corner.y * c);

		minX = std::min(minX, worldX);
		maxX = std::max(maxX, worldX);
		minY = std::min(minY, worldY);
		maxY = std::max(maxY, worldY);
	}

	return { minX, minY, maxX, maxY };
}

bool AABB::overlaps(const AABB& other)
{
	return minX < other.maxX && minY < other.maxY && maxX > other.minX && maxY > other.minY;
}

bool AABB::include(const AABB& other)
{
	return other.minX >= minX && other.maxX <= maxX && other.minY >= minY && other.maxY <= maxY;
}
