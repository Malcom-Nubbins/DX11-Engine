#include "MainScene.h"
#include "../ApplicationNew.h"
#include "../DX11Engine.h"
#include "../Handlers/System Handlers/InputHandler.h"

MainScene::MainScene(Player& player)
	//: Scene(player)

{
	_planeVertexBuffer = nullptr;
	_planeIndexBuffer = nullptr;

	

	_diamondSquareTerrain = nullptr;
	
}

MainScene::~MainScene()
{
}

//void MainScene::Cleanup()
//{
//	_diamondSquareTerrain->Cleanup();
//	_diamondSquareTerrain = nullptr;
//
//	_planeVertexBuffer->Release();
//	_planeIndexBuffer->Release();
//
//	for (auto const sceneElement : _sceneElements)
//	{
//		if (sceneElement)
//		{
//			sceneElement->Cleanup();
//		}
//	}
//
//	_sceneElements.clear();
//}

//void MainScene::InitialiseScene(float windowWidth, float windowHeight)
//{
//	
//
//	ObjectMesh planeMesh;
//	planeMesh.vertexBuffer = _planeVertexBuffer;
//	planeMesh.indexBuffer = _planeIndexBuffer;
//	planeMesh.numberOfIndices = 6;
//	planeMesh.vertexBufferOffset = 0;
//	planeMesh.vertexBufferStride = sizeof(SimpleVertex);
//
//	//ObjectMesh aircraftMesh = OBJLoader::Load(L"Core/Resources/Objects/Hercules.obj", _d3dClass->GetDevice(), false);
//	//ObjectMesh sphere = OBJLoader::Load(L"Core/Resources/Objects/spherex.obj", _d3dClass->GetDevice(), false);
//	//ObjectMesh plant0 = OBJLoader::Load(L"Core/Resources/Objects/plant0.obj", _d3dClass->GetDevice());
//	//ObjectMesh plant1 = OBJLoader::Load(L"Core/Resources/Objects/plant1.obj", _d3dClass->GetDevice());
//
//	auto device = ApplicationNew::Get().GetDevice();
//
//	NewObjectMesh aircraftMesh, sphere, plant0, plant1;
//	ModelLoader::LoadModel(device.Get(), L"Core/Resources/Objects/Hercules.obj", aircraftMesh, false);
//	ModelLoader::LoadModel(device.Get(), L"Core/Resources/Objects/spherex.obj", sphere, false);
//	ModelLoader::LoadModel(device.Get(), L"Core/Resources/Objects/plant0.obj", plant0, false);
//	ModelLoader::LoadModel(device.Get(), L"Core/Resources/Objects/plant1.obj", plant1, false);
//
//	_diamondSquareTerrain = new DiamondSquareTerrain();
//	_diamondSquareTerrain->SetTerrainValues(256, 256, 512);
//	_diamondSquareTerrain->GenerateTerrain();
//
//	m_Player->SetPlayerPosition(XMFLOAT3(0.0f, _diamondSquareTerrain->GetHeight(0.0f, -2.0f) + 2.0f, -2.0f));
//
//	ObjectMesh diamondSquareMesh;
//	diamondSquareMesh.vertexBuffer = _diamondSquareTerrain->GetVertexBuffer();
//	diamondSquareMesh.indexBuffer = _diamondSquareTerrain->GetIndexBuffer();
//	diamondSquareMesh.numberOfIndices = static_cast<int>(_diamondSquareTerrain->GetMeshData()->Indices.size());
//	diamondSquareMesh.vertexBufferOffset = 0;
//	diamondSquareMesh.vertexBufferStride = sizeof(SimpleVertex);
//
//	// Transforms
//	auto groundTransform = std::make_shared<Transform>();
//	groundTransform->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
//	groundTransform->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
//	groundTransform->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
//
//	auto underworldTransform = std::make_shared<Transform>();
//	underworldTransform->SetPosition(XMFLOAT3(0.0f, -15.0f, 0.0f));
//	underworldTransform->SetScale(XMFLOAT3(256.0f, 1.0f, 256.0f));
//	underworldTransform->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
//
//	auto aircraftTransform = std::make_shared<Transform>();
//	aircraftTransform->SetPosition(XMFLOAT3(0.0f, _diamondSquareTerrain->GetHeight(0, 0) + 8.0f, 0));
//	aircraftTransform->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
//	aircraftTransform->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
//
//	auto sunSphereTransform = std::make_shared<Transform>();
//	sunSphereTransform->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
//	sunSphereTransform->SetScale(XMFLOAT3(4.0f, 4.0f, 4.0f));
//	sunSphereTransform->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
//
//
//	// Appearances
//	std::shared_ptr<Appearance> groundAppearance = std::make_shared<Appearance>(diamondSquareMesh, concrete);
//	groundAppearance->SetColourTexture(texHandler->GetTextureByName("GrassyStoneColour"));
//	groundAppearance->SetNormalMap(texHandler->GetTextureByName("GrassyStoneNormal"));
//	groundAppearance->SetDisplacementMap(texHandler->GetTextureByName("GrassyStoneDisplacement"));
//	//groundAppearance->SetSpecularMap(_systemHandlers->GetTextureHandler()->GetGroundSpecularMap());
//
//	std::shared_ptr<Appearance> underworldAppearance = std::make_shared<Appearance>(planeMesh, shiny);
//	underworldAppearance->SetColourTexture(texHandler->GetTextureByName("StoneColour"));
//	underworldAppearance->SetNormalMap(texHandler->GetTextureByName("StoneNormal"));
//	underworldAppearance->SetDisplacementMap(texHandler->GetTextureByName("StoneDisplacement"));
//
//	std::shared_ptr<Appearance> sphereAppearance = std::make_shared<Appearance>(sphere, charcoal);
//	//sphereAppearance->SetColourTexture(_systemHandlers->GetTextureHandler()->GetStoneTexture());
//	//sphereAppearance->SetNormalMap(_systemHandlers->GetTextureHandler()->GetStoneNormalMap());
//	//sphereAppearance->SetDisplacementMap(_systemHandlers->GetTextureHandler()->GetStoneDisplacementMap());
//
//	std::shared_ptr<Appearance> aircraftAppearance = std::make_shared<Appearance>(aircraftMesh, aircraftMat);
//	aircraftAppearance->SetColourTexture(texHandler->GetTextureByName("AircraftColour"));
//	aircraftAppearance->SetNormalMap(texHandler->GetTextureByName("AircraftNormal"));
//	//aircraftAppearance->SetSpecularMap(_systemHandlers->GetTextureHandler()->GetAircraftSpecularMap());
//	//aircraftAppearance->SetDisplacementMap(_systemHandlers->GetTextureHandler()->GetAircraftDisplacementMap());
//
//	std::shared_ptr<Appearance> sunSphereAppearance = std::make_shared<Appearance>(sphere, shiny);
//
//	// Scene Elements
//	{
//		SceneElement* element = new SceneElement(GetStringHash("Ground Plane"), *groundTransform, *groundAppearance);
//		element->SetCastShadows(true);
//		element->SetAffectedByLight(true);
//		_diamondSquareTerrain->SetTerrainFinishedBuilding(true);
//		_sceneElements.push_back(element);
//
//		element = new SceneElement(GetStringHash("Underworld"), *underworldTransform, *underworldAppearance);
//		element->SetCastShadows(true);
//		element->SetAffectedByLight(true);
//		_sceneElements.push_back(element);
//
//		srand(static_cast<unsigned int>(time(NULL)));
//		for (int i = 0; i < 90; ++i)
//		{
//			auto sphereTransform = std::make_shared<Transform>();
//			sphereTransform->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
//			sphereTransform->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
//			sphereTransform->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
//
//			StringHash sphereElementName = GetStringHash(FormatCString("Spheres %d", i));
//			element = new SceneElement(sphereElementName, *sphereTransform, *sphereAppearance);
//			const float randomX = MathsHandler::RandomFloat(-128, 128);
//			const float randomZ = MathsHandler::RandomFloat(-128, 128);
//
//			element->GetTransform()->SetPosition(XMFLOAT3(randomX, _diamondSquareTerrain->GetHeight(randomX, randomZ) + 2.0f, randomZ));
//			element->SetCastShadows(true);
//			element->SetAffectedByLight(true);
//
//			_sceneElements.push_back(element);
//		}
//
//		element = new SceneElement(GetStringHash("Aircraft"), *aircraftTransform, *aircraftAppearance);
//		element->SetCastShadows(true);
//		element->SetAffectedByLight(true);
//		_sceneElements.push_back(element);
//
//		PointLight light;
//		light.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
//		light.diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
//		light.specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
//		light.attenuation = XMFLOAT3(0.0f, 0.1f, 0.0f);
//		light.range = 25.0f;
//		light.position = m_Player->GetPlayerPosition();
//		//_pointLights.push_back(light);
//
//
//		auto plantAppearance = std::make_shared<Appearance>(plant0, matte);
//		auto plant2Appearance = std::make_shared<Appearance>(plant1, matte);
//
//		for (int i = 0; i < 90; ++i)
//		{
//			auto plantTransform = std::make_shared<Transform>();
//			plantTransform->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
//			plantTransform->SetScale(XMFLOAT3(5.0f, 5.0f, 5.0f));
//			plantTransform->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
//
//			float randomX = MathsHandler::RandomFloat(-90, 90);
//			float randomZ = MathsHandler::RandomFloat(-90, 90);
//
//			StringHash plantName = GetStringHash(FormatCString("Plant %d", i));
//			element = new SceneElement(plantName, *plantTransform, *plantAppearance);
//			element->GetTransform()->SetPosition(XMFLOAT3(randomX, _diamondSquareTerrain->GetHeight(randomX, randomZ), randomZ));
//			element->GetTransform()->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
//			element->GetTransform()->SetRotation(XMFLOAT3(0.0f, XMConvertToRadians(static_cast<float>(rand())), 0.0f));
//			element->SetCastShadows(true);
//			element->SetAffectedByLight(true);
//
//			/*light.Position = element->GetPosition();
//			_pointLights.push_back(light);*/
//
//			_sceneElements.push_back(element);
//
//			randomX = MathsHandler::RandomFloat(-90, 90);
//			randomZ = MathsHandler::RandomFloat(-90, 90);
//
//			element = new SceneElement(plantName, *plantTransform, *plant2Appearance);
//			element->GetTransform()->SetPosition(XMFLOAT3(randomX, _diamondSquareTerrain->GetHeight(randomX, randomZ), randomZ));
//			element->GetTransform()->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
//			element->GetTransform()->SetRotation(XMFLOAT3(0.0f, XMConvertToRadians(static_cast<float>(rand())), 0.0f));
//			element->SetCastShadows(true);
//			element->SetAffectedByLight(true);
//
//			/*light.Position = element->GetPosition();
//			_pointLights.push_back(light);*/
//
//			_sceneElements.push_back(element);
//		}
//
//		element = new SceneElement(GetStringHash("Light Source Sphere"), *sunSphereTransform, *sunSphereAppearance);
//		element->SetCastShadows(false);
//		element->SetAffectedByLight(false);
//		_sceneElements.push_back(element);
//	}
//}
//
//void MainScene::Update(UpdateEvent& e)
//{	
//	Scene::Update(e);
//
//	//_pointLights.at(0).Position = _camera->GetPosition();
//
//	/*std::wostringstream out;
//	out.precision(6);
//	out << L"Day: " << _currentDay << L" - Season: " << _seasonNames.find(_currentSeason)->second.c_str() << L" Time speed: " << _lightRotationAmount;*/
//	//_systemHandlers.GetWindowClass()->SetWindowCaption(out.str().c_str());
//}
//
//void MainScene::Draw()
//{	
//	
//	
//
//	
//	
//	
//}
