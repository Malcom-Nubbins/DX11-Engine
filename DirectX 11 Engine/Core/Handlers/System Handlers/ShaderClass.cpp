#include "ShaderClass.h"
#include "../../ApplicationNew.h"
#include "../FileReaderWriter.h"

ID3D11SamplerState* ShaderClass::m_SSLinear(nullptr);
ID3D11SamplerState* ShaderClass::m_SSAnisotropic(nullptr);
ID3D11SamplerState* ShaderClass::m_SSShadowSamplerComparison(nullptr);

ShaderClass::ShaderClass()
{
}


ShaderClass::~ShaderClass()
{
}

void ShaderClass::Cleanup()
{
	if (m_SSLinear) m_SSLinear->Release();
	if (m_SSAnisotropic) m_SSAnisotropic->Release();
	if (m_SSShadowSamplerComparison) m_SSShadowSamplerComparison->Release();
}

HRESULT ShaderClass::CreateVertexShader(WCHAR * shaderFilename, ID3D11VertexShader** vertexShader, ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC layout[], UINT numElements)
{
	auto device = ApplicationNew::Get().GetDevice();
	HRESULT hr;

	char* bytes = { 0 };
	size_t size = FileReaderWriter::ReadFileBytes(shaderFilename, bytes);

	if (size != 0)
	{
		// Create the vertex shader
		hr = device->CreateVertexShader((void*)bytes, size, nullptr, vertexShader);

		if (FAILED(hr))
		{
			free(bytes);
			return hr;
		}

		hr = CreateInputLayout(layout, numElements, inputLayout, bytes, size);
		if (FAILED(hr))
		{
			free(bytes);
			return hr;
		}
	}
	else
	{
		ID3DBlob* vsBlob;
		// Fallback
		hr = CompileShaderFromFile(shaderFilename, "main", "vs_5_0", &vsBlob);
		if (FAILED(hr))
		{
			MessageBox(nullptr,
				L"The Vertex shader cannot be compiled. Please check that the HLSL file is within the Core/Shaders/ Directory", L"Error", MB_OK);
			return hr;
		}

		// Create the vertex shader
		hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, vertexShader);

		if (FAILED(hr))
		{
			free(bytes);
			vsBlob->Release();
			return hr;
		}

		CreateInputLayout(layout, numElements, inputLayout, vsBlob);
		vsBlob->Release();
	}

	free(bytes);

	return S_OK;
}

