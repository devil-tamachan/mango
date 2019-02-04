/*
    MANGO Multimedia Development Platform
    Copyright (C) 2012-2018 Twilight Finland 3D Oy Ltd. All rights reserved.
*/
#pragma once

#include "vector.hpp"

namespace mango
{

    template <>
    struct Vector<s8, 64>
    {
        using VectorType = simd::int8x64;
        using ScalarType = s8;
        enum { VectorSize = 64 };

        union
        {
            VectorType m;
            DeAggregate<ScalarType> component[VectorSize];
        };

        ScalarType& operator [] (size_t index)
        {
            assert(index < VectorSize);
            return component[index].data;
        }

        ScalarType operator [] (size_t index) const
        {
            assert(index < VectorSize);
            return component[index].data;
        }

        const ScalarType* data() const
        {
            return reinterpret_cast<const ScalarType *>(component);
        }

        explicit Vector() {}
        ~Vector() {}

        Vector(s8 s)
            : m(simd::int8x64_set1(s))
        {
        }

        Vector(simd::int8x64 v)
            : m(v)
        {
        }

        Vector& operator = (simd::int8x64 v)
        {
            m = v;
            return *this;
        }

        Vector& operator = (s8 s)
        {
            m = simd::int8x64_set1(s);
            return *this;
        }

        operator simd::int8x64 () const
        {
            return m;
        }

#ifdef int512_is_hardware_vector
        operator simd::int8x64::vector () const
        {
            return m.data;
        }
#endif
    };

    static inline const Vector<s8, 64> operator + (Vector<s8, 64> v)
    {
        return v;
    }

    static inline Vector<s8, 64> operator - (Vector<s8, 64> v)
    {
        return simd::sub(simd::int8x64_zero(), v);
    }

    static inline Vector<s8, 64>& operator += (Vector<s8, 64>& a, Vector<s8, 64> b)
    {
        a = simd::add(a, b);
        return a;
    }

    static inline Vector<s8, 64>& operator -= (Vector<s8, 64>& a, Vector<s8, 64> b)
    {
        a = simd::sub(a, b);
        return a;
    }

    static inline Vector<s8, 64> operator + (Vector<s8, 64> a, Vector<s8, 64> b)
    {
        return simd::add(a, b);
    }

    static inline Vector<s8, 64> operator - (Vector<s8, 64> a, Vector<s8, 64> b)
    {
        return simd::sub(a, b);
    }

    static inline Vector<s8, 64> nand(Vector<s8, 64> a, Vector<s8, 64> b)
    {
        return simd::bitwise_nand(a, b);
    }

    static inline Vector<s8, 64> operator & (Vector<s8, 64> a, Vector<s8, 64> b)
    {
        return simd::bitwise_and(a, b);
    }

    static inline Vector<s8, 64> operator | (Vector<s8, 64> a, Vector<s8, 64> b)
    {
        return simd::bitwise_or(a, b);
    }

    static inline Vector<s8, 64> operator ^ (Vector<s8, 64> a, Vector<s8, 64> b)
    {
        return simd::bitwise_xor(a, b);
    }

    static inline Vector<s8, 64> adds(Vector<s8, 64> a, Vector<s8, 64> b)
    {
        return simd::adds(a, b);
    }

    static inline Vector<s8, 64> subs(Vector<s8, 64> a, Vector<s8, 64> b)
    {
        return simd::subs(a, b);
    }

    static inline Vector<s8, 64> min(Vector<s8, 64> a, Vector<s8, 64> b)
    {
        return simd::min(a, b);
    }

    static inline Vector<s8, 64> max(Vector<s8, 64> a, Vector<s8, 64> b)
    {
        return simd::max(a, b);
    }

    static inline mask8x64 operator > (Vector<s8, 64> a, Vector<s8, 64> b)
    {
        return simd::compare_gt(a, b);
    }

    static inline mask8x64 operator < (Vector<s8, 64> a, Vector<s8, 64> b)
    {
        return simd::compare_gt(b, a);
    }

    static inline mask8x64 operator == (Vector<s8, 64> a, Vector<s8, 64> b)
    {
        return simd::compare_eq(a, b);
    }

    static inline Vector<s8, 64> select(mask8x64 mask, Vector<s8, 64> a, Vector<s8, 64> b)
    {
        return simd::select(mask, a, b);
    }

} // namespace mango
