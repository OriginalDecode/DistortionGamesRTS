#include "stdafx.h"

#include <BuildingComponent.h>
#include <Camera.h>
#include <CollisionComponent.h>
#include <ControllerComponent.h>
#include <EntityFactory.h>
#include <GUIManager.h>
#include <HealthComponent.h>
#include <Intersection.h>
#include <InputWrapper.h>
#include <OnClickMessage.h>
#include <PathFinder.h>
#include "PlayerDirector.h"
#include <PollingStation.h>
#include <Terrain.h>
#include <ToggleGUIMessage.h>
#include <ModelLoader.h>
#include <SpawnUnitMessage.h>

#include <FadeMessage.h>
#include <PostMaster.h>

PlayerDirector::PlayerDirector(const Prism::Terrain& aTerrain, Prism::Scene& aScene, GUI::Cursor* aCursor)
	: Director(eDirectorType::PLAYER, aTerrain)
	, myRenderGUI(true)
	, myCursor(aCursor)
	, myGUIManager(nullptr)
	, mySelectedUnits(56)
	, myTweakValueX(3.273f)
	, myTweakValueY(10.79f)
	, mySelectedAction(eSelectedAction::NONE)
{
	for (int i = 0; i < 64; ++i)
	{
		myUnits.Add(EntityFactory::CreateEntity(eOwnerType::PLAYER, eEntityType::DRAGON, Prism::eOctreeType::DYNAMIC,
			aScene, { 65, 0, 40 }, aTerrain));
		
	}
	
	myActiveUnits.Add(myUnits[0]);
	for (int i = 0; i < myActiveUnits.Size(); ++i)
	{
		myActiveUnits[i]->Spawn({ 65.f, 0.f, 25.f });
		PollingStation::GetInstance()->RegisterEntity(myActiveUnits[i]);
	}

	PostMaster::GetInstance()->Subscribe(eMessageType::TOGGLE_GUI, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::ON_CLICK, this);
}

PlayerDirector::~PlayerDirector()
{
	SAFE_DELETE(myGUIManager);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::TOGGLE_GUI, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
}

void PlayerDirector::InitGUI()
{
	Prism::ModelLoader::GetInstance()->Pause();
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_ingame.xml", this);
	Prism::ModelLoader::GetInstance()->UnPause();
}

void PlayerDirector::Update(float aDeltaTime, const Prism::Camera& aCamera)
{
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_G) == true)
	{
		PostMaster::GetInstance()->SendMessage(ToggleGUIMessage(!myRenderGUI, 1.f/3.f));
	}

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_H) == true)
	{
		for (int i = 0; i < mySelectedUnits.Size(); i++)
		{
			mySelectedUnits[i]->GetComponent<HealthComponent>()->TakeDamage(1);
		}
	}

	UpdateInputs();

	Director::Update(aDeltaTime);
	UpdateMouseInteraction(aCamera);
	myBuilding->Update(aDeltaTime);

	for (int i = mySelectedUnits.Size() - 1; i >= 0; --i) // remove dead units
	{
		if (mySelectedUnits[i]->GetAlive() == false)
		{
			mySelectedUnits[i]->SetSelect(false);
			mySelectedUnits.RemoveCyclicAtIndex(i);
		}
	}

	if (myRenderGUI == true)
	{
		myGUIManager->Update();
	}

	if (myLeftMouseClicked == true || myRightClicked == true || mySelectedAction == eSelectedAction::NONE)
	{
		mySelectedAction = eSelectedAction::NONE;
	}
}

void PlayerDirector::Render(const Prism::Camera& aCamera)
{
	if (myRenderGUI == true)
	{
		for (int i = 0; i < mySelectedUnits.Size(); i++)
		{
			mySelectedUnits[i]->GetComponent<HealthComponent>()->RenderHealthBar(aCamera);
		}

		myGUIManager->Render();
	}
}

void PlayerDirector::OnResize(int aWidth, int aHeight)
{
	myGUIManager->OnResize(aWidth, aHeight);
}

void PlayerDirector::SpawnUnit(Prism::Scene&)
{
	myBuilding->GetComponent<BuildingComponent>()->BuildUnit(eEntityType::DRAGON);
}

