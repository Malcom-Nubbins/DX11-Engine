#pragma once
#include <windows.h>
#include <windowsx.h>
#include <sstream>
#include <assert.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <memory>
#include <string>
#include <stdexcept>

#include <dxgi1_6.h>

#include <wrl.h>

using namespace Microsoft::WRL;

#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

#pragma warning(disable : 4996)

#include <algorithm>

typedef ID3D11Buffer VertexBuffer;
typedef ID3D11Buffer IndexBuffer;

typedef unsigned __int64 u64;
typedef unsigned __int32 u32;
typedef unsigned char u8;

enum SAMPLER_TYPE
{
	LINEAR,
	ANISOTROPIC,
	SHADOW,
	/*DEPTHNORMAL,
	RANDOMVEC,
	SSAOBLUR,
	BLURPOINT*/
};

enum RASTERIZER_TYPE
{
	NO_CULL,
	BACK_CULL,
	WIREFRAME,
	SHADOWDEPTH
};

enum SYSTEM_HANDLER
{
	D3D,
	SHADER,
	RENDER,
	BUFFER,
	WINDOW,
	INP,
	TEXTURE
};

enum class MovementState : u8
{
	None = 0x00,
	Forward = 0x01,
	Backward = 0x02,
	Left = 0x04,
	Right = 0x08
};

inline constexpr MovementState operator~(MovementState a) 
{
	return static_cast<MovementState>(~static_cast<u8>(a));
}

inline constexpr MovementState operator|(MovementState lhs, MovementState const& rhs)
{
	return static_cast<MovementState>(static_cast<u8>(lhs) | static_cast<u8>(rhs));
}

inline constexpr MovementState operator&(MovementState lhs, MovementState const& rhs)
{
	return static_cast<MovementState>(static_cast<u8>(lhs) & static_cast<u8>(rhs));
}

inline constexpr MovementState& operator|=(MovementState& lhs, MovementState rhs)
{
	lhs = static_cast<MovementState>(static_cast<u8>(lhs) | static_cast<u8>(rhs));
	return lhs;
}



inline void ThrowIfFailed(HRESULT const hr)
{
	if (FAILED(hr))
	{
		throw std::exception();
	}
}

inline void ThrowIfFailedBool(BOOL const inResult)
{
	if (!inResult)
	{
		throw std::exception();
	}
}

template<typename ... Args>
std::string FormatString(const std::string& format, Args ... args)
{
	int size_s = _snprintf(nullptr, 0, format.c_str(), args...) + 1;
	if (size_s <= 0)
	{
		throw std::runtime_error("Error during string formatting");
	}

	size_t size = static_cast<size_t>(size_s);
	auto buff = std::make_unique<char[]>(size);

	_snprintf(buff.get(), size, format.c_str(), args...);

	return std::string(buff.get(), buff.get() + size - 1);
}

template<typename ... Args>
std::string FormatCString(char const* const format, Args ... args)
{
	int size_s = _snprintf(nullptr, 0, format, args...) + 1;
	if (size_s <= 0)
	{
		throw std::runtime_error("Error during string formatting");
	}

	size_t size = static_cast<size_t>(size_s);
	auto buff = std::make_unique<char[]>(size);

	_snprintf(buff.get(), size, format, args...);

	return std::string(buff.get(), buff.get() + size - 1);
}