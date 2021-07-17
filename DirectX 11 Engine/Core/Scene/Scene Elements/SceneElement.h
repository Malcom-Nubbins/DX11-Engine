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
	std::unique_ptr<Transform> _transform;
	std::unique_ptr<Appearance> _appearance;

	std::string _name;

	bool _castShadows;
	bool _affectedByLight;

	std::vector<SceneElement*> _children;

public:
	SceneElement(std::string elementName, Transform const& transform, Appearance const& appearance);
	~SceneElement();

	void Cleanup();

	void Update(double deltaTime);
	void Draw();

public:
	
	Transform * GetTransform() const { return _transform.get(); }
	Appearance * GetAppearance() const { return _appearance.get(); }

	void SetCastShadows(bool castShadows) { _castShadows = castShadows; }
	bool CanCastShadows() const { return _castShadows; }

	void SetAffectedByLight(bool affected) { _affectedByLight = affected; }
	bool IsAffectedByLight() const { return _affectedByLight; }

	std::string GetElementName() const { return _name; }

	void AddChild(SceneElement* child);
	std::vector<SceneElement*> GetChildren() { return _children; }
};