void PlayerDirector::ReceiveMessage(const SpawnUnitMessage& aMessage)
{
	if (aMessage.myOwnerType != static_cast<int>(eOwnerType::PLAYER)) return;
	if (myActiveUnits.Size() < 64)
	{
		for (int i = 0; i < myUnits.Size(); ++i)
		{
			if (myUnits[i]->GetType() == static_cast<eEntityType>(aMessage.myUnitType) && myUnits[i]->GetAlive() == false)
			{
				myUnits[i]->Spawn(myBuilding->GetOrientation().GetPos() + CU::Vector3f(0.f, 0.f, -15.f));
				myActiveUnits.Add(myUnits[i]);
				break;
			}
		}
		PollingStation::GetInstance()->RegisterEntity(myActiveUnits.GetLast());
	}
}

void PlayerDirector::ReceiveMessage(const ToggleGUIMessage& aMessage)
{
	myRenderGUI = aMessage.myShowGUI;
	PostMaster::GetInstance()->SendMessage(FadeMessage(aMessage.myFadeTime));
}

void PlayerDirector::ReceiveMessage(const OnClickMessage& aMessage)
{
	switch (aMessage.myEvent)
	{
	case eOnClickEvent::UNIT_ACTION_ATTACK:
		mySelectedAction = eSelectedAction::ATTACK;
		break;

	case eOnClickEvent::UNIT_ACTION_ATTACK_MOVE:
		mySelectedAction = eSelectedAction::MOVE_ATTACK;
		break;

	case eOnClickEvent::UNIT_ACTION_MOVE:
		mySelectedAction = eSelectedAction::MOVE;
		break;

	case eOnClickEvent::UNIT_ACTION_PATROL:
		mySelectedAction = eSelectedAction::PATROL;
		break;

	case eOnClickEvent::UNIT_ACTION_STAND_GROUND:
		mySelectedAction = eSelectedAction::STAND_GROUND;
		break;

	case eOnClickEvent::UNIT_ACTION_STOP:
		mySelectedAction = eSelectedAction::STOP;
		break;
	
	default:
		break;
	}
}

const BuildingComponent& PlayerDirector::GetBuildingComponent() const
{
	return *myBuilding->GetComponent<BuildingComponent>();
}

CU::Vector3<float> PlayerDirector::GetCameraMoveVector() const
{
	return myGUIManager->CalcCameraMovement();
}

void PlayerDirector::SelectUnit(Entity* anEntity)
{
	if (mySelectedUnits.Size() > 0 && mySelectedUnits[0]->GetType() != anEntity->GetType())
	{
		return;
	}

	for (int i = 0; i < mySelectedUnits.Size(); i++)
	{
		if (mySelectedUnits[i] == anEntity)
		{
			return;
		}
	}

	anEntity->SetSelect(true);
	mySelectedUnits.Add(anEntity);
}

CU::Vector3<float> PlayerDirector::CalcCursorWorldPosition(const Prism::Camera& aCamera)
{
	CU::Vector2<float> inputPos(CU::InputWrapper::GetInstance()->GetMousePosition());
	CU::Vector2<float> cursorPos;
	CU::Vector2<float> window = Prism::Engine::GetInstance()->GetWindowSize();
	//myTweakValueX = 3.284f;
	//myTweakValueY = 3.5f;
	float epsilon = 0.1f;
	float aspect = window.x / window.y;
	if (aspect <= 5.f / 4.f + epsilon)
	{
		myTweakValueX = 4.667f;
		myTweakValueY = 7.313f;
	}
	else if (aspect <= 16.f / 10.f + epsilon)
	{
		//tweakValue = 1.605f; // FOV90
		myTweakValueX = 3.646f;
		myTweakValueY = 9.315f;
	}

	float mult = window.y / window.x;
	cursorPos = inputPos;


	cursorPos.y = window.y - cursorPos.y;
	cursorPos.y /= window.y;
	cursorPos.y *= mult / myTweakValueY;
	cursorPos.y += (1.f - mult / myTweakValueY) / 2.f;


	cursorPos.x /= window.x;
	cursorPos.x *= mult / myTweakValueX;
	cursorPos.x += (1.f - mult / myTweakValueX) / 2.f;

	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_L) == true)
	{
		myTweakValueX += 0.001f;
		myTweakValueY += 0.001f;
		if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_LSHIFT) == true)
		{
			myTweakValueX += 0.1f;
			myTweakValueY += 0.1f;
		}
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_K) == true)
	{
		myTweakValueX -= 0.001f;
		myTweakValueY -= 0.001f;
		if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_LSHIFT) == true)
		{
			myTweakValueX -= 0.1f;
			myTweakValueY -= 0.1f;
		}
	}

	DEBUG_PRINT(myTweakValueX);
	DEBUG_PRINT(myTweakValueY);
	DEBUG_PRINT(cursorPos);
	

	CU::Vector3<float> worldPos(myTerrain.CalcIntersection(aCamera.GetOrientation().GetPos()
		, aCamera.RayCast(cursorPos)));

	DEBUG_PRINT(worldPos);

	//Debug:
	Prism::RenderBox(worldPos);
	Prism::RenderLine3D(worldPos, { 100.f, 100.f, 100.f });

	return worldPos;
}

