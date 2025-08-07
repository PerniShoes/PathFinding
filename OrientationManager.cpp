#include "OrientationManager.h"

Rectf OrientationManager::m_CameraRect {0,0,0,0};

Rectf OrientationManager::GetCamera() 
{
	return m_CameraRect;
}

void OrientationManager::UpdateCameraPos(const Point2f& pos)
{
	m_CameraRect.left = pos.x;
	m_CameraRect.bottom = pos.y;
}

void OrientationManager::UpdateCameraSize(const float& height,const float& width)
{
	m_CameraRect.height = height;
	m_CameraRect.width = width;
}

void OrientationManager::UpdateCamera(const Rectf& rect)
{
	m_CameraRect = rect;
}

Point2f OrientationManager::GetWorldLocation(const Point2f& mousePos)
{
	return Point2f{m_CameraRect.left + mousePos.x,m_CameraRect.bottom + mousePos.y};
}

Point2f OrientationManager::GetWorldLocation(const SDL_MouseButtonEvent& e)
{
	return GetWorldLocation(Point2f{float(e.x), float(e.y)});
}

