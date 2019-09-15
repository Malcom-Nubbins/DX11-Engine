#include "HeatHaze.h"

HeatHaze::HeatHaze(const SystemHandlers& systemHandlers)
	: _systemHandlers(systemHandlers), _quad(), _values()
{
	_heatHazePS = nullptr;
	_inputLayout = nullptr;

	_renderTargetTex2D = nullptr;
	_renderTargetView = nullptr;
	_renderTargetSRV = nullptr;

	_heatHazeValues = nullptr;
}

HeatHaze::~HeatHaze()
{
}

void HeatHaze::Cleanup() const
{
	_heatHazePS->Release();
	//_inputLayout->Release();

	_renderTargetTex2D->Release();
	_renderTargetView->Release();
	_renderTargetSRV->Release();

	_heatHazeValues->Release();
}

HRESULT HeatHaze::Initialise(const float width, const float height)
{
	HRESULT hr;
	hr = InitialiseRenderTarget(width, height);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to Create Heat Haze Shader", L"Error", MB_OK);
		return hr;
	}

	hr = InitialiseShaders();
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to Create Heat Haze RTV", L"Error", MB_OK);
		return hr;
	}

	BuildQuad();

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(HeatHazeValues);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	_systemHandlers.GetD3DClass()->GetDevice()->CreateBuffer(&bd, nullptr, &_heatHazeValues);

	return S_OK;
}

void HeatHaze::Resize(const float width, const float height)
{
	_renderTargetTex2D->Release();
	_renderTargetView->Release();
	_renderTargetSRV->Release();

	InitialiseRenderTarget(width, height);
}

HRESULT HeatHaze::InitialiseRenderTarget(const float width, const float height)
{
	HRESULT hr;
	D3D11_TEXTURE2D_DESC renderTargetTexDesc;
	renderTargetTexDesc.Width = static_cast<UINT>(width);
	renderTargetTexDesc.Height = static_cast<UINT>(height);
	renderTargetTexDesc.MipLevels = 1;
	renderTargetTexDesc.ArraySize = 1;
	renderTargetTexDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	renderTargetTexDesc.SampleDesc.Count = 1;
	renderTargetTexDesc.SampleDesc.Quality = 0;
	renderTargetTexDesc.Usage = D3D11_USAGE_DEFAULT;
	renderTargetTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	renderTargetTexDesc.CPUAccessFlags = 0;
	renderTargetTexDesc.MiscFlags = 0;

	_systemHandlers.GetD3DClass()->GetDevice()->CreateTexture2D(&renderTargetTexDesc, nullptr, &_renderTargetTex2D);

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetDesc;
	renderTargetDesc.Format = renderTargetTexDesc.Format;
	renderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetDesc.Texture2D.MipSlice = 0;

	hr = _systemHandlers.GetD3DClass()->GetDevice()->CreateRenderTargetView(_renderTargetTex2D, &renderTargetDesc, &_renderTargetView);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to create Render Target View (BasicLight)", L"Error", MB_OK);
		return hr;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = renderTargetTexDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	_systemHandlers.GetD3DClass()->GetDevice()->CreateShaderResourceView(_renderTargetTex2D, &srvDesc, &_renderTargetSRV);

	return S_OK;
}

HRESULT HeatHaze::InitialiseShaders()
{
	HRESULT hr = _systemHandlers.GetShaderClass()->CreatePixelShader((WCHAR*)L"Core/Shaders/HeatHazePS.hlsl", &_heatHazePS);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

void HeatHaze::BuildQuad()
{
	HRESULT hr = _systemHandlers.GetBufferClass()->CreateQuad(&_quad.vertexBuffer, &_quad.indexBuffer);
	if (FAILED(hr))
	{
		return;
	}

	_quad.numberOfIndices = 6;
	_quad.vertexBufferOffset = 0;
	_quad.vertexBufferStride = sizeof(SimpleQuad);
}

void HeatHaze::SetAsCurrentRenderTarget() const
{
	_systemHandlers.GetRenderClass()->SetRenderTargetAndDepthStencil(_renderTargetView, nullptr);
}

void HeatHaze::SetAsCurrentPixelShader() const
{
	_systemHandlers.GetShaderClass()->SetShadersAndInputLayout(nullptr, _heatHazePS, nullptr);
}

void HeatHaze::Update(const float deltaTime)
{
	_values.time = deltaTime;
}

void HeatHaze::Render(ID3D11ShaderResourceView * textureToProcess, TextureHandler* texHandler, std::string season)
{
	if (season == "Winter")
	{
		_values.blizzard = 1.0f;
		_values.heatwave = 0.0f;

		ID3D11ShaderResourceView* snowTex = texHandler->GetSnowTexture();
		_systemHandlers.GetD3DClass()->GetContext()->PSSetShaderResources(1, 1, &snowTex);
	}
	else if (season == "Summer")
	{
		_values.blizzard = 0.0f;
		_values.heatwave = 1.0f;

		ID3D11ShaderResourceView* distortionMap = texHandler->GetDistortionMap();
		_systemHandlers.GetD3DClass()->GetContext()->PSSetShaderResources(1, 1, &distortionMap);
	}

	_systemHandlers.GetBufferClass()->SetPixelShaderBuffers(&_heatHazeValues);

	_systemHandlers.GetD3DClass()->GetContext()->PSSetSamplers(0, 1, _systemHandlers.GetShaderClass()->GetSamplerState(LINEAR));
	SetAsCurrentRenderTarget();
	SetAsCurrentPixelShader();

	_systemHandlers.GetD3DClass()->GetContext()->IASetVertexBuffers(0, 1, &_quad.vertexBuffer, &_quad.vertexBufferStride, &_quad.vertexBufferOffset);
	_systemHandlers.GetD3DClass()->GetContext()->IASetIndexBuffer(_quad.indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	_systemHandlers.GetD3DClass()->GetContext()->PSSetShaderResources(0, 1, &textureToProcess);

	_systemHandlers.GetD3DClass()->GetContext()->UpdateSubresource(_heatHazeValues, 0, nullptr, &_values, 0, 0);

	_systemHandlers.GetD3DClass()->GetContext()->DrawIndexed(_quad.numberOfIndices, 0, 0);
}
