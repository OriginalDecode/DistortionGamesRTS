#include "stdafx.h"

#include <AudioInterface.h>
#include <Camera.h>
#include <ColoursForBG.h>
#include <CommonHelper.h>
#include "Console.h"
#include <Cursor.h>
#include <DebugFont.h>
#include <Engine.h>
#include <EntityId.h>
#include <FadeMessage.h>
#include <FileWatcher.h>
#include "Game.h"
#include <GameStateMessage.h>
#include "InGameState.h"
#include <InputWrapper.h>
#include "MainMenuState.h"
#include <ModelLoader.h>
#include <StreakDataContainer.h>
#include <ParticleDataContainer.h>
#include "PollingStation.h"
#include <PostMaster.h>
#include <ResizeMessage.h>
#include <SystemMonitor.h>
#include <TimerManager.h>
#include <VTuneApi.h>
#include <Vector.h>
#include <XMLReader.h>

#include "ScriptInterface.h"
#include <ScriptSystem.h>



Game::Game()
	: myLockMouse(true)
#ifdef RELEASE_BUILD
	, myShowSystemInfo(false)
#else
	, myShowSystemInfo(true)
#endif
{
	PostMaster::Create();
	Prism::Audio::AudioInterface::CreateInstance();

	Prism::Audio::AudioInterface::GetInstance()->Init("Data/Resource/Sound/Init.bnk");
	Prism::Audio::AudioInterface::GetInstance()->LoadBank("Data/Resource/Sound/RTSSoundBank.bnk");

	Prism::Engine::GetInstance()->SetShowDebugText(myShowSystemInfo);

	myCursor = new GUI::Cursor(Prism::Engine::GetInstance()->GetWindowSizeInt());

	SetCursorPos(Prism::Engine::GetInstance()->GetWindowSizeInt().x / 2, Prism::Engine::GetInstance()->GetWindowSizeInt().y / 2);
	myStateStack.SetCursor(myCursor);
}

Game::~Game()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::GAME_STATE, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::FADE, this);
	SAFE_DELETE(myCursor);
	Prism::Audio::AudioInterface::Destroy();
	Prism::StreakDataContainer::Destroy();
	Prism::ParticleDataContainer::Destroy();
	CU::InputWrapper::Destroy();
	CU::TimerManager::Destroy();
	Console::Destroy();
	PostMaster::Destroy();
	myStateStack.Clear();
	Prism::DebugDrawer::Destroy();

	LUA::ScriptSystem::Destroy();
}

bool Game::Init(HWND& aHwnd)
{
	myWindowHandler = &aHwnd;
	myIsComplete = false;

	PostMaster::GetInstance()->Subscribe(eMessageType::GAME_STATE, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::FADE, this);

	Prism::Engine::GetInstance()->SetClearColor({ MAGENTA });
	CU::InputWrapper::Create(aHwnd, GetModuleHandle(NULL), DISCL_NONEXCLUSIVE
		| DISCL_FOREGROUND, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);



	LUA::ScriptSystem::Create();
	LUA::ScriptSystem::GetInstance()->Init(ScriptInterface::RegisterFunctions);

	Console::GetInstance();
	//myMainMenu = new MainMenuState();
	//myStateStack.PushMainGameState(myMainMenu);

	PostMaster::GetInstance()->SendMessage(GameStateMessage(eGameState::LOAD_GAME, 1));
	GAME_LOG("Init Successful");
	return true;
}

bool Game::Destroy()
{
	return true;
}

bool Game::Update()
{
	CU::InputWrapper::GetInstance()->Update();
	CU::TimerManager::GetInstance()->Update();
	Prism::Audio::AudioInterface::GetInstance()->Update();
	float deltaTime = CU::TimerManager::GetInstance()->GetMasterTimer().GetTime().GetFrameTime();
	float fps = 1.f / deltaTime;
	DEBUG_PRINT(fps);
	float frameTime = deltaTime * 1000;
	DEBUG_PRINT(frameTime);

	if (deltaTime > 1.0f / 10.0f)
	{
		deltaTime = 1.0f / 10.0f;
	}

	if (myLockMouse == true)
	{
		//SetCursorPos(Prism::Engine::GetInstance()->GetWindowSize().x / 2, Prism::Engine::GetInstance()->GetWindowSize().y / 2);
		RECT windowRect;
		GetWindowRect(*myWindowHandler, &windowRect);
		if (Prism::Engine::GetInstance()->IsFullscreen() == false)
		{
			windowRect.left += 10;
			windowRect.top += 35;
			windowRect.right -= 10;
			windowRect.bottom -= 10;
		}
		ClipCursor(&windowRect);
	}

	if (myStateStack.UpdateCurrentState(deltaTime) == false)
	{
		return false;
	}

	myStateStack.RenderCurrentState();

	CU::TimerManager::GetInstance()->CapFrameRate(100.f);
	myCursor->Update();
	myCursor->Render();

	static bool run = true;
	if (run == false)
	{
		//LUA::ScriptSystem::GetInstance()->RunLuaFromString("Print(\"apa\")");
	}
	run = false;

	LUA::ScriptSystem::GetInstance()->Update();
	//Console::GetInstance()->Update();

	return true;
}

void Game::Pause()
{
	myLockMouse = false;
	ShowCursor(true);
}

void Game::UnPause()
{
	myLockMouse = true;
	ShowCursor(false);
}

void Game::OnResize(int aWidth, int aHeight)
{
	myStateStack.OnResize(aWidth, aHeight);
	myCursor->OnResize(aWidth, aHeight);
	PostMaster::GetInstance()->SendMessage(ResizeMessage(aWidth, aHeight));
}

void Game::ReceiveMessage(const GameStateMessage& aMessage)
{
	switch (aMessage.myGameState)
	{
	case eGameState::LOAD_GAME:
		Prism::MemoryTracker::GetInstance()->SetRunTime(false);
		myGame = new InGameState();
		myStateStack.PushMainGameState(myGame);
		break;
	case eGameState::LOAD_MENU:
		break;
	default:
		break;
	}
}

void Game::ReceiveMessage(const FadeMessage& aMessage)
{
	Prism::Engine::GetInstance()->StartFade(aMessage.myDuration);
}
