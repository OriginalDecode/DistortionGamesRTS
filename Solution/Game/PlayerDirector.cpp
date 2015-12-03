#include "stdafx.h"

#include <Camera.h>
#include <CollisionComponent.h>
#include <ControllerComponent.h>
#include <EntityFactory.h>
#include <GUIManager.h>
#include <Intersection.h>
#include <InputWrapper.h>
#include "PlayerDirector.h"
#include "PollingStation.h"
#include <Terrain.h>
#include <ModelLoader.h>


PlayerDirector::PlayerDirector(const Prism::Terrain& aTerrain, Prism::Scene& aScene, GUI::Cursor* aCursor)
	: Director(eDirectorType::PLAYER, aTerrain)
	, myRenderGUI(true)
	, myCursor(aCursor)
	, myGUIManager(nullptr)
{
	for (int i = 0; i < 1; ++i)
	{
		myUnits.Add(EntityFactory::CreateEntity(eOwnerType::PLAYER, eEntityType::DRAGON, Prism::eOctreeType::DYNAMIC,
			aScene, { 20.f + i, 0.f, 20.f }, aTerrain));
	}
	Prism::ModelLoader::GetInstance()->Pause();
	myGUIManager = new GUI::GUIManager(aCursor, "Data/Resource/GUI/GUI_ingame.xml", myUnits);
	Prism::ModelLoader::GetInstance()->UnPause();

	for (int i = 0; i < myUnits.Size(); ++i)
	{
		PollingStation::GetInstance()->RegisterEntity(myUnits[i]);
	}
}

PlayerDirector::~PlayerDirector()
{
	SAFE_DELETE(myGUIManager);
}

void PlayerDirector::Update(float aDeltaTime, const Prism::Camera& aCamera)
{
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_G) == true)
	{
		myRenderGUI = !myRenderGUI;
	}

	Director::Update(aDeltaTime);
	UpdateMouseInteraction(aCamera);

	if (myRenderGUI == true)
	{
		myGUIManager->Update();
	}
}

void PlayerDirector::Render()
{
	if (myRenderGUI == true)
	{
		myGUIManager->Render();
	}
}

void PlayerDirector::OnResize(int aWidth, int aHeight)
{
	myGUIManager->OnResize(aWidth, aHeight);
}

void PlayerDirector::SpawnUnit(Prism::Scene& aScene)
{
	if (myUnits.Size() < 64)
	{
		myUnits.Add(EntityFactory::CreateEntity(eOwnerType::PLAYER, eEntityType::DRAGON, Prism::eOctreeType::DYNAMIC,
			aScene, { 20.f, 0.f, 20.f }, myTerrain));
		PollingStation::GetInstance()->RegisterEntity(myUnits.GetLast());
	}
}

CU::Vector3<float> PlayerDirector::CalcCursorWorldPosition(const Prism::Camera& aCamera)
{
	CU::Vector2<float> inputPos(CU::InputWrapper::GetInstance()->GetMousePosition());
	CU::Vector2<float> cursorPos;
	CU::Vector2<float> window = Prism::Engine::GetInstance()->GetWindowSize();

	float tweakValue = 1.85f; // for 16:9
	float epsilon = 0.1f;
	float aspect = window.x / window.y;
	if (aspect <= 5.f / 4.f + epsilon)
	{
		tweakValue = 1.255f;
	}
	else if (aspect <= 16.f / 10.f + epsilon)
	{
		tweakValue = 1.605f;
	}

	float mult = window.y / window.x;
	cursorPos = inputPos;

	cursorPos.y = window.y - cursorPos.y;
	cursorPos.y /= window.y;
	cursorPos.y *= mult / tweakValue;
	cursorPos.y += (1.f - mult / tweakValue) / 2.f;

	cursorPos.x /= window.x;

	Prism::Engine::GetInstance()->PrintText(cursorPos.x, { 50.f, 50.f }, Prism::eTextType::DEBUG_TEXT);
	Prism::Engine::GetInstance()->PrintText(cursorPos.y, { 280.f, 50.f }, Prism::eTextType::DEBUG_TEXT);
	Prism::Engine::GetInstance()->PrintText(tweakValue, { 480.f, 50.f }, Prism::eTextType::DEBUG_TEXT);

	CU::Vector3<float> worldPos(myTerrain.CalcIntersection(aCamera.GetOrientation().GetPos()
		, aCamera.RayCast(cursorPos)));

	//Debug:
	Prism::RenderBox(worldPos);
	Prism::RenderLine3D(worldPos, { 100.f, 100.f, 100.f });

	return worldPos;
}

void PlayerDirector::UpdateMouseInteraction(const Prism::Camera& aCamera)
{
	CU::Vector3<float> targetPos = CalcCursorWorldPosition(aCamera);
	CU::Intersection::LineSegment3D line(aCamera.GetOrientation().GetPos(), targetPos);

	bool leftClicked;
	if (myRenderGUI == true) // no inworld clicking when mouse is over gui:
	{
		leftClicked = CU::InputWrapper::GetInstance()->MouseDown(0) && !(myGUIManager->MouseOverGUI()); 
	}
	else
	{
		leftClicked = CU::InputWrapper::GetInstance()->MouseDown(0);
	}

	bool hasSelected = false;
	bool hasHovered = false;
	for (int i = 0; i < myUnits.Size(); ++i)
	{
		bool mouseOnUnit = myUnits[i]->GetComponent<CollisionComponent>()->Collide(line);

		if (leftClicked == true)
		{
			myUnits[i]->SetSelect(false);
		}

		myUnits[i]->SetHovered(false);

		if (mouseOnUnit == true)
		{
			if (leftClicked == true && hasSelected == false)
			{
				myUnits[i]->SetSelect(true);
				hasSelected = true;
			}
			else if (hasHovered == false)
			{
				myUnits[i]->SetHovered(true);
				hasHovered = true;
			}
		}
	}

	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_LSHIFT) && CU::InputWrapper::GetInstance()->MouseDown(1))
	{
		CU::Vector3<float> newPos(CalcCursorWorldPosition(aCamera));
		for (int i = 0; i < myUnits.Size(); ++i)
		{
			if (myUnits[i]->IsSelected())
			{
				myUnits[i]->GetComponent<ControllerComponent>()->MoveTo(newPos, false);
			}
		}
	}
	else if (CU::InputWrapper::GetInstance()->MouseDown(1))
	{
		CU::Vector3<float> newPos(CalcCursorWorldPosition(aCamera));
		for (int i = 0; i < myUnits.Size(); ++i)
		{
			if (myUnits[i]->IsSelected())
			{
				myUnits[i]->GetComponent<ControllerComponent>()->MoveTo(newPos, true);
			}
		}
	}
}