#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
private:
	bool orbiting;
	float theta;
	float phi;
	float distance;
	double fov;

public:
	glm::mat4 modelMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	//double scaleFactor;

	void StartOrbiting();
	void StopOrbiting();
	void OnMouseMoved(double dx, double dy);
	void OnMouseScroll(double value);
	void UpdateTransformationMatrices(double aspectRatio);
	Camera();
};

