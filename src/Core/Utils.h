#pragma once 
#include "PrimitiveTypes.h"
#include <string>
#include "Debugger.h"
#include "Vector2.hpp"

using Radians = float32;
using Vector2f = Vector2<float32>;


#ifdef _DEBUG
#define GCLE_NEW new( _NORMAL_BLOCK, __FILE__ , __LINE__ )
#else
#define GCLE_NEW new
#endif

#define GCLE_ASSERT( condition ) if (!(condition)) { GCLE_ERROR << "Assertion failed: " #condition << ENDL; std::abort(); }

Vector2f ComputeRatioPositionFromPivot(Vector2f pivot, Vector2f size, float32 ratioX, float32 ratioY);
 
Vector2f ComputePivotFromRatioPosition(Vector2f ratioPosition, Vector2f size, float32 ratioX, float32 ratioY);

int8 GenerateRandomNumber(int8 min, int8 max);

template<typename T>
class SmartPtr 
{
public: 
    SmartPtr(T* pP = nullptr) : mp_Ptr(pP)
    {
        if (pP)
            (*mp_RefCount)++;
        else
            mp_RefCount = nullptr;
    }
     
    SmartPtr(const SmartPtr& other) : mp_Ptr(other.mp_Ptr), mp_RefCount(other.mp_RefCount)
    {
        if (mp_RefCount)
            ++(*mp_RefCount);
    }
     
    SmartPtr& operator=(const SmartPtr& other)
    {
        if (this != &other)
        {
            Release();

            mp_Ptr = other.mp_Ptr;
            mp_RefCount = other.mp_RefCount;

            if (mp_RefCount)
                ++(*mp_RefCount);
        }

        return *this;
    }
     
    ~SmartPtr()
    {
        Release();
    }

    T& operator*() const
    {
        return *mp_Ptr;
    }

    T* operator->() const
    {
        return mp_Ptr;
    }

    uint64 UseCount() const
    {
        return mp_RefCount ? *mp_RefCount : 0;
    }

private:
    void Release()
    {
        if (mp_RefCount)
        {
            --(*mp_RefCount);

            if (*mp_RefCount == 0)
            {
                delete mp_Ptr;
                delete mp_RefCount;
            }
        }
    }

private:
    T* mp_Ptr;
    uint64* mp_RefCount;
};

struct AABB {
    float32 minX, minY, maxX, maxY;

    bool overlaps(const AABB& other);

    bool include(const AABB& other);
};

AABB GetRotatedAABB(Vector2<float32> center, Vector2<float32> halfSize, Radians rotation);

