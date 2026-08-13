#pragma once
#include "Core/include.h"
#include <type_traits>

class Mask
{
public:
	Mask() = default;
	~Mask() = default;

	template <typename T>
	static void AddLayer(T& BaseMask, T Layer);

	template <typename T>
	static void RemoveLayer(T& BaseMask, T Layer);

	template <typename T>
	static bool IsOnLayer(T BaseMask, T layer);
	
	template <typename T>
	static bool AreOnASameLayer(T BaseMask1, T BaseMask2);

private:

};

#include "Mask.inl"

