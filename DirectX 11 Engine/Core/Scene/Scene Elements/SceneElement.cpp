#include "SceneElement.h"

SceneElement::SceneElement(TStringHash elementName, Transform const& transform, Appearance const& appearance)
	: m_TransformPtr(std::make_unique<Transform>(transform))
	, m_AppearancePtr(std::make_unique<Appearance>(appearance))
	, m_Name(elementName)
	, m_bCastShadows(false), m_bAffectedByLight(false)
{
}

SceneElement::~SceneElement()
{
}

void SceneElement::Cleanup()
{
	if (m_TransformPtr != nullptr)
	{
		Transform* transformPtr = m_TransformPtr.release();
		delete transformPtr;
		m_TransformPtr = nullptr;
	}
	
	if (m_AppearancePtr != nullptr)
	{
		m_AppearancePtr->Cleanup();

		Appearance* appearancePtr = m_AppearancePtr.release();
		delete appearancePtr;
		m_AppearancePtr = nullptr;
	}
}

void SceneElement::AddChild(SceneElement * child)
{
	m_Children.push_back(child);
	child->GetTransform()->SetParent(m_TransformPtr.get());
}

void SceneElement::Update(double deltaTime)
{
	m_TransformPtr->Update(deltaTime);

	if (!m_Children.empty())
	{
		for (SceneElement* child : m_Children)
		{	
			child->Update(deltaTime);
		}
	}
}

void SceneElement::Draw()
{
	if(m_Name == GetStringHash("Aircraft"))
	{
		auto temp = 1;
	}

	m_AppearancePtr->Draw();
}
