bool esp_enabled = false;
bool line_enabled = false;
bool enemies_counter_enabled = false;
extern ImColor esp_box_color;
extern ImColor line_color;
extern float box_thickness;
extern float line_thickness;

ImDrawList* g_Draw = nullptr;

void DrawBox(const ImRect& r, float alpha) {
    ImColor col = esp_box_color;
    col.Value.w *= alpha;
    ImColor black = ImColor(0, 0, 0, static_cast<int>(255 * alpha));

    g_Draw->AddRect(r.Min, r.Max, black, 0, 0, box_thickness + 1.0f);
    g_Draw->AddRect(r.Min, r.Max, col, 0, 0, box_thickness);
}

void player() {
    g_Draw = ImGui::GetBackgroundDrawList();

    if(!esp_enabled && !line_enabled) return;

    uint64_t playermanager = rpm<uint64_t>(rpm<uint64_t>(rpm<uint64_t>(rpm<uint64_t>(il2cpp_base + 135070688) + 0x58) + 0xB8) + 0x0);
    if(!playermanager) return;

    uint64_t playersList = rpm<uint64_t>(playermanager + 0x28);
    uint64_t localPlayer = rpm<uint64_t>(playermanager + 0x70);

    if(!playersList || !localPlayer) return;

    int playersListSize = rpm<int>(playersList + 0x20);
    if(playersListSize <= 0 || playersListSize > 32) return;

    uint64_t localPhoton = rpm<uint64_t>(localPlayer + 0x158);
    uint64_t localProps = rpm<uint64_t>(localPhoton + 0x38);
    int localTeam = 0;
    if(localProps) {
        int propsSize = rpm<int>(localProps + 0x20);
        for(int j = 0; j < propsSize; j++) {
            uint64_t propsList = rpm<uint64_t>(localProps + 0x18);
            uint64_t propkey = rpm<uint64_t>(propsList + 0x28 + 0x18 * j);
            if(propkey) {
                char keyBuf[32] = {0};
                int keyLen = rpm<int>(propkey + 0x10);
                if(keyLen > 0 && keyLen < 20) {
                    for(int k = 0; k < keyLen; k++) {
                        keyBuf[k] = rpm<char>(propkey + 0x14 + k * 2);
                    }
                    if(strstr(keyBuf, "team")) {
                        uint64_t propval = rpm<uint64_t>(propsList + 0x30 + 0x18 * j);
                        localTeam = rpm<int>(propval + 0x10);
                        break;
                    }
                }
            }
        }
    }

    Matrix mat = rpm<Matrix>(rpm<uint64_t>(rpm<uint64_t>(rpm<uint64_t>(localPlayer + 0xE0) + 0x20) + 0x10) + 0x100);

    for(int i = 0; i < playersListSize; i++) {
        uint64_t player = rpm<uint64_t>(rpm<uint64_t>(playersList + 0x18) + 0x30 + 0x18 * i);
        if(!player) continue;

        uint64_t photon = rpm<uint64_t>(player + 0x158);
        uint64_t props = rpm<uint64_t>(photon + 0x38);

        int hp = 0;
        int playerTeam = 0;

        if(props) {
            int propsSize = rpm<int>(props + 0x20);
            for(int j = 0; j < propsSize; j++) {
                uint64_t propsList = rpm<uint64_t>(props + 0x18);
                uint64_t propkey = rpm<uint64_t>(propsList + 0x28 + 0x18 * j);
                if(propkey) {
                    char keyBuf[32] = {0};
                    int keyLen = rpm<int>(propkey + 0x10);
                    if(keyLen > 0 && keyLen < 20) {
                        for(int k = 0; k < keyLen; k++) {
                            keyBuf[k] = rpm<char>(propkey + 0x14 + k * 2);
                        }
                        uint64_t propval = rpm<uint64_t>(propsList + 0x30 + 0x18 * j);
                        if(strstr(keyBuf, "health")) {
                            hp = rpm<int>(propval + 0x10);
                        }
                        if(strstr(keyBuf, "team")) {
                            playerTeam = rpm<int>(propval + 0x10);
                        }
                    }
                }
            }
        }

        if(hp <= 0) continue;
        if(playerTeam == localTeam) continue;

        vec3_t position = rpm<vec3_t>(rpm<uint64_t>(rpm<uint64_t>(player + 0x98) + 0xB0) + 0x44);
        if(position.x == 0 && position.y == 0 && position.z == 0) continue;

        auto WorldToScreen = [&](vec3_t pos) -> vec3_t {
            float screenX = (mat.m11 * pos.x) + (mat.m21 * pos.y) + (mat.m31 * pos.z) + mat.m41;
            float screenY = (mat.m12 * pos.x) + (mat.m22 * pos.y) + (mat.m32 * pos.z) + mat.m42;
            float screenW = (mat.m14 * pos.x) + (mat.m24 * pos.y) + (mat.m34 * pos.z) + mat.m44;

            vec3_t result;
            if(screenW < 0.0001f) {
                result.z = -1;
                return result;
            }

            float camX = abs_ScreenX / 2.0f;
            float camY = abs_ScreenY / 2.0f;
            result.x = camX + (camX * screenX / screenW);
            result.y = camY - (camY * screenY / screenW);
            result.z = screenW;
            return result;
        };

        vec3_t w2sTop = WorldToScreen(vec3_t(position.x, position.y + 1.67f, position.z));
        vec3_t w2sBottom = WorldToScreen(position);

        if(w2sTop.z <= 0 || w2sBottom.z <= 0) continue;

        float pmtXtop = w2sTop.x;
        float pmtXbottom = w2sBottom.x;
        if(w2sTop.x > w2sBottom.x) {
            pmtXtop = w2sBottom.x;
            pmtXbottom = w2sTop.x;
        }

        float boxWidth = fabs((w2sTop.y - w2sBottom.y) / 4.0f);
        ImRect r(ImVec2(pmtXtop - boxWidth, w2sTop.y), ImVec2(pmtXbottom + boxWidth, w2sBottom.y));

        if(line_enabled) {
            ImVec2 line_start(abs_ScreenX / 2.0f, abs_ScreenY);
            ImVec2 line_end((r.Min.x + r.Max.x) / 2.0f, r.Max.y);

            g_Draw->AddLine(line_start, line_end, line_color, line_thickness);
        }

        if(esp_enabled) {
            DrawBox(r, 1.0f);
        }
    }
}
