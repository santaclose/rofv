#include "Camera.h"

#define PHI_MAX 1.5708
#define PHI_MIN -1.5708

#define MIN_DISTANCE -20.0
#define MAX_DISTANCE -1.5

#define MOUSE_SCROLL_SENSITIVITY 0.6
#define MOUSE_SENSITIVITY 0.01

Camera::Camera()
{
	orbiting = false;
	theta = 0.0;
	phi = 0.523599; // 30degs
	distance = -6.5f;
	fov = 1.0472; // 60degs
	//scaleFactor = 1.0;
}

void Camera::StartOrbiting()
{
	orbiting = true;
}
void Camera::StopOrbiting()
{
	orbiting = false;
}

void Camera::OnMouseMoved(double dx, double dy)
{
	if (orbiting)
	{
		theta += dx * MOUSE_SENSITIVITY;
		phi += dy * MOUSE_SENSITIVITY;
		if (phi > PHI_MAX) phi = PHI_MAX;
		else if (phi < PHI_MIN) phi = PHI_MIN;
		//printf("theta: %lf\nphi: %lf\n", theta, phi);
	}
}

void Camera::OnMouseScroll(double value)
{
	distance += value * MOUSE_SCROLL_SENSITIVITY;
	if (distance < MIN_DISTANCE) distance = MIN_DISTANCE;
	else if (distance > MAX_DISTANCE) distance = MAX_DISTANCE;
	//printf("distance: %lf\n", distance);
}