#include "ShaderClass.h"

ShaderClass::ShaderClass(D3DClass* d3dClass) : _d3dClass(d3dClass)
{
}


ShaderClass::~ShaderClass()
{
}

void ShaderClass::Cleanup()
{
	if (_linear) _linear->Release();
	if (_anisotropic) _anisotropic->Release();
	if (_shadowSamplerComparison) _shadowSamplerComparison->Release();
}

HRESULT ShaderClass::CreateVertexShader(WCHAR * shaderFilename, ID3D11VertexShader** vertexShader, ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC layout[], UINT numElements)
{
	HRESULT hr;
	ID3DBlob* vsBlob;

	hr = CompileShaderFromFile(shaderFilename, "main", "vs_5_0", &vsBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The Vertex shader cannot be compiled. Please check that the HLSL file is within the Core/Shaders/ Directory", L"Error", MB_OK);
		return hr;
	}

	// Create the vertex shader
	hr = _d3dClass->GetDevice()->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, vertexShader);

	if (FAILED(hr))
	{
		vsBlob->Release();
		return hr;
	}

	CreateInputLayout(layout, numElements, inputLayout, vsBlob);

	//_vsBlob->Release();

	return S_OK;
}

HRESULT ShaderClass::CreateHullShader(WCHAR * shaderFilename, ID3D11HullShader ** hullShader)
{
	HRESULT hr;
	ID3DBlob* hsBlob = nullptr;
	hr = CompileShaderFromFile(shaderFilename, "main", "hs_5_0", &hsBlob);

	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The Hull shader cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = _d3dClass->GetDevice()->CreateHullShader(hsBlob->GetBufferPointer(), hsBlob->GetBufferSize(), nullptr, hullShader);
	hsBlob->Release();

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT ShaderClass::CreateDomainShader(WCHAR * shaderFilename, ID3D11DomainShader ** domainShader)
{
	HRESULT hr;
	ID3DBlob* dsBlob = nullptr;
	hr = CompileShaderFromFile(shaderFilename, "main", "ds_5_0", &dsBlob);

	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The Domain shader cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = _d3dClass->GetDevice()->CreateDomainShader(dsBlob->GetBufferPointer(), dsBlob->GetBufferSize(), nullptr, domainShader);
	dsBlob->Release();

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT ShaderClass::CreatePixelShader(WCHAR * shaderFilename, ID3D11PixelShader ** pixelShader)
{
	HRESULT hr;
	ID3DBlob* psBlob = nullptr;
	hr = CompileShaderFromFile(shaderFilename, "main", "ps_5_0", &psBlob);

	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The Pixel shader cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = _d3dClass->GetDevice()->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, pixelShader);
	psBlob->Release();

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT ShaderClass::CreateInputLayout(D3D11_INPUT_ELEMENT_DESC layout[], UINT numElements, ID3D11InputLayout** inputLayout, ID3DBlob* vsBlob)
{
	HRESULT hr;

	hr = _d3dClass->GetDevice()->CreateInputLayout(layout, numElements, vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(), inputLayout);

	if (FAILED(hr))
		return hr;

	vsBlob->Release();

	return hr;
}

void ShaderClass::SetShadersAndInputLayout(ID3D11VertexShader * vertexShader, ID3D11PixelShader * pixelShader, ID3D11InputLayout * inputLayout)
{
	if(inputLayout != nullptr)
		_d3dClass->GetContext()->IASetInputLayout(inputLayout);

	if(vertexShader != nullptr)
		_d3dClass->GetContext()->VSSetShader(vertexShader, nullptr, 0);

	if (pixelShader != nullptr)
		_d3dClass->GetContext()->PSSetShader(pixelShader, nullptr, 0);
}

void ShaderClass::SetHullAndDomainShaders(ID3D11HullShader * hullShader, ID3D11DomainShader * domainShader)
{
	if (hullShader != nullptr)
		_d3dClass->GetContext()->HSSetShader(hullShader, nullptr, 0);

	if (domainShader != nullptr)
		_d3dClass->GetContext()->DSSetShader(domainShader, nullptr, 0);
}

void ShaderClass::UnbindTesselationStages()
{
	_d3dClass->GetContext()->HSSetShader(nullptr, nullptr, 0);
	_d3dClass->GetContext()->DSSetShader(nullptr, nullptr, 0);
}

HRESULT ShaderClass::CompileShaderFromFile(WCHAR * filename, LPCSTR entrypoint, LPCSTR shaderModel, ID3DBlob ** outBlob)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	
	ID3DBlob* pErrorBlob;
	hr = D3DCompileFromFile(filename, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entrypoint, shaderModel,
		dwShaderFlags, 0, outBlob, &pErrorBlob);

	if (FAILED(hr))
	{
		if (pErrorBlob != nullptr)
			OutputDebugStringA(static_cast<char*>(pErrorBlob->GetBufferPointer()));

		if (pErrorBlob) pErrorBlob->Release();

		MessageBox(nullptr,
			L"Could not compile the shader. Check the output for more information", L"Error", MB_OK);

		return hr;
	}

	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}

HRESULT ShaderClass::CreateSamplerStates()
{
	HRESULT hr;
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));

	// Linear
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = _d3dClass->GetDevice()->CreateSamplerState(&sampDesc, &_linear);
	if (FAILED(hr))
		return hr;

	// Anisotropic
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.MaxAnisotropy = 16;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = _d3dClass->GetDevice()->CreateSamplerState(&sampDesc, &_anisotropic);
	if (FAILED(hr))
		return hr;

	sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	sampDesc.BorderColor[0] = 1.0f;
	sampDesc.BorderColor[1] = 1.0f;
	sampDesc.BorderColor[2] = 1.0f;
	sampDesc.BorderColor[3] = 1e5f;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = _d3dClass->GetDevice()->CreateSamplerState(&sampDesc, &_shadowSamplerComparison);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

ID3D11SamplerState ** ShaderClass::GetSamplerState(SAMPLER_TYPE type)
{
	switch (type)
	{
	case LINEAR:
		return &_linear;
	case ANISOTROPIC:
		return &_anisotropic;
	case SHADOW:
		return &_shadowSamplerComparison;
	default:
		return &_linear;
	}
}
