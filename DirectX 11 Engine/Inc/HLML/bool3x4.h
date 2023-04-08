/*
===========================================================================

HLML
v2.2.0

MIT License:

Copyright (c) 2019 Dan Moody (daniel.guy.moody@gmail.com).

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

===========================================================================
*/

// THIS FILE WAS AUTOMATICALLY GENERATED BY A TOOL.
// EDITING THIS FILE MAY CAUSE SIDE EFFECTS.
// DO SO AT YOUR OWN RISK.

#pragma once

struct bool2x2;
struct bool2x3;
struct bool2x4;
struct bool3x2;
struct bool3x3;
struct bool4x2;
struct bool4x3;
struct bool4x4;
struct int2x2;
struct int2x3;
struct int2x4;
struct int3x2;
struct int3x3;
struct int3x4;
struct int4x2;
struct int4x3;
struct int4x4;
struct uint2x2;
struct uint2x3;
struct uint2x4;
struct uint3x2;
struct uint3x3;
struct uint3x4;
struct uint4x2;
struct uint4x3;
struct uint4x4;
struct float2x2;
struct float2x3;
struct float2x4;
struct float3x2;
struct float3x3;
struct float3x4;
struct float4x2;
struct float4x3;
struct float4x4;
struct double2x2;
struct double2x3;
struct double2x4;
struct double3x2;
struct double3x3;
struct double3x4;
struct double4x2;
struct double4x3;
struct double4x4;

#include "bool4.h"

struct bool3x4
{
	bool4 rows[3];

	// Default constructor.
	HLML_INLINE bool3x4() {}

	// Sets each diagonal component of the matrix to the given scalar.
	HLML_INLINE explicit bool3x4( const bool32_t diagonal );

	// Sets each diagonal component of the matrix to the corresponding vector component.
	HLML_INLINE bool3x4( const bool4& diagonal );

	// Sets each row of the matrix to the corresponding vector.
	HLML_INLINE bool3x4( const bool4& row0, const bool4& row1, const bool4& row2 );

	// Sets each component of the matrix to the corresponding scalar value.
	HLML_INLINE bool3x4( const bool32_t m00, const bool32_t m01, const bool32_t m02, const bool32_t m03,
					const bool32_t m10, const bool32_t m11, const bool32_t m12, const bool32_t m13,
					const bool32_t m20, const bool32_t m21, const bool32_t m22, const bool32_t m23 );

	// Copy constructor.
	HLML_INLINE bool3x4( const bool3x4& mat );

	// Conversion constructor.  Casts all components of 'mat' from type int32_t to type bool32_t.
	HLML_INLINE explicit bool3x4( const int3x4& mat );

	// Conversion constructor.  Casts all components of 'mat' from type uint32_t to type bool32_t.
	HLML_INLINE explicit bool3x4( const uint3x4& mat );

	// Conversion constructor.  Casts all components of 'mat' from type float to type bool32_t.
	HLML_INLINE explicit bool3x4( const float3x4& mat );

	// Conversion constructor.  Casts all components of 'mat' from type double to type bool32_t.
	HLML_INLINE explicit bool3x4( const double3x4& mat );

	HLML_INLINE ~bool3x4() {}

	// Sets each row of the matrix to be the same as the parameter.
	HLML_INLINE bool3x4 operator=( const bool3x4& mat );

	// Returns the row vector at the given index of the matrix.
	// Index CANNOT be lower than 0 or higher than 3.
	HLML_INLINE bool4& operator[]( const int32_t index );

	// Returns the row vector at the given index of the matrix.
	// Index CANNOT be lower than 0 or higher than 3.
	HLML_INLINE const bool4& operator[]( const int32_t index ) const;
};
