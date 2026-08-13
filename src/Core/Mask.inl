#pragma once

template <typename T>
void Mask::AddLayer(T& baseMask, T layer)
{
    static_assert(std::is_integral_v<T>, "Mask type must be an integer type");

    baseMask |= (T{ 1 } << layer);
}

template <typename T>
void Mask::RemoveLayer(T& baseMask, T layer)
{
    static_assert(std::is_integral_v<T>, "Mask type must be an integer type");

    baseMask &= ~(T{ 1 } << layer);
}

template <typename T>
bool Mask::IsOnLayer(T baseMask, T layer)
{
    static_assert(std::is_integral_v<T>, "Mask type must be an integer type");

    return (baseMask & (T{ 1 } << layer)) != T{ 0 };
}

template <typename T>
bool Mask::AreOnASameLayer(T baseMask1, T baseMask2)
{
    static_assert(std::is_integral_v<T>, "Mask type must be an integer type");

    return (baseMask1 & baseMask2) != T{ 0 };
}