#pragma once
class Camera
{
private:
	bool orbiting;

public:
	double theta;
	double phi;
	double distance;
	//double scaleFactor;

	void StartOrbiting();
	void StopOrbiting();
	void OnMouseMoved(double dx, double dy);
	void OnMouseScroll(double value);
	Camera();
};

