#pragma once
#include <windows.h>
#include <windowsx.h>
#include <sstream>
#include <assert.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>

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