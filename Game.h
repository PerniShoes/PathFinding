#pragma once
#include "BaseGame.h"
#include <vector>
#include <memory>

class Time;
class Texture;
class Pathfinding;

class Game : public BaseGame
{
public:
	explicit Game( const Window& window );
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game( Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	// http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-override
	~Game();

	void Update( float elapsedSec ) override;
	void Draw( ) const override;

	// Event handling
	void ProcessKeyDownEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessKeyUpEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e ) override;
	void ProcessMouseDownEvent( const SDL_MouseButtonEvent& e ) override;
	void ProcessMouseUpEvent( const SDL_MouseButtonEvent& e ) override;
	void ProcessMouseWheelEvent(const SDL_MouseWheelEvent& e) override;
	

	
private:
	// Non-default Game members:
	std::unique_ptr<Pathfinding> m_PF;









	// Default Game members:
	const Window m_Window;

	Point2f m_LastMousePos;
	Point2f m_PreviousMousePos;
	std::unique_ptr<Time> m_Time;
	std::unique_ptr<Time> m_AccumulatedTime;

	int m_RenderedFrames;
	std::unique_ptr<Texture> m_FPSCounter;
	float m_TargetFPS;


	bool m_AltHeld;
	bool m_LeftClickHeld;
	// Default 

	
	// Camera testing:
	bool m_CenterToGrid;
	void PushCameraMatrix() const;
	Rectf m_CameraPos;

	//






	// FUNCTIONS
	void Initialize();
	void Cleanup( );
	void ClearBackground( ) const;
};