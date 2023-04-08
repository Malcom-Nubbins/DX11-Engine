#pragma once
#include "../../Globals/stdafx.h"
#include "../../Globals/Structs.h"
#include "Element Components/Transform.h"
#include "Element Components/Appearance.h"
#include <string>
#include <vector>

using namespace DirectX;

class SceneElement
{
private:
	std::unique_ptr<Transform> m_TransformPtr;
	std::unique_ptr<Appearance> m_AppearancePtr;

	TStringHash m_Name;

	bool m_bCastShadows;
	bool m_bAffectedByLight;

	std::vector<SceneElement*> m_Children;

public:
	SceneElement(TStringHash elementName, Transform const& transform, Appearance const& appearance);
	~SceneElement();

	void Cleanup();

	void Update(double deltaTime);
	void Draw();

public:
	
	Transform* GetTransform() const { return m_TransformPtr.get(); }
	Appearance* GetAppearance() const { return m_AppearancePtr.get(); }

	void SetCastShadows(bool castShadows) { m_bCastShadows = castShadows; }
	bool CanCastShadows() const { return m_bCastShadows; }

	void SetAffectedByLight(bool affected) { m_bAffectedByLight = affected; }
	bool IsAffectedByLight() const { return m_bAffectedByLight; }

	TStringHash GetElementName() const { return m_Name; }

	void AddChild(SceneElement* child);
	std::vector<SceneElement*> GetChildren() { return m_Children; }
};

