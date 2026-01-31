bool running = true;

void RenderMenu() {
    ImGui::Checkbox("box", &esp_enabled);

    g_window = ImGui::GetCurrentWindow();
    ImGui::End();
}
