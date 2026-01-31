bool running = true;
extern bool esp_enabled;
extern bool line_enabled;
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
    fprintf(file, "esp_box_color=%d %d %d %d\n", esp_box_color.Value.x, esp_box_color.Value.y, esp_box_color.Value.z, esp_box_color.Value.w);
    fprintf(file, "line_color=%d %d %d %d\n", line_color.Value.x, line_color.Value.y, line_color.Value.z, line_color.Value.w);
    fprintf(file, "box_thickness=%.2f\n", box_thickness);
    fprintf(file, "line_thickness=%.2f\n", line_thickness);

    fclose(file);
}

void LoadConfig() {
    FILE* file = fopen("/sdcard/VoredomDLC/config.cfg", "r");
    if (!file) return;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "esp_enabled=")) {
            esp_enabled = atoi(line + 12) != 0;
        } else if (strstr(line, "line_enabled=")) {
            line_enabled = atoi(line + 13) != 0;
        } else if (strstr(line, "esp_box_color=")) {
            sscanf(line + 14, "%f %f %f %f", &esp_box_color.Value.x, &esp_box_color.Value.y, &esp_box_color.Value.z, &esp_box_color.Value.w);
        } else if (strstr(line, "line_color=")) {
            sscanf(line + 12, "%f %f %f %f", &line_color.Value.x, &line_color.Value.y, &line_color.Value.z, &line_color.Value.w);
        } else if (strstr(line, "box_thickness=")) {
            box_thickness = atof(line + 14);
        } else if (strstr(line, "line_thickness=")) {
            line_thickness = atof(line + 15);
        }
    }

    fclose(file);
}

void RenderMenu() {
    if (menu_visible) {
        ImGui::Begin("VoredomDLC", nullptr, ImGuiWindowFlags_NoResize);
        ImGui::SetWindowSize(ImVec2(700, 700));
        ImGui::BeginTabBar("Menu Tabs");
        if (ImGui::BeginTabItem("ESP")) {
            ImGui::Checkbox("Box", &esp_enabled);
            ImGui::Checkbox("Line", &line_enabled);
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
    }
}
