#include "SceneElement.h"

SceneElement::SceneElement(StringHash elementName, Transform const& transform, Appearance const& appearance)
	: _transform(std::make_unique<Transform>(transform)), _appearance(std::make_unique<Appearance>(appearance)), _name(elementName), _castShadows(false), _affectedByLight(false)
{
}

SceneElement::~SceneElement()
{
}

void SceneElement::Cleanup()
{
	if (_transform != nullptr)
	{
		Transform* transformPtr = _transform.release();
		delete transformPtr;
		_transform = nullptr;
	}
	
	if (_appearance != nullptr)
	{
		_appearance->Cleanup();

		Appearance* appearancePtr = _appearance.release();
		delete appearancePtr;
		_appearance = nullptr;
	}
}

void SceneElement::AddChild(SceneElement * child)
{
	_children.push_back(child);
	child->GetTransform()->SetParent(_transform.get());
}

void SceneElement::Update(double deltaTime)
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

void SceneElement::Draw()
{
	if(_name == GetStringHash("Aircraft"))
	{
		auto temp = 1;
	}

	_appearance->Draw();
}
