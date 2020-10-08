#include "OptionsPanel.h"

#include <sstream>

OptionsPanel::OptionsPanel(const std::string& name, ModelPanel* modelPanel, int msaaCount) : Panel(name)
{
	m_modelPanel = modelPanel;
	m_currentMsaaCount = msaaCount;
}

void OptionsPanel::ImGuiCall(const ImGuiIO& io)
{
	ImGui::Begin(m_name.c_str());

	int lastMsaaCount = m_currentMsaaCount;
	ImGui::SliderInt("MSAA Count", &m_currentMsaaCount, 2, 8);
	if (
		(m_currentMsaaCount == 2 || m_currentMsaaCount == 4 || m_currentMsaaCount == 8) &&
		m_currentMsaaCount != lastMsaaCount
		)
	{
		m_modelPanel->SetSamples((uint32_t)m_currentMsaaCount);
	}

	ImGui::Checkbox("Wireframe", &m_wireframeEnabled);
	if (m_wireframeEnabled)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	m_modelPanel->SetLightingEnabled(!m_wireframeEnabled);

	ImGui::Checkbox("Cull back face", &m_cullBackFace);
	if (m_cullBackFace)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);


	ImGui::Checkbox("Smooth enabled", &m_smoothEnabled);
	m_modelPanel->SetSmoothEnabled(m_smoothEnabled);

	std::stringstream ss;
	ss << "Vertex count: " << m_modelPanel->GetLastVertexCount() << '\n';
	ImGui::Text(ss.str().c_str());

	//ImGui::Checkbox("Save UVs", &m_saveUvsOption);

	if (ImGui::Button("Save"))
		m_modelPanel->SaveModel(m_saveUvsOption);

	ImGui::End();
}