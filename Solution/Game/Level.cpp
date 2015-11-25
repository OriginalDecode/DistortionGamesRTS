#include "stdafx.h"

#include <DirectionalLight.h>
#include <Engine.h>
#include <EngineEnums.h>
#include "Level.h"
#include <Scene.h>
#include <Terrain.h>


#include <Entity.h>
#include <GraphicsComponent.h>

Level::Level(const Prism::Camera& aCamera)
{
	myTerrain = new Prism::Terrain("Data/Resource/Texture/Terrain/heightmap.tga"
		, "Data/Resource/Texture/Terrain/T_rock.dds", { 256.f, 256.f }, 25.5f, CU::Matrix44<float>());


	myUnit = new Entity(eEntityType::PLAYER, *myScene, Prism::eOctreeType::DYNAMIC, "TestUnit");
	myUnit->AddComponent(new GraphicsComponent(*myUnit, "Data/Resource/Model/BoxBro/boxBro_idle_anim.fbx"
		, "Data/Resource/Shader/S_effect_no_texture.fx"));

	myScene = new Prism::Scene(aCamera, *myTerrain);

	myLight = new Prism::DirectionalLight();
	myLight->SetColor({ 0.5f, 0.5f, 0.9f, 100.f });
	myLight->SetDir(CU::Vector3<float>(0, 1, 0) * CU::Matrix44<float>::CreateRotateAroundZ(-3.14f / 2.f));
	myScene->AddLight(myLight);
}

Level::~Level()
{
	SAFE_DELETE(myTerrain);
	SAFE_DELETE(myUnit);
	SAFE_DELETE(myScene);
	SAFE_DELETE(myLight);
}

bool Level::LogicUpdate(float aDeltaTime)
{
	return true;
}

void Level::Render()
{
	Prism::Engine::GetInstance()->SetClearColor({ 1.0f, 0.f, 0.f, 1.f });

	myScene->Render();
}

void Level::OnResize(int aWidth, int aHeigth)
{
}
