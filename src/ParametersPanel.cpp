#include "ParametersPanel.h"
#include "Model.h"

ParametersPanel::ParametersPanel(const std::string& name, bool* modelGenerationPending) : Panel(name)
{
	m_modelGenerationPending = modelGenerationPending;
}

void ParametersPanel::ImGuiCall(const ImGuiIO& io)
{
	ImGui::Begin(m_name.c_str());
	Model::Bindings(*m_modelGenerationPending);
	ImGui::End();
}
