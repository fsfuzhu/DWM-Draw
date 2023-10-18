#include "draw.h"
#include "imgui/imgui.h"
#include "communication/communication.h"
#include <misc/importer.hpp>
#include <misc/render_cache.hpp>
#include "xorstr.hpp"
extern unsigned char font_fang_zheng_zhun_yuan_jianti[3022028];
namespace draw{
    communication::buffer* share_buffer = nullptr;
    bool init() {
        auto& io = ImGui::GetIO();
        io.BackendPlatformName = xorstr_("dwm");




        
        quick_import_function("User32.dll", EnumDisplaySettingsA);

        DEVMODEA dm;
        dm.dmSize = sizeof(DEVMODEA);
        dm.dmDriverExtra = 0;
        EnumDisplaySettingsA(NULL, ENUM_CURRENT_SETTINGS, &dm);
        float fontsize = dm.dmPelsWidth > 2000 ? 16.0f : 13.0f;
        io.Fonts->AddFontFromMemoryTTF(font_fang_zheng_zhun_yuan_jianti, sizeof(font_fang_zheng_zhun_yuan_jianti), fontsize, NULL, io.Fonts->GetGlyphRangesChineseFull());

        share_buffer = communication::communica::create_buffer();

        quick_import_function("User32.dll", SetWindowLongPtrA);
        quick_import_function("User32.dll", GetWindowLongPtrA);
        quick_import_function("User32.dll", FindWindowA);

        if (share_buffer) {
            SetWindowLongPtrA(FindWindowA(xorstr_("Dwm"),0), GWLP_USERDATA, share_buffer->get_combination_handle());
        }


        return GetWindowLongPtrA(FindWindowA(xorstr_("Dwm"), 0), GWLP_USERDATA) ? true : false;
    }
    void draw_call()
    {
        //OutputDebugStringA("2222222");
        //»æÖÆ
        static std::vector<int8_t> need_draw_data;
        need_draw_data.clear();
        if (share_buffer->get_data(need_draw_data)) {
            misc::render_cache::draw_info* draw_data = (misc::render_cache::draw_info*)(need_draw_data.data());

            for (; draw_data->type != misc::render_cache::draw_type::unknown; draw_data++)
            {
                //printf("draw type %d\n", draw_data->type);
                if (draw_data->type == misc::render_cache::draw_type::text) {

                    if (draw_data->info.text.outline) {
                        ImGui::GetBackgroundDrawList()->AddText(
                            ImVec2(draw_data->info.text.x + 1.0f, draw_data->info.text.y + 1.0f),
                            ImColor(0.0f, 0.0f, 0.0f, 0.7f),
                            draw_data->info.text.text
                        );
          
                    }
                    ImGui::GetBackgroundDrawList()->AddText(
                        ImVec2(draw_data->info.text.x, draw_data->info.text.y),
                        draw_data->info.text.color,
                        draw_data->info.text.text
                    );
                }
                else if (draw_data->type == misc::render_cache::draw_type::line) {
                    ImGui::GetBackgroundDrawList()->AddLine(
                        ImVec2(draw_data->info.line.x1, draw_data->info.line.y1),
                        ImVec2(draw_data->info.line.x2, draw_data->info.line.y2),
                        draw_data->info.line.color,
                        draw_data->info.line.thickness
                    );

                }
                else if (draw_data->type == misc::render_cache::draw_type::rect) {
                    if (draw_data->info.rect.fill) {
                        ImGui::GetBackgroundDrawList()->AddRectFilled(
                            ImVec2(draw_data->info.rect.x, draw_data->info.rect.y),
                            ImVec2(draw_data->info.rect.x + draw_data->info.rect.w, draw_data->info.rect.y + draw_data->info.rect.h),
                            draw_data->info.rect.color,
                            draw_data->info.rect.rounding
                        );

                    }
                    else {
                        ImGui::GetBackgroundDrawList()->AddRect(
                            ImVec2(draw_data->info.rect.x, draw_data->info.rect.y),
                            ImVec2(draw_data->info.rect.x + draw_data->info.rect.w, draw_data->info.rect.y + draw_data->info.rect.h),
                            draw_data->info.rect.color,
                            draw_data->info.rect.rounding,
                            0,
                            draw_data->info.rect.thickness
                        );
                    }
                }
                else if (draw_data->type == misc::render_cache::draw_type::circle) {
                    if (draw_data->info.circle.fill) {
                        ImGui::GetBackgroundDrawList()->AddCircleFilled(
                            ImVec2(draw_data->info.circle.x, draw_data->info.circle.y),
                            draw_data->info.circle.radius,
                            draw_data->info.circle.color,
                            64
                        );
                    }
                    else {
                        ImGui::GetBackgroundDrawList()->AddCircle(
                            ImVec2(draw_data->info.circle.x, draw_data->info.circle.y),
                            draw_data->info.circle.radius,
                            draw_data->info.circle.color,
                            64,
                            draw_data->info.circle.thickness
                        );
                    }
                }







            }
        }


    }
};

#include "communication/communication.cpp"