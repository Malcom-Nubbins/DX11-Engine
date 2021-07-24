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
#include <map>

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

typedef ComPtr<ID3D11Buffer> VertexBuffer;
typedef ComPtr<ID3D11Buffer> IndexBuffer;

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

enum class SettingType
{
	Graphics,
	Video,
	Engine
};

enum class UIOriginPoint
{
	TopLeft,
	TopMiddle,
	TopRight,
	CentreLeft,
	Centre,
	CentreRight,
	BottomLeft,
	BottomCentre,
	BottomRight
};

extern std::map<std::string, UIOriginPoint> g_OriginEnumStrings;

inline void BuildOriginPointEnumStrings()
{
	g_OriginEnumStrings.emplace(std::make_pair(std::string("TopLeft"), UIOriginPoint::TopLeft));
	g_OriginEnumStrings.emplace(std::make_pair(std::string("TopMiddle"), UIOriginPoint::TopMiddle));
	g_OriginEnumStrings.emplace(std::make_pair(std::string("TopRight"), UIOriginPoint::TopRight));
	g_OriginEnumStrings.emplace(std::make_pair(std::string("CentreLeft"), UIOriginPoint::CentreLeft));
	g_OriginEnumStrings.emplace(std::make_pair(std::string("Centre"), UIOriginPoint::Centre));
	g_OriginEnumStrings.emplace(std::make_pair(std::string("CentreRight"), UIOriginPoint::CentreRight));
	g_OriginEnumStrings.emplace(std::make_pair(std::string("BottomLeft"), UIOriginPoint::BottomLeft));
	g_OriginEnumStrings.emplace(std::make_pair(std::string("BottomCentre"), UIOriginPoint::BottomCentre));
	g_OriginEnumStrings.emplace(std::make_pair(std::string("BottomRight"), UIOriginPoint::BottomRight));
}

inline UIOriginPoint GetOriginEnumValueFromString(std::string& inString)
{
	return g_OriginEnumStrings[inString];
}

enum class UIAnchorPoint
{
	TopLeft,
	TopMiddle,
	TopRight,
	CentreLeft,
	Centre,
	CentreRight,
	BottomLeft,
	BottomCentre,
	BottomRight
};

extern std::map<std::string, UIAnchorPoint> g_AnchorEnumStrings;

inline void BuildAnchorPointEnumStrings()
{
	g_AnchorEnumStrings.emplace(std::make_pair(std::string("TopLeft"), UIAnchorPoint::TopLeft));
	g_AnchorEnumStrings.emplace(std::make_pair(std::string("TopMiddle"), UIAnchorPoint::TopMiddle));
	g_AnchorEnumStrings.emplace(std::make_pair(std::string("TopRight"), UIAnchorPoint::TopRight));
	g_AnchorEnumStrings.emplace(std::make_pair(std::string("CentreLeft"), UIAnchorPoint::CentreLeft));
	g_AnchorEnumStrings.emplace(std::make_pair(std::string("Centre"), UIAnchorPoint::Centre));
	g_AnchorEnumStrings.emplace(std::make_pair(std::string("CentreRight"), UIAnchorPoint::CentreRight));
	g_AnchorEnumStrings.emplace(std::make_pair(std::string("BottomLeft"), UIAnchorPoint::BottomLeft));
	g_AnchorEnumStrings.emplace(std::make_pair(std::string("BottomCentre"), UIAnchorPoint::BottomCentre));
	g_AnchorEnumStrings.emplace(std::make_pair(std::string("BottomRight"), UIAnchorPoint::BottomRight));
}

inline UIAnchorPoint GetAnchorEnumValueFromString(std::string& inString)
{
	return g_AnchorEnumStrings[inString];
}

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