#pragma once
#include <vector>
#include <string>
#include <cstdint>
namespace misc {
	namespace render_cache {
#pragma pack(4) 
		enum class draw_type : int32_t
		{
			unknown = 0,
			text,
			circle,
			rect,
			line,
		};
		struct text_buffer {
			float_t x;
			float_t y;
			uint32_t color;
			int32_t size;
			bool outline;
			char text[0x1EB];
			text_buffer(const text_buffer& copy) :x(copy.x), y(copy.y), color(copy.color), size(copy.size), outline(copy.outline){
				memcpy(text, copy.text, sizeof(text));
			}
			text_buffer() {}
		};

		struct circle_buffer {
			float_t x;
			float_t y;
			float_t radius;
			uint32_t color;
			float_t thickness;
			bool fill;
			circle_buffer() {}
		};
		struct rect_buffer {
			float_t x;
			float_t y;
			float_t w;
			float_t h;
			uint32_t color;
			float_t thickness;
			float_t rounding;
			bool fill;
			rect_buffer() {}
		};
		struct line_buffer {
			float_t x1;
			float_t y1;
			float_t x2;
			float_t y2;
			uint32_t color;
			float_t thickness;
			line_buffer() {}
		};
		union _info
		{
			text_buffer	  text;
			circle_buffer circle;
			rect_buffer   rect;
			line_buffer	  line;
			_info() { }
			~_info() { }
		};
		struct draw_info {
			draw_type type;
			_info info;
			draw_info(draw_type type) :type(type) { memset(&info, 0, sizeof(info)); }
			draw_info(const draw_info& copy) {
				this->type = copy.type;
				if (copy.type == draw_type::text) {
					this->info.text = copy.info.text;
				}
				else if (copy.type == draw_type::line) {
					this->info.line = copy.info.line;
				}
				else if (copy.type == draw_type::rect) {
					this->info.rect = copy.info.rect;
				}
				else if (copy.type == draw_type::circle) {
					this->info.circle = copy.info.circle;
				}
			}
		};
		//[+][text_buffer]    0x1FC   Line[131]
		//[+][circle_buffer]  0x18    Line[132]
		//[+][rect_buffer]    0x20    Line[133]
		//[+][line_buffer]    0x18    Line[134]
		//[+][draw_info]      0x200   Line[135]
		static_assert(sizeof(text_buffer) == 0x1FC, "You have modified the definition of structure, please modify text_buffer::text[xxx] to maintain memory alignment");
		static_assert(sizeof(circle_buffer) == 0x18, "You have modified the definition of structure, please modify text_buffer::text[xxx] to maintain memory alignment");
		static_assert(sizeof(rect_buffer) == 0x20, "You have modified the definition of structure, please modify text_buffer::text[xxx] to maintain memory alignment");
		static_assert(sizeof(line_buffer) == 0x18, "You have modified the definition of structure, please modify text_buffer::text[xxx] to maintain memory alignment");
		static_assert(sizeof(draw_info) == 0x200, "You have modified the definition of structure, please modify text_buffer::text[xxx] to maintain memory alignment");


#pragma pack() 

		class draw_list {
		private:
			std::vector<draw_info>draw_buffer;
			size_t g_screen_w;
			size_t g_screen_h;
			

		public:
			void init(size_t init_size, size_t screen_w, size_t screen_h) {
				draw_buffer.reserve(init_size);
				g_screen_w = screen_w;
				g_screen_h = screen_h;
			}

			float_t width() {
				return (float_t)g_screen_w;
			}
			float_t height() {
				return (float_t)g_screen_h;
			}
			void    new_frame() {
				draw_buffer.clear();
			}
			void    add_text(const std::wstring& str, float_t x, float_t y, uint32_t color, int32_t size,bool outline = true) {
				draw_info info(draw_type::text);
				info.info.text.x = x;
				info.info.text.y = y;
				info.info.text.color = color;
				info.info.text.size = size;
				info.info.text.outline = outline;
				size_t copy_size = str.size() * sizeof(std::wstring::value_type);
				copy_size = copy_size > sizeof(info.info.text.text) ? sizeof(info.info.text.text) : copy_size;
				memcpy_s(info.info.text.text, sizeof(info.info.text.text), str.data(), copy_size);
				draw_buffer.push_back(info);
			}
			void    add_text(const std::string& str, float_t x, float_t y, uint32_t color, int32_t size, bool outline = true) {
				draw_info info(draw_type::text);
				info.info.text.x = x;
				info.info.text.y = y;
				info.info.text.color = color;
				info.info.text.size = size;
				info.info.text.outline = outline;
				size_t copy_size = str.size() * sizeof(std::string::value_type);
				copy_size = copy_size > sizeof(info.info.text.text) ? sizeof(info.info.text.text) : copy_size;
				memcpy_s(info.info.text.text, sizeof(info.info.text.text), str.data(), copy_size);
				draw_buffer.push_back(info);
			}

			void    add_line(float_t p1_x, float_t p1_y, float_t p2_x, float_t p2_y, uint32_t color, float_t thickness) {
				draw_info info(draw_type::line);
				info.info.line.x1 = p1_x;
				info.info.line.y1 = p1_y;
				info.info.line.x2 = p2_x;
				info.info.line.y2 = p2_y;
				info.info.line.color = color;
				info.info.line.thickness = thickness;
				draw_buffer.push_back(info);
			}
			void    add_rect(float_t x, float_t y, float_t w, float_t h, uint32_t color, float_t thickness, float_t rounding) {
				draw_info info(draw_type::rect);
				info.info.rect.x = x;
				info.info.rect.y = y;
				info.info.rect.w = w;
				info.info.rect.h = h;
				info.info.rect.color = color;
				info.info.rect.thickness = thickness;
				info.info.rect.rounding = rounding;
				info.info.rect.fill = false;
				draw_buffer.push_back(info);
			}
			void    add_rect_filled(float_t x, float_t y, float_t w, float_t h, uint32_t color, float_t thickness, float_t rounding) {
				draw_info info(draw_type::rect);
				info.info.rect.x = x;
				info.info.rect.y = y;
				info.info.rect.w = w;
				info.info.rect.h = h;
				info.info.rect.color = color;
				info.info.rect.thickness = thickness;
				info.info.rect.rounding = rounding;
				info.info.rect.fill = true;
				draw_buffer.push_back(info);
			}
			void    add_circle(float_t x, float_t y, float_t radius, uint32_t color, float_t thickness) {
				draw_info info(draw_type::circle);
				info.info.circle.x = x;
				info.info.circle.y = y;
				info.info.circle.radius = radius;
				info.info.circle.color = color;
				info.info.circle.thickness = thickness;
				info.info.circle.fill = false;
				draw_buffer.push_back(info);
			}
			void    add_circle_filled(float_t x, float_t y, float_t radius, uint32_t color, float_t thickness) {
				draw_info info(draw_type::circle);
				info.info.circle.x = x;
				info.info.circle.y = y;
				info.info.circle.radius = radius;
				info.info.circle.color = color;
				info.info.circle.thickness = thickness;
				info.info.circle.fill = true;
				draw_buffer.push_back(info);
			}
			std::vector<draw_info>& get_draw_data()
			{
				return draw_buffer;
			}
		
		};

	};
};