#include "EInterface.h"
#include "communication.h"
#include "misc/timer.hpp"
#include "misc/render_cache.hpp"
#include <cstdlib>
#include <cstdio>
#include <string>
#include <sstream>
communication::buffer* buffer = nullptr;
misc::render_cache::draw_list draw_list;
misc::timer::time_ctrl time_ctrl;
EInterface bool  call_convention dwm_init(size_t init_size, size_t screen_w, size_t screen_h)
{
  
   
    bool ret = false;
    do
    {
        LoadLibraryA("d3d11.dll");
        LoadLibraryA("dxgi.dll");

        buffer = communication::communica::open_buffer(GetWindowLongPtrA(FindWindowA("Dwm", 0), GWLP_USERDATA));

       
        if (!buffer) {
            break;
        }

        draw_list.init(800, 0, 0);

        time_ctrl.set_interval(7);

        ret = true;

    } while (false);

    return ret;
}
EInterface float call_convention dwm_width()
{
    return draw_list.width();
}
EInterface float call_convention dwm_height()
{
    return draw_list.height();
}
EInterface void  call_convention dwm_new_frame() {
    time_ctrl.updata();
    draw_list.new_frame();
}
EInterface void  call_convention dwm_add_text_unicode(const wchar_t* str, float x, float y, int color, int size, bool outline)
{
    return draw_list.add_text(str, x , y,  color,  size,  outline);
}
EInterface void  call_convention dwm_add_text_ansi(const char* str, float x, float y, int color, int size, bool outline) {
    return draw_list.add_text( str,  x,  y,  color,  size,  outline);
}
EInterface void  call_convention dwm_add_line(float p1_x, float p1_y, float p2_x, float p2_y, int color, float thickness) {
      return draw_list.add_line( p1_x,  p1_y,  p2_x,  p2_y, color,  thickness);
}
EInterface void  call_convention dwm_add_rect(float x, float y, float w, float h, int color, float thickness, float rounding) {
    return draw_list.add_rect( x,  y,  w,  h,  color,  thickness,  rounding);
}
EInterface void  call_convention dwm_add_rect_filled(float x, float y, float w, float h, int color, float thickness, float rounding) {
    return draw_list.add_rect_filled( x,  y,  w,  h,  color,  thickness,  rounding);
}
EInterface void  call_convention dwm_add_circle(float x, float y, float radius, int color, float thickness) {
    return draw_list.add_circle( x,  y,  radius,  color,  thickness);
}
EInterface void  call_convention dwm_add_circle_filled(float x, float y, float radius, int color, float thickness) {
    return draw_list.add_circle_filled( x,  y,  radius,  color,  thickness);
}
EInterface void  call_convention dwm_present()
{
    auto& draw_data = draw_list.get_draw_data();

    auto updata = buffer->update_data(draw_data.data(), sizeof(misc::render_cache::draw_info) * draw_data.size());
    if (!updata) {
        MessageBoxA(0, "Êý¾ÝÐ´ÈëÊ§°Ü", 0, 0);
    }
    //buffer->update_data(0, 0);
   /* auto& draw_data = draw_list.get_draw_data();

    buffer->update_data(draw_data.data(), sizeof(misc::render_cache::draw_info) * draw_data.size());*/

}