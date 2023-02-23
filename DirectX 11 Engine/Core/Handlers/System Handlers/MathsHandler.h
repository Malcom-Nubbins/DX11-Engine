#pragma once
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <time.h>
#include "../../Globals/Structs.h"

using namespace DirectX;
class MathsHandler
{
public:
	MathsHandler();
	~MathsHandler();

	static XMFLOAT2 Add(const XMFLOAT2& a, const XMFLOAT2& b);
	static XMFLOAT3 Add(const XMFLOAT3& a, const XMFLOAT3& b);

	static XMFLOAT2 Subtract(const XMFLOAT2& a, const XMFLOAT2& b);
	static XMFLOAT3 Subtract(const XMFLOAT3& a, const XMFLOAT3& b);

	static XMFLOAT2 Multiply(const XMFLOAT2& a, const XMFLOAT2& b);
	static XMFLOAT3 Multiply(const XMFLOAT3& a, const XMFLOAT3& b);

	static XMFLOAT2 Divide(const XMFLOAT2& a, const XMFLOAT2& b);
	static XMFLOAT3 Divide(const XMFLOAT3& a, const XMFLOAT3& b);

	static XMFLOAT3 CalculateTangent(TempVertex v1, TempVertex v2, TempVertex v3);
	static XMFLOAT3 CalculateBinormal(TempVertex v1, TempVertex v2, TempVertex v3);

	static float Magnitude(XMFLOAT3& const inVec);

	static float DotProduct(const XMFLOAT3& src, const XMFLOAT3& dst);

	static XMFLOAT4 Make4x4(XMFLOAT3 const& inPos, XMFLOAT4X4 const& inMatrix);
	static bool WorldToScreen(XMFLOAT3 const inPos, XMFLOAT2& outScreenPos, XMFLOAT4X4 const world, XMFLOAT4X4 const view, XMFLOAT4X4 const proj, XMFLOAT2 const screenSize);

	static float RandomFloat(float one, float two);
};

