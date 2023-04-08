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

#include "uint4x4.h"
#include "uint4.inl"

uint4x4::uint4x4( const uint32_t diagonal )
{
	rows[0][0] = diagonal;
	rows[1][1] = diagonal;
	rows[2][2] = diagonal;
	rows[3][3] = diagonal;
}

uint4x4::uint4x4( const uint4& diagonal )
{
	rows[0][0] = diagonal[0];
	rows[1][1] = diagonal[1];
	rows[2][2] = diagonal[2];
	rows[3][3] = diagonal[3];
}

// Sets each row of the matrix to the corresponding vector.
uint4x4::uint4x4( const uint4& row0, const uint4& row1, const uint4& row2, const uint4& row3 )
{
	rows[0] = row0;
	rows[1] = row1;
	rows[2] = row2;
	rows[3] = row3;
}

// Sets each component of the matrix to the corresponding scalar value.
uint4x4::uint4x4( const uint32_t m00, const uint32_t m01, const uint32_t m02, const uint32_t m03,
					const uint32_t m10, const uint32_t m11, const uint32_t m12, const uint32_t m13,
					const uint32_t m20, const uint32_t m21, const uint32_t m22, const uint32_t m23,
					const uint32_t m30, const uint32_t m31, const uint32_t m32, const uint32_t m33 )
{
	rows[0] = uint4( m00, m01, m02, m03 );
	rows[1] = uint4( m10, m11, m12, m13 );
	rows[2] = uint4( m20, m21, m22, m23 );
	rows[3] = uint4( m30, m31, m32, m33 );
}

uint4x4::uint4x4( const uint4x4& mat )
{
	rows[0] = mat[0];
	rows[1] = mat[1];
	rows[2] = mat[2];
	rows[3] = mat[3];
}

uint4x4::uint4x4( const bool4x4& mat )
{
	rows[0] = uint4( mat[0] );
	rows[1] = uint4( mat[1] );
	rows[2] = uint4( mat[2] );
	rows[3] = uint4( mat[3] );
}

uint4x4::uint4x4( const int4x4& mat )
{
	rows[0] = uint4( mat[0] );
	rows[1] = uint4( mat[1] );
	rows[2] = uint4( mat[2] );
	rows[3] = uint4( mat[3] );
}

uint4x4::uint4x4( const float4x4& mat )
{
	rows[0] = uint4( mat[0] );
	rows[1] = uint4( mat[1] );
	rows[2] = uint4( mat[2] );
	rows[3] = uint4( mat[3] );
}

uint4x4::uint4x4( const double4x4& mat )
{
	rows[0] = uint4( mat[0] );
	rows[1] = uint4( mat[1] );
	rows[2] = uint4( mat[2] );
	rows[3] = uint4( mat[3] );
}

uint4x4 uint4x4::operator=( const uint4x4& other )
{
	rows[0] = other[0];
	rows[1] = other[1];
	rows[2] = other[2];
	rows[3] = other[3];

	return *this;
}

uint4& uint4x4::operator[]( const int32_t index )
{
	HLML_ASSERT( index >= 0 && index < 4 );
	return rows[index];
}

const uint4& uint4x4::operator[]( const int32_t index ) const
{
	HLML_ASSERT( index >= 0 && index < 4 );
	return rows[index];
}
