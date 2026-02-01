bool running = true;
extern bool esp_enabled;
extern bool line_enabled;
extern bool enemies_counter_enabled;
bool menu_visible = true;
ImColor esp_box_color = ImColor(255, 0, 0, 255);
ImColor line_color = ImColor(255, 255, 255, 255);
float box_thickness = 2.0f;
float line_thickness = 1.5f;

void SaveConfig() {
    FILE* file = fopen("/sdcard/VoredomDLC/config.cfg", "w");
    if (!file) return;

    fprintf(file, "esp_enabled=%d\n", esp_enabled ? 1 : 0);
    fprintf(file, "line_enabled=%d\n", line_enabled ? 1 : 0);
    fprintf(file, "enemies_counter_enabled=%d\n", enemies_counter_enabled ? 1 : 0);
    fprintf(file, "esp_box_color=%d %d %d %d\n",
        (int)roundf(esp_box_color.Value.x * 255.0f),
        (int)roundf(esp_box_color.Value.y * 255.0f),
        (int)roundf(esp_box_color.Value.z * 255.0f),
        (int)roundf(esp_box_color.Value.w * 255.0f));
    fprintf(file, "line_color=%d %d %d %d\n",
        (int)roundf(line_color.Value.x * 255.0f),
        (int)roundf(line_color.Value.y * 255.0f),
        (int)roundf(line_color.Value.z * 255.0f),
        (int)roundf(line_color.Value.w * 255.0f));
    fprintf(file, "box_thickness=%.2f\n", box_thickness);
    fprintf(file, "line_thickness=%.2f\n", line_thickness);

    fclose(file);
}

void LoadConfig() {
    FILE* file = fopen("/sdcard/VoredomDLC/config.cfg", "r");
    if (!file) return;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Убираем символ новой строки в конце, если он есть
        line[strcspn(line, "\n")] = 0;

        if (strncmp(line, "esp_enabled=", 12) == 0) {
            esp_enabled = atoi(line + 12) != 0;
        } else if (strncmp(line, "line_enabled=", 13) == 0) {
            line_enabled = atoi(line + 13) != 0;
        } else if (strncmp(line, "enemies_counter_enabled=", 25) == 0) {
            enemies_counter_enabled = atoi(line + 25) != 0;
        } else if (strncmp(line, "esp_box_color=", 14) == 0) {
            int r, g, b, a;
            sscanf(line + 14, "%d %d %d %d", &r, &g, &b, &a);
            esp_box_color.Value.x = r / 255.0f;
            esp_box_color.Value.y = g / 255.0f;
            esp_box_color.Value.z = b / 255.0f;
            esp_box_color.Value.w = a / 255.0f;
        } else if (strncmp(line, "line_color=", 11) == 0) {
            int r, g, b, a;
            sscanf(line + 11, "%d %d %d %d", &r, &g, &b, &a);
            line_color.Value.x = r / 255.0f;
            line_color.Value.y = g / 255.0f;
            line_color.Value.z = b / 255.0f;
            line_color.Value.w = a / 255.0f;
        } else if (strncmp(line, "box_thickness=", 14) == 0) {
            box_thickness = atof(line + 14);
        } else if (strncmp(line, "line_thickness=", 15) == 0) {
            line_thickness = atof(line + 15);
        }
    }

    fclose(file);
}

void RenderMenu() {
    if (menu_visible) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

        ImGui::Begin("VoredomDLC", nullptr, ImGuiWindowFlags_NoResize);
        ImGui::SetWindowSize(ImVec2(700, 700));
        ImGui::BeginTabBar("Menu Tabs");
        if (ImGui::BeginTabItem("ESP")) {
            ImGui::Checkbox("Box", &esp_enabled);
            ImGui::Checkbox("Line", &line_enabled);
            ImGui::Checkbox("Enemies", &enemies_counter_enabled);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Settings")) {
            ImGui::Text("Box Settings:");
            ImGui::ColorEdit4("Box Color", (float*)&esp_box_color.Value);
            ImGui::SliderFloat("Box Thickness", &box_thickness, 1.0f, 3.0f);
            box_thickness = round(box_thickness);
            if (box_thickness < 1.0f) box_thickness = 1.0f;
            if (box_thickness > 3.0f) box_thickness = 3.0f;

            ImGui::Separator();
            ImGui::Text("Line Settings:");
            ImGui::ColorEdit4("Line Color", (float*)&line_color.Value);
            ImGui::SliderFloat("Line Thickness", &line_thickness, 1.0f, 3.0f);
            line_thickness = round(line_thickness);
            if (line_thickness < 1.0f) line_thickness = 1.0f;
            if (line_thickness > 3.0f) line_thickness = 3.0f;
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Config")) {
            if (ImGui::Button("Save Config")) {
                system("mkdir -p /sdcard/VoredomDLC");
                SaveConfig();
            }
            ImGui::SameLine();
            if (ImGui::Button("Load Config")) {
                LoadConfig();
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
        g_window = ImGui::GetCurrentWindow();
        ImGui::End();

        ImGui::PopStyleColor(5);
    }
}
