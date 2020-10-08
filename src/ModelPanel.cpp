#include "ModelPanel.h"
#include <glad/glad.h>
#include <iostream>
#include <cstdint>

#include "Model.h"
#include "modelTool/ml.h"

#define SENSITIVITY 0.01f
#define MAX_DISTANCE 7.0f
#define MIN_DISTANCE 2.0f

#define CAMERA_LIMIT 1.565

void ModelPanel::UpdateCamera()
{
	m_camera->SetPosition(m_gimbal.Forward() * m_distance);
	m_camera->LookAt(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
}

ModelPanel::ModelPanel(const std::string& name, const glm::vec3& clearColor, uint32_t msaaCount) : DisplayPanelMSAA(name, clearColor, 8, true)
{
	CameraSpecs ss;
	ss.perspective = true;
	m_camera = new Camera(ss);
	m_distance = 3.19f;
	m_gimbal.SetRotation(glm::fquat(m_gimbalRot));
	UpdateCamera();

	m_shader.CreateFromFiles("assets/shaders/vert.glsl", "assets/shaders/frag.glsl");
	m_shader.Bind();

	ml::Initialize();
}

ModelPanel::~ModelPanel()
{
	ml::Terminate();
}

void ModelPanel::OnResize()
{
	m_camera->m_specs.aspectRatio = m_size.x / m_size.y;
}

void ModelPanel::HandleInput(const ImGuiIO& io, const glm::vec2& relativeMousePos)
{
	m_distance -= io.MouseWheel;
	if (m_distance > MAX_DISTANCE)
		m_distance = MAX_DISTANCE;
	else if (m_distance < MIN_DISTANCE)
		m_distance = MIN_DISTANCE;

	if (io.MouseDown[2])
	{
		inputHandlingRetained = this;
		m_gimbalRot.y -= io.MouseDelta.x * SENSITIVITY;
		m_gimbalRot.x += io.MouseDelta.y * SENSITIVITY;

		if (m_gimbalRot.x > CAMERA_LIMIT)
			m_gimbalRot.x = CAMERA_LIMIT;
		else if (m_gimbalRot.x < -CAMERA_LIMIT)
			m_gimbalRot.x = -CAMERA_LIMIT;

		m_gimbal.SetRotation(glm::fquat(m_gimbalRot));
		m_lightDir = -m_gimbal.Forward();
	}
	else
	{
		inputHandlingRetained = nullptr;
	}
	UpdateCamera();
}

void ModelPanel::Draw()
{
	m_camera->Bind();
	m_camera->ComputeMatrices();

	m_shader.Bind();
	m_shader.SetUniformMatrix4fv("u_Mat", (float*)(&(m_camera->GetMatrix())));

	m_shader.SetUniform3fv("u_LightDir", &m_lightDir.x);
	m_shader.SetUniform1i("u_NoLighting", m_noLighting);
	m_shader.SetUniform1i("u_UseTexture", m_useTexture);

	//// Draw model
	if (m_modelGenerationPending)
	{
		//std::cout << "generating model\n";
		ml::GenerateModel(m_modelGenerationPending);
	}
	//std::cout << "drawing model\n";
	ml::DrawModel();
}

bool* ModelPanel::GetModelGenerationPendingBool()
{
	return &m_modelGenerationPending;
}

void ModelPanel::SetSmoothEnabled(bool value)
{
	m_modelGenerationPending |= ml::SetSmoothMode(value);
}

void ModelPanel::SetLightingEnabled(bool value)
{
	m_noLighting = !value;
}

unsigned int ModelPanel::GetLastVertexCount()
{
	return ml::GetLastDrawVertexCount();
}

void ModelPanel::SaveModel(bool saveUvs)
{
	ml::SetExporting();
	m_modelGenerationPending = true;
	std::cout << "exporting as obj\n";
}
