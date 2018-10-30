#include "MathsHandler.h"



MathsHandler::MathsHandler()
{
}


MathsHandler::~MathsHandler()
{
}

XMFLOAT2 MathsHandler::Add(const XMFLOAT2& a, const XMFLOAT2& b)
{
	XMFLOAT2 result = XMFLOAT2(0.0f, 0.0f);
	XMStoreFloat2(&result, XMVectorAdd(XMLoadFloat2(&a), XMLoadFloat2(&b)));

	return result;
}

XMFLOAT3 MathsHandler::Add(const XMFLOAT3& a, const XMFLOAT3& b)
{
	XMFLOAT3 result = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMStoreFloat3(&result, XMVectorAdd(XMLoadFloat3(&a), XMLoadFloat3(&b)));

	return result;
}

XMFLOAT2 MathsHandler::Subtract(const XMFLOAT2& a, const XMFLOAT2& b)
{
	XMFLOAT2 result = XMFLOAT2(0.0f, 0.0f);
	XMStoreFloat2(&result, XMVectorSubtract(XMLoadFloat2(&a), XMLoadFloat2(&b)));

	return result;
}

XMFLOAT3 MathsHandler::Subtract(const XMFLOAT3& a, const XMFLOAT3& b)
{
	XMFLOAT3 result = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMStoreFloat3(&result, XMVectorSubtract(XMLoadFloat3(&a), XMLoadFloat3(&b)));

	return result;
}

XMFLOAT2 MathsHandler::Multiply(const XMFLOAT2& a, const XMFLOAT2& b)
{
	XMFLOAT2 result = XMFLOAT2(0.0f, 0.0f);
	XMStoreFloat2(&result, XMVectorMultiply(XMLoadFloat2(&a), XMLoadFloat2(&b)));

	return result;
}

XMFLOAT3 MathsHandler::Multiply(const XMFLOAT3& a, const XMFLOAT3& b)
{
	XMFLOAT3 result = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMStoreFloat3(&result, XMVectorMultiply(XMLoadFloat3(&a), XMLoadFloat3(&b)));

	return result;
}

XMFLOAT2 MathsHandler::Divide(const XMFLOAT2& a, const XMFLOAT2& b)
{
	XMFLOAT2 result = XMFLOAT2(0.0f, 0.0f);
	XMStoreFloat2(&result, XMVectorDivide(XMLoadFloat2(&a), XMLoadFloat2(&b)));

	return result;
}

XMFLOAT3 MathsHandler::Divide(const XMFLOAT3& a, const XMFLOAT3& b)
{
	XMFLOAT3 result = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMStoreFloat3(&result, XMVectorDivide(XMLoadFloat3(&a), XMLoadFloat3(&b)));

	return result;
}

XMFLOAT3 MathsHandler::CalculateTangent(TempVertex v1, TempVertex v2, TempVertex v3)
{
	float length;
	float den;
	XMFLOAT3 vector1, vector2;
	XMFLOAT2 tuVector, tvVector;

	vector1 = XMFLOAT3(v2.pos.x - v1.pos.x, v2.pos.y - v1.pos.y, v2.pos.z - v1.pos.z);
	vector2 = XMFLOAT3(v3.pos.x - v1.pos.x, v3.pos.y - v1.pos.y, v3.pos.z - v1.pos.z);

	tuVector = XMFLOAT2(v2.tex.x - v1.tex.x, v3.tex.x - v1.tex.x);
	tvVector = XMFLOAT2(v2.tex.y - v1.tex.y, v3.tex.y - v1.tex.y);

	den = 1.0f / (tuVector.x * tvVector.y - tuVector.y * tvVector.x);

	XMFLOAT3 outTan = XMFLOAT3(0.0f, 0.0f, 0.0f);

	outTan.x = (tvVector.y * vector1.x - tvVector.x * vector2.x) * den;
	outTan.y = (tvVector.y * vector1.y - tvVector.x * vector2.y) * den;
	outTan.z = (tvVector.y * vector1.z - tvVector.x * vector2.z) * den;

	length = sqrt((outTan.x * outTan.x) + (outTan.y * outTan.y) + (outTan.z * outTan.z));

	outTan.x /= length;
	outTan.y /= length;
	outTan.z /= length;

	return outTan;
}

XMFLOAT3 MathsHandler::CalculateBinormal(TempVertex v1, TempVertex v2, TempVertex v3)
{
	float length;
	float den;
	XMFLOAT3 vector1, vector2;
	XMFLOAT2 tuVector, tvVector;

	vector1 = XMFLOAT3(v2.pos.x - v1.pos.x, v2.pos.y - v1.pos.y, v2.pos.z - v1.pos.z);
	vector2 = XMFLOAT3(v3.pos.x - v1.pos.x, v3.pos.y - v1.pos.y, v3.pos.z - v1.pos.z);

	tuVector = XMFLOAT2(v2.tex.x - v1.tex.x, v3.tex.x - v1.tex.x);
	tvVector = XMFLOAT2(v2.tex.y - v1.tex.y, v3.tex.y - v1.tex.y);

	den = 1.0f / (tuVector.x * tvVector.y - tuVector.y * tvVector.x);

	XMFLOAT3 outBinorm = XMFLOAT3(0.0f, 0.0f, 0.0f);

	outBinorm.x = (tuVector.x * vector2.x - tuVector.y * vector1.x) * den;
	outBinorm.y = (tuVector.x * vector2.y - tuVector.y * vector1.y) * den;
	outBinorm.z = (tuVector.x * vector2.z - tuVector.y * vector1.z) * den;

	length = sqrt((outBinorm.x * outBinorm.x) + (outBinorm.y * outBinorm.y) + (outBinorm.z * outBinorm.z));

	outBinorm.x = outBinorm.x / length;
	outBinorm.y = outBinorm.y / length;
	outBinorm.z = outBinorm.z / length;

	return outBinorm;
}

float MathsHandler::RandomFloat(float one, float two)
{
	float random = one + static_cast<float>(rand()) / RAND_MAX * (two - one);
	return random;
}