HRESULT ShaderClass::CreateHullShader(WCHAR * shaderFilename, ID3D11HullShader ** hullShader)
{
	auto device = ApplicationNew::Get().GetDevice();
	HRESULT hr;

	char* bytes = { 0 };
	size_t size = FileReaderWriter::ReadFileBytes(shaderFilename, bytes);

	if (size != 0)
	{
		hr = device->CreateHullShader((void*)bytes, size, nullptr, hullShader);
		if (FAILED(hr))
		{
			free(bytes);
			return hr;
		}
	}
	else
	{
		ID3DBlob* hsBlob = nullptr;
		hr = CompileShaderFromFile(shaderFilename, "main", "hs_5_0", &hsBlob);

		if (FAILED(hr))
		{
			MessageBox(nullptr,
				L"The Hull shader cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
			return hr;
		}

		// Create the pixel shader
		hr = device->CreateHullShader(hsBlob->GetBufferPointer(), hsBlob->GetBufferSize(), nullptr, hullShader);
		hsBlob->Release();

		if (FAILED(hr))
			return hr;
	}

	free(bytes);

	return S_OK;
}

HRESULT ShaderClass::CreateDomainShader(WCHAR * shaderFilename, ID3D11DomainShader ** domainShader)
{
	auto device = ApplicationNew::Get().GetDevice();
	HRESULT hr;

	char* bytes = { 0 };
	size_t size = FileReaderWriter::ReadFileBytes(shaderFilename, bytes);

	if (size != 0)
	{
		hr = device->CreateDomainShader((void*)bytes, size, nullptr, domainShader);
		if (FAILED(hr))
		{
			free(bytes);
			return hr;
		}
	}
	else
	{
		ID3DBlob* dsBlob = nullptr;
		hr = CompileShaderFromFile(shaderFilename, "main", "ds_5_0", &dsBlob);

		if (FAILED(hr))
		{
			MessageBox(nullptr,
				L"The Domain shader cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
			return hr;
		}

		// Create the pixel shader
		hr = device->CreateDomainShader(dsBlob->GetBufferPointer(), dsBlob->GetBufferSize(), nullptr, domainShader);
		dsBlob->Release();

		if (FAILED(hr))
			return hr;
	}

	free(bytes);

	return S_OK;
}

HRESULT ShaderClass::CreatePixelShader(WCHAR * shaderFilename, ID3D11PixelShader ** pixelShader)
{
	auto device = ApplicationNew::Get().GetDevice();
	HRESULT hr;

	char* bytes = { 0 };
	size_t size = FileReaderWriter::ReadFileBytes(shaderFilename, bytes);

	if (size != 0)
	{
		hr = device->CreatePixelShader((void*)bytes, size, nullptr, pixelShader);
		if (FAILED(hr))
		{
			free(bytes);
			return hr;
		}
	}
	else
	{
		ID3DBlob* psBlob = nullptr;
		hr = CompileShaderFromFile(shaderFilename, "main", "ps_5_0", &psBlob);

		if (FAILED(hr))
		{
			MessageBox(nullptr,
				L"The Pixel shader cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
			return hr;
		}

		// Create the pixel shader
		hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, pixelShader);
		psBlob->Release();

		if (FAILED(hr))
			return hr;
	}

	free(bytes);

	return S_OK;
}

HRESULT ShaderClass::CreateInputLayout(D3D11_INPUT_ELEMENT_DESC layout[], UINT numElements, ID3D11InputLayout** inputLayout, ID3DBlob* vsBlob)
{
	auto device = ApplicationNew::Get().GetDevice();
	HRESULT hr;

	hr = device->CreateInputLayout(layout, numElements, vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(), inputLayout);

	if (FAILED(hr))
		return hr;

	return hr;
}

HRESULT ShaderClass::CreateInputLayout(D3D11_INPUT_ELEMENT_DESC layout[], UINT numElements, ID3D11InputLayout** inputLayout, char* bytes, size_t len)
{
	auto device = ApplicationNew::Get().GetDevice();
	HRESULT hr;

	hr = device->CreateInputLayout(layout, numElements, (void*)bytes, len, inputLayout);

	if (FAILED(hr))
		return hr;

	return hr;
}

void ShaderClass::SetShadersAndInputLayout(ID3D11VertexShader * vertexShader, ID3D11PixelShader * pixelShader, ID3D11InputLayout * inputLayout)
{
	auto context = ApplicationNew::Get().GetContext();
	if(inputLayout != nullptr)
		context->IASetInputLayout(inputLayout);

	if(vertexShader != nullptr)
		context->VSSetShader(vertexShader, nullptr, 0);

	if (pixelShader != nullptr)
		context->PSSetShader(pixelShader, nullptr, 0);
}

void ShaderClass::SetHullAndDomainShaders(ID3D11HullShader * hullShader, ID3D11DomainShader * domainShader)
{
	auto context = ApplicationNew::Get().GetContext();

	if (hullShader != nullptr)
		context->HSSetShader(hullShader, nullptr, 0);

	if (domainShader != nullptr)
		context->DSSetShader(domainShader, nullptr, 0);
}

void ShaderClass::UnbindTesselationStages()
{
	auto context = ApplicationNew::Get().GetContext();
	context->HSSetShader(nullptr, nullptr, 0);
	context->DSSetShader(nullptr, nullptr, 0);
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
	auto device = ApplicationNew::Get().GetDevice();

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
	hr = device->CreateSamplerState(&sampDesc, &m_SSLinear);
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
	hr = device->CreateSamplerState(&sampDesc, &m_SSAnisotropic);
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
	hr = device->CreateSamplerState(&sampDesc, &m_SSShadowSamplerComparison);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

ID3D11SamplerState ** ShaderClass::GetSamplerState(SAMPLER_TYPE type)
{
	switch (type)
	{
	case LINEAR:
		return &m_SSLinear;
	case ANISOTROPIC:
		return &m_SSAnisotropic;
	case SHADOW:
		return &m_SSShadowSamplerComparison;
	default:
		return &m_SSLinear;
	}
}
