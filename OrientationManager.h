#pragma once
#include <SDL.h>
#include "utils.h"

class OrientationManager final
{
public:
    OrientationManager() = delete;
   
    static void UpdateCameraPos(const Point2f& pos);
    static void UpdateCameraSize(const float& height, const float& width);
    static void UpdateCamera(const Rectf& rect);
    static Rectf GetCamera();
    
    static Point2f GetWorldLocation(const Point2f& mousePos);
    static Point2f GetWorldLocation(const SDL_MouseButtonEvent& e);

private:

    static Rectf m_CameraRect;

};

