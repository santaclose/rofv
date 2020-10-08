#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Shader.h"
#include "Entity.h"

struct CameraSpecs
{
	bool perspective = true;
	float orthographicScale = 5.0;
	float aspectRatio = 1.77777777778;
	float fieldOfView = 90.0;
	float nearClippingPlane = 0.1;
	float farClippingPlane = 1000.0;
};

class Camera : public Entity
{
private:
	glm::mat4 m_projectionMatrix;
	glm::mat4 m_viewMatrix;
	glm::mat4 m_cameraMatrix;


public:
	CameraSpecs m_specs;
	void ComputeMatrices();
	const glm::mat4& GetMatrix();
	const glm::mat4& GetViewMatrix();
	const glm::mat4& GetProjectionMatrix();
	const glm::vec3& GetPosition();

	Camera();
	Camera(const CameraSpecs& specs);
	void Bind();

	static Camera* boundCamera;
};

