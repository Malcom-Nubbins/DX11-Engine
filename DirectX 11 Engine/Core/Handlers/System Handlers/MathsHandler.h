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

	static float RandomFloat(float one, float two);
};

