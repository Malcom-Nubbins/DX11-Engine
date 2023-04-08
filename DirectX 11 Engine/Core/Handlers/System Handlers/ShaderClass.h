#pragma once
#include "../../Globals/stdafx.h"
#include "../../Globals/AppValues.h"

class ShaderClass
{
private:
	static ID3D11SamplerState * m_SSLinear;
	static ID3D11SamplerState * m_SSAnisotropic;
	static ID3D11SamplerState* m_SSShadowSamplerComparison;
	/*ID3D11SamplerState * _DepthNormal = nullptr;
	ID3D11SamplerState * _RandomVec = nullptr;
	ID3D11SamplerState* _SSAOBlurSampler = nullptr;
	
	ID3D11SamplerState* _ScreenBlurSampler = nullptr;*/

private:
	static HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	static HRESULT CreateInputLayout(D3D11_INPUT_ELEMENT_DESC layout[], UINT numElements, ID3D11InputLayout** inputLayout, ID3DBlob* vsBlob);
	static HRESULT CreateInputLayout(D3D11_INPUT_ELEMENT_DESC layout[], UINT numElements, ID3D11InputLayout** inputLayout, char* bytes, size_t len);
public:
	ShaderClass();
	~ShaderClass();
	void Cleanup();

	static HRESULT CreateVertexShader(WCHAR* shaderFilename, ID3D11VertexShader** vertexShader, ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC layout[], UINT numElements);
	static HRESULT CreateHullShader(WCHAR* shaderFilename, ID3D11HullShader** hullShader);
	static HRESULT CreateDomainShader(WCHAR* shaderFilename, ID3D11DomainShader** domainShader);
	static HRESULT CreatePixelShader(WCHAR* shaderFilename, ID3D11PixelShader** pixelShader);

	static void SetShadersAndInputLayout(ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11InputLayout* inputLayout);
	static void SetHullAndDomainShaders(ID3D11HullShader* hullShader, ID3D11DomainShader* domainShader);

	static void UnbindTesselationStages();

	HRESULT CreateSamplerStates();
	static ID3D11SamplerState** GetSamplerState(SamplerType type);

};

