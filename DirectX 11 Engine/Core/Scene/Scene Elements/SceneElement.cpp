#include "SceneElement.h"

SceneElement::SceneElement(std::string elementName, Transform* transform, Appearance* appearance)
	: _transform(transform), _appearance(appearance), _name(elementName), _castShadows(false), _affectedByLight(false)
{
}

SceneElement::~SceneElement()
{
}

void SceneElement::Cleanup()
{
	delete _transform;
	delete _appearance;
}

void SceneElement::AddChild(SceneElement * child)
{
	_children.push_back(child);
	child->GetTransform()->SetParent(_transform);
}

void SceneElement::Update(float deltaTime)
{
	_transform->Update(deltaTime);

	if (!_children.empty())
	{
		for (SceneElement* child : _children)
		{	
			child->Update(deltaTime);
		}
	}
}

void SceneElement::Draw(ID3D11DeviceContext* deviceContext)
{
	if(_name == "Aircraft")
	{
		auto temp = 1;
	}

	_appearance->Draw(deviceContext);
}
