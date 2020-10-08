#include "Application.h"
#include "OptionsPanel.h"
#include "ParametersPanel.h"
#include "ModelPanel.h"
#include <iostream>

ModelPanel* modelPanel;
OptionsPanel* optionsPanel;
ParametersPanel* propertiesPanel;

void Application::Init()
{
	modelPanel = new ModelPanel("Model", glm::vec3(0.3, 0.3, 0.3), 8);
	optionsPanel = new OptionsPanel("Options", modelPanel, 8);
	propertiesPanel = new ParametersPanel("Parameters", modelPanel->GetModelGenerationPendingBool());
}

void Application::OnUpdate(const ImGuiIO& io)
{
	optionsPanel->ImGuiCall(io);
	propertiesPanel->ImGuiCall(io);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	modelPanel->ImGuiCall(io);
	ImGui::PopStyleVar();
}

void Application::Terminate()
{
	delete propertiesPanel;
	delete optionsPanel;
	delete modelPanel;
}
