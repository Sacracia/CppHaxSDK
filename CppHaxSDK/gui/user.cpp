#include <imgui.h>
#include <string>

static ImVec4 HexToColor(const std::string& hex_string) {
	int r, g, b, a;
	sscanf_s(hex_string.c_str(), "%02x%02x%02x%02x", &r, &g, &b, &a);
	ImVec4 color{ (float(r) / 255), (float(g) / 255), (float(b) / 255), (float(a) / 255) };
	return color;
}

void ApplyStyle() {
	ImGuiStyle* styles = &ImGui::GetStyle();
	auto colors = styles->Colors;
	colors[ImGuiCol_Border] = HexToColor("26383FFF");
	colors[ImGuiCol_BorderShadow] = HexToColor("33333300");
	colors[ImGuiCol_Button] = HexToColor("23303DFF");
	colors[ImGuiCol_ButtonActive] = HexToColor("474968FF");
	colors[ImGuiCol_ButtonHovered] = HexToColor("444C70FF");
	colors[ImGuiCol_CheckMark] = HexToColor("A5BCDBFF");
	colors[ImGuiCol_ChildBg] = HexToColor("1E262BFF");
	colors[ImGuiCol_DragDropTarget] = HexToColor("FFFF00E5");
	colors[ImGuiCol_FrameBg] = HexToColor("2D3F44FF");
	colors[ImGuiCol_FrameBgActive] = HexToColor("30383DFF");
	colors[ImGuiCol_FrameBgHovered] = HexToColor("26303DFF");
	colors[ImGuiCol_Header] = HexToColor("0000003D");
	colors[ImGuiCol_HeaderActive] = HexToColor("0070EAFF");
	colors[ImGuiCol_HeaderHovered] = HexToColor("1E2833CC");
	colors[ImGuiCol_MenuBarBg] = HexToColor("1E232DFF");
	colors[ImGuiCol_ModalWindowDimBg] = HexToColor("CCCCCC59");
	colors[ImGuiCol_NavHighlight] = HexToColor("4296F9FF");
	colors[ImGuiCol_NavWindowingDimBg] = HexToColor("CCCCCC33");
	colors[ImGuiCol_NavWindowingHighlight] = HexToColor("FFFFFFB2");
	colors[ImGuiCol_PlotHistogram] = HexToColor("E5B200FF");
	colors[ImGuiCol_PlotHistogramHovered] = HexToColor("FF9900FF");
	colors[ImGuiCol_PlotLines] = HexToColor("9B9B9BFF");
	colors[ImGuiCol_PlotLinesHovered] = HexToColor("FF6D59FF");
	colors[ImGuiCol_PopupBg] = HexToColor("14161CEF");
	colors[ImGuiCol_ResizeGrip] = HexToColor("A3C9F93F");
	colors[ImGuiCol_ResizeGripActive] = HexToColor("6D8CB2F2");
	colors[ImGuiCol_ResizeGripHovered] = HexToColor("A5BFDDAA");
	colors[ImGuiCol_ScrollbarBg] = HexToColor("1C1C1C63");
	colors[ImGuiCol_ScrollbarGrab] = HexToColor("875E5EFF");
	colors[ImGuiCol_ScrollbarGrabActive] = HexToColor("8E1919FF");
	colors[ImGuiCol_ScrollbarGrabHovered] = HexToColor("7C3A3AFF");
	colors[ImGuiCol_Separator] = HexToColor("333F49FF");
	colors[ImGuiCol_SeparatorActive] = HexToColor("6B91BFFF");
	colors[ImGuiCol_SeparatorHovered] = HexToColor("4F7299C6");
	colors[ImGuiCol_SliderGrab] = HexToColor("5977ADFF");
	colors[ImGuiCol_SliderGrabActive] = HexToColor("ADCCFFFF");
	colors[ImGuiCol_Tab] = HexToColor("1C262BFF");
	colors[ImGuiCol_TabActive] = HexToColor("333F49FF");
	colors[ImGuiCol_TabHovered] = HexToColor("969696CC");
	colors[ImGuiCol_TabUnfocused] = HexToColor("1C262BFF");
	colors[ImGuiCol_TabUnfocusedActive] = HexToColor("1C262BFF");
	colors[ImGuiCol_TableBorderLight] = HexToColor("3A3A3FFF");
	colors[ImGuiCol_TableBorderStrong] = HexToColor("4F4F59FF");
	colors[ImGuiCol_TableHeaderBg] = HexToColor("303033FF");
	colors[ImGuiCol_TableRowBg] = HexToColor("333F49FF");
	colors[ImGuiCol_TableRowBgAlt] = HexToColor("1C262BFF");
	colors[ImGuiCol_Text] = HexToColor("F2F4F9FF");
	colors[ImGuiCol_TextDisabled] = HexToColor("2B353DFF");
	colors[ImGuiCol_TextSelectedBg] = HexToColor("4296F959");
	colors[ImGuiCol_TitleBg] = HexToColor("232D38A5");
	colors[ImGuiCol_TitleBgActive] = HexToColor("212830FF");
	colors[ImGuiCol_TitleBgCollapsed] = HexToColor("26262682");
	colors[ImGuiCol_WindowBg] = HexToColor("1E2623FF");

	//Styles
	styles->Alpha = 1.0;
	styles->AntiAliasedFill = true;
	styles->AntiAliasedLines = true;
	styles->AntiAliasedLinesUseTex = true;
	styles->ButtonTextAlign = ImVec2(0.5, 0.5);
	styles->CellPadding = ImVec2(4.0, 2.0);
	styles->ChildBorderSize = 1.0;
	styles->ChildRounding = 5.0;
	styles->CircleTessellationMaxError = 0.30000001192092896;
	styles->ColorButtonPosition = ImGuiDir_Right;
	styles->ColumnsMinSpacing = 6.0;
	styles->CurveTessellationTol = 1.25;
	styles->DisabledAlpha = 0.6000000238418579;
	styles->DisplaySafeAreaPadding = ImVec2(3.0, 3.0);
	styles->DisplayWindowPadding = ImVec2(19.0, 19.0);
	styles->FrameBorderSize = 0.0;
	styles->FramePadding = ImVec2(4.0, 3.0);
	styles->FrameRounding = 4.0;
	styles->GrabMinSize = 10.0;
	styles->GrabRounding = 4.0;
	styles->IndentSpacing = 21.0;
	styles->ItemInnerSpacing = ImVec2(4.0, 4.0);
	styles->ItemSpacing = ImVec2(8.0, 4.0);
	styles->LogSliderDeadzone = 4.0;
	styles->MouseCursorScale = 1.0;
	styles->PopupBorderSize = 1.0;
	styles->PopupRounding = 0.0;
	styles->ScrollbarRounding = 9.0;
	styles->ScrollbarSize = 14.0;
	styles->SelectableTextAlign = ImVec2(0.0, 0.0);
	styles->TabBorderSize = 0.0;
	styles->TabMinWidthForCloseButton = 0.0;
	styles->TabRounding = 4.0;
	styles->TouchExtraPadding = ImVec2(0.0, 0.0);
	styles->WindowBorderSize = 1.0;
	styles->WindowMenuButtonPosition = ImGuiDir_Left;
	styles->WindowMinSize = ImVec2(32.0, 32.0);
	styles->WindowPadding = ImVec2(8.0, 8.0);
	styles->WindowRounding = 0.0;
	styles->WindowTitleAlign = ImVec2(0.0, 0.5);
}