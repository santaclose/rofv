#pragma once
#include <imgui.h>
#include "Panel.h"

class ParametersPanel : public Panel
{
	bool* m_modelGenerationPending;

public:
	ParametersPanel(const std::string& name, bool* modelGenerationPending);
	void ImGuiCall(const ImGuiIO& io) override;
};