void PlayerDirector::UpdateInputs()
{
	myShiftPressed = CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_LSHIFT)
		|| CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_RSHIFT);

	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_A) == true)
	{
		mySelectedAction = eSelectedAction::ATTACK;
	}

	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_S) == true)
	{
		mySelectedAction = eSelectedAction::STOP;
	}

	if (myRenderGUI == true) // no inworld clicking when mouse is over gui:
	{
		myLeftMouseClicked = CU::InputWrapper::GetInstance()->MouseDown(0) && !(myGUIManager->MouseOverGUI());
		myRightClicked = CU::InputWrapper::GetInstance()->MouseDown(1) && !(myGUIManager->MouseOverGUI());
	}
	else
	{
		myLeftMouseClicked = CU::InputWrapper::GetInstance()->MouseDown(0);
		myRightClicked = CU::InputWrapper::GetInstance()->MouseDown(1);
	}

	if (myLeftMouseClicked == true && myShiftPressed == false && 
		(mySelectedAction == eSelectedAction::NONE || mySelectedAction == eSelectedAction::STOP))
	{
		mySelectedUnits.RemoveAll();
	}
}

void PlayerDirector::UpdateMouseInteraction(const Prism::Camera& aCamera)
{
	CU::Vector3<float> targetPos = CalcCursorWorldPosition(aCamera);
	Entity* hoveredEnemy = PollingStation::GetInstance()->FindEntityAtPosition(targetPos, eOwnerType::ENEMY);
	if (hoveredEnemy != nullptr)
	{
		Prism::RenderBox(hoveredEnemy->GetOrientation().GetPos(), eColorDebug::RED);
	}

	bool hasSelected = false;
	bool hasHovered = false;
	CU::Intersection::LineSegment3D line(aCamera.GetOrientation().GetPos(), targetPos);
	for (int i = 0; i < myUnits.Size(); ++i)
	{
		SelectOrHoverEntity(myUnits[i], hasSelected, hasHovered, line);
		ControllerComponent* controller = myUnits[i]->GetComponent<ControllerComponent>();

		if (myUnits[i]->IsSelected())
		{
			if (mySelectedAction == eSelectedAction::ATTACK && myLeftMouseClicked == true)
			{
				controller->Attack(targetPos, !myShiftPressed);
			}
			else if (myRightClicked)
			{
				controller->MoveTo(targetPos, !myShiftPressed);
			}
			else if (mySelectedAction == eSelectedAction::STOP)
			{
				controller->Stop();
			}
		}
	}

	SelectOrHoverEntity(myBuilding, hasSelected, hasHovered, line);
}

void PlayerDirector::SelectOrHoverEntity(Entity* aEntity, bool &aSelected, bool &aHovered
	, const CU::Intersection::LineSegment3D& aMouseRay)
{
	if (myLeftMouseClicked == true && myShiftPressed == false && mySelectedAction != eSelectedAction::ATTACK)
	{
		aEntity->SetSelect(false);
	}

	aEntity->SetHovered(false);

	if (aEntity->GetComponent<CollisionComponent>()->Collide(aMouseRay))
	{
		if (myLeftMouseClicked == true && aSelected == false)
		{
			SelectUnit(aEntity);
			aSelected = true;
		}
		else if (aHovered == false)
		{
			aEntity->SetHovered(true);
			aHovered = true;
		}
	}
}