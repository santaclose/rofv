#pragma once
#include <imgui.h>
#include "Panel.h"
#include "ModelPanel.h"

class OptionsPanel : public Panel
{
private:
	ModelPanel* m_modelPanel;
	int m_currentMsaaCount = 8;
	bool m_wireframeEnabled = false;
	bool m_cullBackFace = false;
	bool m_smoothEnabled = false;
	bool m_renderTexture = false;
	bool m_saveUvsOption = false;

public:
	OptionsPanel(const std::string& name, ModelPanel* modelPanel, int msaaCount);
	void ImGuiCall(const ImGuiIO& io) override;
};