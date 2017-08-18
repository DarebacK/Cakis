#include "stdafx.h"
#include "Camera.h"
#include "Game.h"
#include "GameObject.h"

DE::Camera::Camera(float nearPlaneDistance, float farPlaneDistance, float fieldOfView)
	:m_nearPlaneDistance{ nearPlaneDistance }, m_farPlaneDistance{ farPlaneDistance }, m_fieldOfView{ fieldOfView }
{
}
