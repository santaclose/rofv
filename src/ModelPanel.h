#pragma once
#include "DisplayPanelMSAA.h"
#include "Shader.h"
#include "Camera.h"

class ModelPanel : public DisplayPanelMSAA
{
    Entity m_gimbal;
    glm::vec3 m_gimbalRot = glm::vec3(0.0, 3.14159265, 0.0);
    float m_distance = 4.0;
	Shader m_shader;
	Camera* m_camera;

	bool m_modelGenerationPending = true;
	glm::vec3 m_lightDir = { 0.0, 0.0, -1.0 };
	bool m_noLighting = false;
	bool m_useTexture = false;

    void UpdateCamera();

public:
	ModelPanel(const std::string& name, const glm::vec3& clearColor, uint32_t msaaCount);
	~ModelPanel();
	void OnResize();
	void HandleInput(const ImGuiIO& io, const glm::vec2& relativeMousePos);
	void Draw();

	bool* GetModelGenerationPendingBool();
	void SetSmoothEnabled(bool value);
	void SetLightingEnabled(bool value);
	unsigned int GetLastVertexCount();
	void SaveModel(bool saveUvs);
};