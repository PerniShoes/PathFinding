#include "Game.h"

#include "utils.h"
#include "Time.h"
#include "Texture.h"
#include "OrientationManager.h"
#include "Pathfinding.h"

#include <iostream>
#include <cmath>
#include <print>


Game::Game(const Window& window)
	:BaseGame(window)
	,m_Window{ window }
	,m_RenderedFrames{0}
{
	Initialize();
}

Game::~Game()
{
	Cleanup();
}

void Game::Initialize()
{
	m_PF = std::make_unique<Pathfinding>(50);


	// Default
	m_FPSCounter = std::make_unique<Texture>("placeHolder","Resources/Fonts/consola.ttf",16,Color4f{1,1,1,1});
	m_TargetFPS = 60.0f;
	m_Time = std::make_unique<Time>(0.0f);
	m_AccumulatedTime = std::make_unique<Time>(0.0f);

	OrientationManager::UpdateCamera(GetViewPort());
	// Default
}

void Game::Cleanup()
{
	
}

void Game::Update(float elapsedSec)
{
	// Check keyboard state
	using namespace std;

	// Camera code
	float maxCameraSpeedDivider{1.5f};
	float minCameraSpeedDivider{0.5f};
	float cameraSpeedDivider{1.0f};

	Point2f mousePosDiff{m_LastMousePos-m_PreviousMousePos};
	m_PreviousMousePos = m_LastMousePos;

	if (m_AltHeld && m_LeftClickHeld)
	{
		m_CameraPos.left -= (mousePosDiff.x / cameraSpeedDivider);
		m_CameraPos.bottom -= (mousePosDiff.y/ cameraSpeedDivider);
		
	}

	OrientationManager::UpdateCamera(m_CameraPos);

	//

	int m_FPS = int((float)m_RenderedFrames / m_Time->GetTime());
	m_Time->Update(elapsedSec);
	m_AccumulatedTime->Update(elapsedSec);
	
	const float frameDuration{1.0f / m_TargetFPS};

	// Limits the frequency of code inside
	if (m_AccumulatedTime->GetTime() >= frameDuration)
	{
		m_PF->Update(elapsedSec);
		m_AccumulatedTime->Restart();
		m_FPSCounter = std::make_unique<Texture>("FPS: " + std::to_string(m_FPS),"Resources/Fonts/consola.ttf",16,Color4f{1,1,1,1}); // Has to be better
	}
	m_RenderedFrames += 1;
}


void Game::PushCameraMatrix() const
{

	glTranslatef(-m_CameraPos.left,-m_CameraPos.bottom,0);
	
}

void Game::Draw() const
{
	ClearBackground();
	glPushMatrix();
	{
		PushCameraMatrix();
		m_PF->Draw();

	}
	glPopMatrix();

	m_PF->DrawUI(GetViewPort());
	m_FPSCounter->Draw(Point2f{10.0f, GetViewPort().height - m_FPSCounter->GetHeight() - 4.0f},Rectf{});

}

void Game::ProcessKeyDownEvent(const SDL_KeyboardEvent& e)
{
	//std::cout << "KEYDOWN event: " << e.keysym.sym << std::endl;
	switch (e.keysym.sym)
	{
	case SDLK_p:
		m_PF->Pause();
		break;
	case SDLK_o:
		m_PF->Start();
		break;
	case SDLK_y:
		m_PF->FullRestart();
		break;
	case SDLK_u:
		m_PF->RestartAStar();
		break;
	case SDLK_z:
		m_PF->CycleStateUp();
		break;
	case SDLK_x:
		m_PF->CycleStateDown();
		break;
	case SDLK_COMMA:
		m_TargetFPS -= 5.0f;
		if (m_TargetFPS <= 0.0f)
		{
			m_TargetFPS = 5.0f;
		}
		break;
	case SDLK_PERIOD:
		m_TargetFPS += 5.0f;
		if (m_TargetFPS >= 120.0f)
		{
			m_TargetFPS = 120.0f;
		}
	case SDLK_LALT:
		m_AltHeld = true;
		break;
	case SDLK_ESCAPE:
		// Not sure about this, but works ¯\_(^^)_/¯
		SDL_Event e{SDL_QUIT};
		SDL_PushEvent(&e);
		break;
	}
	
}

void Game::ProcessKeyUpEvent(const SDL_KeyboardEvent& e)
{
	//std::cout << "KEYUP event: " << e.keysym.sym << std::endl;

	switch ( e.keysym.sym )
	{
	case SDLK_LALT:
		m_AltHeld = false;
		break;
	}

	
}

void Game::ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e)
{
	//std::cout << "MOUSEMOTION event: " << e.x << ", " << e.y << std::endl;
	m_LastMousePos = Point2f{ float(e.x),float(e.y) };

	m_PF->OnMouseMotion(e);
}

void Game::ProcessMouseDownEvent(const SDL_MouseButtonEvent& e)
{
	//std::cout << "MOUSEBUTTONDOWN event: ";
	switch (e.button)
	{
	case SDL_BUTTON_LEFT:
		m_LeftClickHeld = true;
		break;
	case SDL_BUTTON_RIGHT:
		break;
	case SDL_BUTTON_MIDDLE:
		break;
	}
	if (m_AltHeld == false)
	{
		m_PF->OnMouseDown(e);
	}
}

void Game::ProcessMouseUpEvent(const SDL_MouseButtonEvent& e)
{
	//std::cout << "MOUSEBUTTONUP event: ";
	
	switch ( e.button )
	{
	case SDL_BUTTON_LEFT:
		m_LeftClickHeld = false;
		break;
	case SDL_BUTTON_RIGHT:
		
		break;
	case SDL_BUTTON_MIDDLE:
		
		break;
	}
	m_PF->OnMouseUp(e);
}

void Game::ProcessMouseWheelEvent(const SDL_MouseWheelEvent& e)
{
	// Wheel up
	if (e.y > 0)
	{
		
	}
	// Wheel down
	else if (e.y < 0)
	{
		
	}	
}

void Game::ClearBackground() const
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}
