#pragma once
#include "D3DClass.h"
#include "../Globals/stdafx.h"
#include "../Globals/AppValues.h"
class ShaderClass
{
private:
	D3DClass * _d3dClass;

	ID3D11SamplerState * _linear = nullptr;
	ID3D11SamplerState * _anisotropic = nullptr;
	ID3D11SamplerState* _shadowSamplerComparison = nullptr;
	/*ID3D11SamplerState * _DepthNormal = nullptr;
	ID3D11SamplerState * _RandomVec = nullptr;
	ID3D11SamplerState* _SSAOBlurSampler = nullptr;
	
	ID3D11SamplerState* _ScreenBlurSampler = nullptr;*/

private:
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT CreateInputLayout(D3D11_INPUT_ELEMENT_DESC layout[], UINT numElements, ID3D11InputLayout** inputLayout, ID3DBlob* vsBlob);
public:
	ShaderClass(D3DClass* d3dClass);
	~ShaderClass();
	void Cleanup();

	HRESULT CreateVertexShader(WCHAR* shaderFilename, ID3D11VertexShader** vertexShader, ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC layout[], UINT numElements);
	HRESULT CreatePixelShader(WCHAR* shaderFilename, ID3D11PixelShader** pixelShader);

	void SetShadersAndInputLayout(ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11InputLayout* inputLayout);

	HRESULT CreateSamplerStates();
	ID3D11SamplerState** GetSamplerState(SAMPLER_TYPE type);

};

