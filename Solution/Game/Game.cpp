#include "stdafx.h"

#include <AudioInterface.h>
#include <Camera.h>
#include <ColoursForBG.h>
#include <CommonHelper.h>
#include <Cursor.h>
#include <DebugFont.h>
#include <Engine.h>
#include <FileWatcher.h>
#include "Game.h"
#include <GameStateMessage.h>
#include <GUIManager.h>
#include "InGameState.h"
#include <InputWrapper.h>
#include <ModelLoader.h>
#include <SystemMonitor.h>
#include <TimerManager.h>
#include <VTuneApi.h>
#include <Vector.h>
#include <XMLReader.h>

Game::Game()
	: myLockMouse(true)
#ifdef RELEASE_BUILD
	, myShowSystemInfo(false)
#else
	, myShowSystemInfo(true)
#endif
{
	Prism::Audio::AudioInterface::CreateInstance();
	myInputWrapper = new CU::InputWrapper();
	Prism::Engine::GetInstance()->SetShowDebugText(myShowSystemInfo);

	myGUIManager = new GUI::GUIManager(myInputWrapper);
	myCursor = new GUI::Cursor(myInputWrapper, Prism::Engine::GetInstance()->GetWindowSize());

	SetCursorPos(Prism::Engine::GetInstance()->GetWindowSize().x / 2, Prism::Engine::GetInstance()->GetWindowSize().y / 2);
}

Game::~Game()
{
	SAFE_DELETE(myGUIManager);
	SAFE_DELETE(myInputWrapper);
	SAFE_DELETE(myCursor);

	Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_MenuMusic", 0);
	Prism::Audio::AudioInterface::Destroy();
}

bool Game::Init(HWND& aHwnd)
{
	myWindowHandler = &aHwnd;
	myIsComplete = false;

	Prism::Engine::GetInstance()->SetClearColor({ MAGENTA });
	myInputWrapper->Init(aHwnd, GetModuleHandle(NULL), DISCL_NONEXCLUSIVE 
		| DISCL_FOREGROUND, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	myWindowSize.x = Prism::Engine::GetInstance()->GetWindowSize().x;
	myWindowSize.y = Prism::Engine::GetInstance()->GetWindowSize().y;

	GAME_LOG("Init Successful");
	return true;
}

bool Game::Destroy()
{
	return true;
}

bool Game::Update()
{
	myInputWrapper->Update();
	if (myInputWrapper->KeyDown(DIK_ESCAPE))
	{
		return false;
	}

	if (myLockMouse == true)
	{
		SetCursorPos(Prism::Engine::GetInstance()->GetWindowSize().x / 2, Prism::Engine::GetInstance()->GetWindowSize().y / 2);
	}

	myGUIManager->Update();
	myGUIManager->Render();

	myCursor->Update();
	myCursor->Render();

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
	myWindowSize.x = aWidth;
	myWindowSize.y = aHeight;
}

void Game::ReceiveMessage(const GameStateMessage& aMessage)
{
	switch (aMessage.GetGameState())
	{
	case eGameState::LOAD_GAME:
		myGame = new InGameState(myInputWrapper);
		myStateStack.PushSubGameState(myGame);
		myGame->SetLevel(aMessage.GetID(), aMessage.GetSecondID());
		break;
	case eGameState::LOAD_MENU:
		break;
	default:
		break;
	}
}
