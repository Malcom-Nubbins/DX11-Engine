#pragma once
#include "../../Scene/Camera.h"
#include "UIBitmap.h"
#include "../Core/Loaders/IConfigInterface.h"

class UserInterface : public IConfigInterface
{
private:
	Camera* m_CameraPtr;
	std::vector<UIBitmap*> m_Bitmaps;

	ID3D11InputLayout* m_InputLayout;
	ID3D11VertexShader* m_UIVertexShader;
	ID3D11PixelShader* m_UIPixelShader;

	ID3D11Buffer* m_MatrixBufferPtr;
	XMFLOAT4X4 m_WorldMatrix;

public:
	UserInterface(Camera* camera);
	~UserInterface() override;

	void ReloadUI();

	void Cleanup();

	void Resize(float width, float height);

	void Initialise();

	void AddBitmapToUI(UIElementInfo const& inElementInfo);

	UIBitmap* GetUIElement(TStringHash const elementNameHash) const;

	void Update(double delta);
	void Draw();

protected:
	void CreateConfig() override;
	void LoadConfig() override;
};
