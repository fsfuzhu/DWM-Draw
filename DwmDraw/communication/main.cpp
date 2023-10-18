#include "communication.h"
#include "misc/timer.hpp"
#include "misc/render_cache.hpp"
#include <cstdlib>
#include <cstdio>
#include <string>
#include <sstream>


#define IM_COL32_R_SHIFT    0
#define IM_COL32_G_SHIFT    8
#define IM_COL32_B_SHIFT    16
#define IM_COL32_A_SHIFT    24
#define IM_COL32_A_MASK     0xFF000000

#define IM_COL32(R,G,B,A)    (((uint32_t)(A)<<IM_COL32_A_SHIFT) | ((uint32_t)(B)<<IM_COL32_B_SHIFT) | ((uint32_t)(G)<<IM_COL32_G_SHIFT) | ((uint32_t)(R)<<IM_COL32_R_SHIFT))
#define IM_COL32_WHITE       IM_COL32(255,255,255,255)  // Opaque white = 0xFFFFFFFF
#define IM_COL32_BLACK       IM_COL32(0,0,0,255)        // Opaque black
#define IM_COL32_BLACK_TRANS IM_COL32(0,0,0,0)          // Transparent black = 0x00000000



#ifdef _WINDLL
BOOL APIENTRY DllMain(HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved)
{
    
    return true;
}
#else
char test_data[] = "ÄãºÃ»º³åÇø!\n";
DWORD thread(LPVOID) {
    communication::buffer* buffer = nullptr;

    buffer = communication::communica::create_buffer();

    misc::render_cache::draw_list draw_list;
    draw_list.init(800, 0, 0);

    if (!buffer) {
        OutputDebugStringA("´´½¨Í¨Ñ¶Ê§°Ü\n");
        return 0;
    }
    else
    {
        OutputDebugStringA("´´½¨Í¨Ñ¶³É¹¦\n");
        delete buffer;
    }

    return 0;

}

int main(int argv, char** argc) {

    system("pause");

    printf("%X\n%X\n", LoadLibraryA("d3d11.dll"), LoadLibraryA("dxgi.dll"));
  

    communication::buffer* buffer = nullptr;

   
	buffer = communication::communica::open_buffer(GetWindowLongPtrA(FindWindowA("Dwm", 0), GWLP_USERDATA));

    misc::render_cache::draw_list draw_list;
    draw_list.init(800, 0, 0);

    if (!buffer) {
        std::printf("´´½¨Í¨Ñ¶Ê§°Ü\n");
        return 0;
    }


    std::printf("buffer size               %I64d\n", buffer->size());
    std::printf("buffer combination_handle %p\n", buffer->get_combination_handle());
    //std::printf("buffer shared_handle      %p\n", buffer->get_shared_handle());


    misc::timer::time_ctrl time_ctrl;
    time_ctrl.set_interval(7);

    while (!(GetAsyncKeyState(VK_END) & 0x8000))
    {
        time_ctrl.updata();

        draw_list.new_frame();
        
        const char* test_txt = u8"¦²×ÔÃé×ÔÃé×ÔÃé£¡£¡£¡\nÎäÆ÷\n ×°±¸\n MI6A4\n AK\n ¿ÕÍ¶\nÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷\nÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷\nÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷Îä\nÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷\nÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷\nÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷\nÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷\nÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷\nÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷\nÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷\nÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷ÎäÆ÷\n";
       
        
		draw_list.add_text(test_txt, 10.0f, 50.0f, IM_COL32(100, 100, 0, 255), 50);

        draw_list.add_line(100, 100, 500, 300, 0xff0000ff, 1.0f);
        draw_list.add_rect(600, 300, 50, 50, 0xff0000ff, 1.0f, 5.0f);
        draw_list.add_rect_filled(800, 300, 50, 50, 0xff0000ff, 1.0f, 5.0f);
        draw_list.add_circle(100, 900, 50, 0xff0000ff, 1.0f);
        draw_list.add_circle_filled(700, 900, 50, 0xffffff00, 1.0f);

        auto& draw_data = draw_list.get_draw_data();

        auto updata = buffer->update_data(draw_data.data(), sizeof(misc::render_cache::draw_info) * draw_data.size());

        if (!updata) {
            std::printf("Êý¾ÝÐ´ÈëÊ§°Ü\n");
            break;
        }


        Sleep(7);
    }

    buffer->update_data(0, 0);

    if (buffer) {
        delete buffer;
    }


    return 0;
}
#endif

