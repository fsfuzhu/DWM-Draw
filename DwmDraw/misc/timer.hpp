#pragma once
#include <windows.h>
#include <chrono>
#include <thread>
#pragma push_macro("INLINE")
#undef INLINE
#define INLINE __forceinline
#pragma warning(push)
#pragma warning(disable:4996)
namespace misc {
    namespace timer {


        /*!
         * @brief 计时器
         */
        struct mill_recorder
        {
            using time_point = LARGE_INTEGER;

            time_point start;
            time_point frequency;

#ifdef function_ptr
            function_ptr(QueryPerformanceCounter);
            function_ptr(QueryPerformanceFrequency);
#endif // function_ptr


            mill_recorder(mill_recorder&&) = delete;

            mill_recorder() {
#ifdef function_ptr
                QueryPerformanceCounter = get_function_ptr("kernel32.dll", QueryPerformanceCounter);
                QueryPerformanceFrequency = get_function_ptr("kernel32.dll", QueryPerformanceFrequency);
#endif // function_ptr
                QueryPerformanceCounter(&start);
                QueryPerformanceFrequency(&frequency);
            }
            
            void reset() { 
                QueryPerformanceCounter(&start);
            }
            
            template<typename T>
            T get_interval() { 
                time_point now; 
                QueryPerformanceCounter(&now);
                double interval = (now.QuadPart - start.QuadPart) / (double)frequency.QuadPart;
                return static_cast<T>(interval * 1000);
            }
            
            template<typename T>
            operator T() {return get_interval<T>();}



        };


        struct micr_recorder
        {
            using time_point = LARGE_INTEGER;

            time_point start;
            time_point frequency;

#ifdef function_ptr
            function_ptr(QueryPerformanceCounter);
            function_ptr(QueryPerformanceFrequency);
#endif // function_ptr


            micr_recorder(micr_recorder&&) = delete;

            micr_recorder() {
#ifdef function_ptr
                QueryPerformanceCounter = get_function_ptr("kernel32.dll", QueryPerformanceCounter);
                QueryPerformanceFrequency = get_function_ptr("kernel32.dll", QueryPerformanceFrequency);
#endif // function_ptr
                QueryPerformanceCounter(&start);
                QueryPerformanceFrequency(&frequency);
            }

            void reset() {
                QueryPerformanceCounter(&start);
            }

            template<typename T>
            T get_interval() {
                time_point now;
                QueryPerformanceCounter(&now);
                double interval = (now.QuadPart - start.QuadPart) / (double)frequency.QuadPart;
                return static_cast<T>(interval * 1000 * 1000);
            }

            template<typename T>
            operator T() { return get_interval<T>(); }



        };

        class time_ctrl {
        private:
            void refresh_fps() {
                now_time_point = std::chrono::steady_clock::now();
                auto last_now = now_time_point - last_updata_time;
                if (std::chrono::duration_cast<std::chrono::milliseconds>(last_now).count() != 0)
                    _fps = 1000.0f / std::chrono::duration_cast<std::chrono::milliseconds>(last_now).count();
            }
        public:
            time_ctrl() {
                start_time_point = std::chrono::steady_clock::now();
                last_updata_time = start_time_point;
            }
            time_ctrl(size_t  interval) {
                start_time_point = std::chrono::steady_clock::now();
                last_updata_time = start_time_point;
                set_interval(interval);
            }
            ~time_ctrl() {

            }

            void start() {
                start_time_point = std::chrono::steady_clock::now();
                last_updata_time = start_time_point;
            }

            void updata() {
                now_time_point = std::chrono::steady_clock::now();
                updata_numbers++;

                auto last_now = now_time_point - last_updata_time;

                refresh_fps();

                if (Invertal > std::chrono::milliseconds(0)) {
                    if (!(std::chrono::duration_cast<std::chrono::milliseconds>(last_now) >= Invertal)) {

                        std::this_thread::sleep_for(Invertal - last_now);
                    }
                }

                last_updata_time = now_time_point;
            }

            void updata_no_sleep() {
                now_time_point = std::chrono::steady_clock::now();
                updata_numbers++;

                auto last_now = now_time_point - last_updata_time;

                refresh_fps();

                if (Invertal > std::chrono::milliseconds(0)) {
                    if (!(std::chrono::duration_cast<std::chrono::milliseconds>(last_now) >= Invertal)) {
                        //std::this_thread::sleep_for(Invertal - last_now);
                    }
                }

              

                last_updata_time = now_time_point;
            }

            void set_interval(size_t interval_) {
                Invertal = std::chrono::milliseconds(interval_);
            }

            float fps() const{
                return _fps;
            }


            size_t get_time_bypass() {
                auto bypass = std::chrono::steady_clock::now() - start_time_point;
                return static_cast<size_t>(std::chrono::duration_cast<std::chrono::milliseconds>(bypass).count());
            }

        private:
            //当前时间
            std::chrono::steady_clock::time_point now_time_point;
            //上次更新时间
            std::chrono::steady_clock::time_point last_updata_time;
            //开始时间
            std::chrono::steady_clock::time_point start_time_point;
            //更新次数
            size_t updata_numbers = 0;
            // 每一帧间隔
            std::chrono::milliseconds Invertal = std::chrono::milliseconds(16);
            float _fps = 0.0;
        };
       
    
    };

};





#pragma warning(pop)
#pragma pop_macro("INLINE")