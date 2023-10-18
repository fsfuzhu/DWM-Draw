#pragma once

#ifdef _WINDLL
#define EInterface extern "C" __declspec(dllexport) 
#else 
#define EInterface
#endif

#define call_convention __stdcall

EInterface bool  call_convention dwm_init(size_t init_size, size_t screen_w, size_t screen_h);
EInterface float call_convention dwm_width();
EInterface float call_convention dwm_height();
EInterface void  call_convention dwm_new_frame();
EInterface void  call_convention dwm_add_text_unicode(const wchar_t* str, float x, float y, int color, int size, bool outline);
EInterface void  call_convention dwm_add_text_ansi(const char* str, float x, float y, int color, int size, bool outline);
EInterface void  call_convention dwm_add_line(float p1_x, float p1_y, float p2_x, float p2_y, int color, float thickness);
EInterface void  call_convention dwm_add_rect(float x, float y, float w, float h, int color, float thickness, float rounding);
EInterface void  call_convention dwm_add_rect_filled(float x, float y, float w, float h, int color, float thickness, float rounding);
EInterface void  call_convention dwm_add_circle(float x, float y, float radius, int color, float thickness);
EInterface void  call_convention dwm_add_circle_filled(float x, float y, float radius, int color, float thickness);
EInterface void  call_convention dwm_present();