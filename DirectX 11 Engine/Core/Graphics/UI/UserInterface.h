#pragma once
#include "../../Scene/Camera.h"
#include "UIBitmap.h"
#include "../../Handlers/SystemHandlers.h"
#include "../Core/Loaders/IConfigInterface.h"

class UserInterface : public IConfigInterface
{
private:
	Camera& _camera;
	std::vector<UIBitmap*> _bitmaps;

	ID3D11InputLayout* _inputLayout;
	ID3D11VertexShader* _vertexShader;
	ID3D11PixelShader* _pixelShader;

	ID3D11Buffer* _matrixBuffer;
	XMFLOAT4X4 _worldMatrix;

public:
	UserInterface(Camera& camera);
	~UserInterface() override;

	void ReloadUI();

	void Cleanup();

	void Resize(float width, float height);

	void Initialise();

	void AddBitmapToUI(S_UIElementInfo const& inElementInfo);

	UIBitmap* GetUIElement(StringHash const elementNameHash) const;

	void Update(double delta);
	void Draw();

protected:
	void CreateConfig() override;
	void LoadConfig() override;
};
