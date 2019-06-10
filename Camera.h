#pragma once
class Camera
{
private:
	bool orbiting;

public:
	float theta;
	float phi;
	float distance;
	double fov;
	//double scaleFactor;

	void StartOrbiting();
	void StopOrbiting();
	void OnMouseMoved(double dx, double dy);
	void OnMouseScroll(double value);
	Camera();
};

