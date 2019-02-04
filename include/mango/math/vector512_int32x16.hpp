/*
    MANGO Multimedia Development Platform
    Copyright (C) 2012-2018 Twilight Finland 3D Oy Ltd. All rights reserved.
*/
#pragma once

#include "vector.hpp"

namespace mango
{

    template <>
    struct Vector<s32, 16>
    {
        using VectorType = simd::int32x16;
        using ScalarType = s32;
        enum { VectorSize = 16 };

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

        Vector(s32 s)
            : m(simd::int32x16_set1(s))
        {
        }

        Vector(s32 v0, s32 v1, s32 v2, s32 v3, s32 v4, s32 v5, s32 v6, s32 v7,
            s32 v8, s32 v9, s32 v10, s32 v11, s32 v12, s32 v13, s32 v14, s32 v15)
            : m(simd::int32x16_set16(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15))
        {
        }

        Vector(simd::int32x16 v)
            : m(v)
        {
        }

        Vector& operator = (simd::int32x16 v)
        {
            m = v;
            return *this;
        }

        Vector& operator = (s32 s)
        {
            m = simd::int32x16_set1(s);
            return *this;
        }

        operator simd::int32x16 () const
        {
            return m;
        }

#ifdef int512_is_hardware_vector
        operator simd::int32x16::vector () const
        {
            return m.data;
        }
#endif
    };

    static inline const Vector<s32, 16> operator + (Vector<s32, 16> v)
    {
        return v;
    }

    static inline Vector<s32, 16> operator - (Vector<s32, 16> v)
    {
        return simd::sub(simd::int32x16_zero(), v);
    }

    static inline Vector<s32, 16>& operator += (Vector<s32, 16>& a, Vector<s32, 16> b)
    {
        a = simd::add(a, b);
        return a;
    }

    static inline Vector<s32, 16>& operator -= (Vector<s32, 16>& a, Vector<s32, 16> b)
    {
        a = simd::sub(a, b);
        return a;
    }

    static inline Vector<s32, 16> operator + (Vector<s32, 16> a, Vector<s32, 16> b)
    {
        return simd::add(a, b);
    }

    static inline Vector<s32, 16> operator - (Vector<s32, 16> a, Vector<s32, 16> b)
    {
        return simd::sub(a, b);
    }

    static inline Vector<s32, 16> nand(Vector<s32, 16> a, Vector<s32, 16> b)
    {
        return simd::bitwise_nand(a, b);
    }

    static inline Vector<s32, 16> operator & (Vector<s32, 16> a, Vector<s32, 16> b)
    {
        return simd::bitwise_and(a, b);
    }

    static inline Vector<s32, 16> operator | (Vector<s32, 16> a, Vector<s32, 16> b)
    {
        return simd::bitwise_or(a, b);
    }

    static inline Vector<s32, 16> operator ^ (Vector<s32, 16> a, Vector<s32, 16> b)
    {
        return simd::bitwise_xor(a, b);
    }

    static inline Vector<s32, 16> min(Vector<s32, 16> a, Vector<s32, 16> b)
    {
        return simd::min(a, b);
    }

    static inline Vector<s32, 16> max(Vector<s32, 16> a, Vector<s32, 16> b)
    {
        return simd::max(a, b);
    }

    static inline mask32x16 operator > (Vector<s32, 16> a, Vector<s32, 16> b)
    {
        return simd::compare_gt(a, b);
    }

    static inline mask32x16 operator < (Vector<s32, 16> a, Vector<s32, 16> b)
    {
        return simd::compare_gt(b, a);
    }

    static inline mask32x16 operator == (Vector<s32, 16> a, Vector<s32, 16> b)
    {
        return simd::compare_eq(a, b);
    }

    static inline Vector<s32, 16> select(mask32x16 mask, Vector<s32, 16> a, Vector<s32, 16> b)
    {
        return simd::select(mask, a, b);
    }

    static inline Vector<s32, 16> operator << (Vector<s32, 16> a, int b)
    {
        return simd::sll(a, b);
    }

    static inline Vector<s32, 16> operator >> (Vector<s32, 16> a, int b)
    {
        return simd::sra(a, b);
    }

    static inline Vector<s32, 16> operator << (Vector<s32, 16> a, Vector<u32, 16> b)
    {
        return simd::sll(a, b);
    }

    static inline Vector<s32, 16> operator >> (Vector<s32, 16> a, Vector<u32, 16> b)
    {
        return simd::sra(a, b);
    }

} // namespace mango
