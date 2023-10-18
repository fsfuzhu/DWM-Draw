#include "draw/draw.h"
#include "dwm.h"
#include "misc/importer.hpp"
#include "misc/timer.hpp"
#include "imgui/direct10/imgui_impl_dx10.h"
#include <d3d10_1.h>


#define hr_fail_ret(ret) if(hr != S_OK){return ret;}
#define hr_fail_break if(hr != S_OK) {\
ERO_LOG("hr fail 0x%x",hr);\
break;\
}

namespace dwm {
    namespace win7 {

        function_ptr(VirtualAlloc);
        function_ptr(VirtualProtect);
        function_ptr(RtlAddVectoredExceptionHandler);
        function_ptr(RtlRemoveVectoredExceptionHandler);
        char* hook_fun_address = 0;
        char* hook_fun_ori_address = 0;
        DWORD hook_fun_memory_proct = 0;


        namespace dx10 {
            com_ptr<IDXGISwapChain> g_dxgiSwapChain;
            com_ptr<ID3D10Texture2D> g_back_buffer;
            com_ptr<ID3D10Device1>  g_d3dDevice;
            com_ptr<ID3D10RenderTargetView> g_mainRenderTargetView;
            com_ptr<ID3D10Texture2D>        g_AnitCaptureTexture2D;
            D3D10_TEXTURE2D_DESC back_buffer_desc;

            __int64 __fastcall my_present_hook(__int64 a1, __int64 a2, __int64 a3, __int64 a4, __int64 a5, __int64 a6, __int64 a7, __int64 a8) {


                static bool init = false;
                if (!init) {

                    do
                    {

                        IDXGISwapChain* swap_chain = (IDXGISwapChain*)a1;

                        auto hr = 0;

                        com_ptr<ID3D10Device1> d3dDevice;

                        TEMP_GUID(IID_ID3D10Device1, 0x9B7E4C8F, 0x342C, 0x4106, 0xA1, 0x9F, 0x4F, 0x27, 0x04, 0xF6, 0x89, 0xF0);

                        hr = swap_chain->GetDevice(IID_ID3D10Device1, &d3dDevice);

                        hr_fail_break;

                        

                        TEMP_GUID(IID_ID3D10Texture2D, 0x9B7E4C04, 0x342C, 0x4106, 0xA1, 0x9F, 0x4F, 0x27, 0x04, 0xF6, 0x89, 0xF0);

                        hr = swap_chain->GetBuffer(0, IID_ID3D10Texture2D,&g_back_buffer);

                        hr_fail_break;

                        
                        g_back_buffer->GetDesc(&back_buffer_desc);

                        MSG_LOG("Width %d , Height %d", back_buffer_desc.Width, back_buffer_desc.Height);

                        com_ptr<ID3D10RenderTargetView> main_render_target_view;
                        
                        hr = d3dDevice->CreateRenderTargetView(g_back_buffer.Get(), NULL, &main_render_target_view);

                        hr_fail_break;

                        ImGui::CreateContext();
                        auto& io = ImGui::GetIO();
                        io.DisplaySize.x = back_buffer_desc.Width;
                        io.DisplaySize.y = back_buffer_desc.Height;

                        if (!ImGui_ImplDX10_Init(d3dDevice.Get())) {
                            break;
                        }

                        if (!draw::init()) {
                            break;
                        }

                        //反截图
                        {
                            //创建欺骗纹理
                            D3D10_TEXTURE2D_DESC desc;
                            memset(&desc, 0x00, sizeof(desc));
                            g_back_buffer->GetDesc(&desc);
                            hr = d3dDevice->CreateTexture2D(&desc, 0, &(g_AnitCaptureTexture2D));
                            hr_fail_break;
                        }


                        g_dxgiSwapChain.Attach(swap_chain);
                        g_d3dDevice.Attach(d3dDevice.Get());
                        g_mainRenderTargetView = main_render_target_view;

                        MSG_LOG("win7 dx10 init success");

                        init = true;
                    } while (false);
                }

                if (init)
                {
                    g_d3dDevice->CopyResource(g_AnitCaptureTexture2D.Get(), g_back_buffer.Get());

                    ImGui_ImplDX10_NewFrame();
                    auto& io = ImGui::GetIO();
                    io.DisplaySize.x = back_buffer_desc.Width;
                    io.DisplaySize.y = back_buffer_desc.Height;

                    ImGui::NewFrame();
                    draw::draw_call();

                    ImGui::Render();


                    g_d3dDevice->OMSetRenderTargets(1, g_mainRenderTargetView.GetAddressOf(), NULL);
                    ImGui_ImplDX10_RenderDrawData(ImGui::GetDrawData());

                }

                __int64 ret_val = ((decltype(my_present_hook)*)hook_fun_ori_address)(a1, a2, a3, a4, a5, a6, a7, a8);

                if (init)
                {
                    g_d3dDevice->CopyResource(g_back_buffer.Get(), g_AnitCaptureTexture2D.Get());
                }


                return ret_val;
            }
        };



       


        LONG WINAPI veh_handler(EXCEPTION_POINTERS* pExceptionInfo)
        {
           
            DWORD64 page_start = ((DWORD64)(hook_fun_address)) & 0xFFFFFFFFFFFFF000;
            DWORD64 page_end = page_start + 0x1000;

            LONG result;
            if (pExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION) //捕获 PAGE_GUARD 异常
            {
                if ((pExceptionInfo->ContextRecord->Rip >= page_start) && (pExceptionInfo->ContextRecord->Rip <= page_end)) {

                    if (pExceptionInfo->ContextRecord->Rip == (DWORD64)(hook_fun_address)) {
                        IUnknown* pObject = (IUnknown*)(pExceptionInfo->ContextRecord->Rcx);

                        //安装hook
                        {
                            //获取原来的虚表
                            char* table = *(char**)(pObject);
                            //申请内存放置假虚表
                            auto cheat_table = VirtualAlloc(0, 0x200, MEM_COMMIT, PAGE_READWRITE);
                            memcpy(cheat_table, table, 0x200);
                            //安装假虚表
                            *(char**)(pObject) = (char*)cheat_table;
                            for (size_t i = 0; i < 0x200; i++)
                            {
                                //寻找当前hook的虚函数的指针
                                if (((char**)cheat_table)[i] == hook_fun_address) {
                                    ((char**)cheat_table)[i] = (char*)(& dx10::my_present_hook);
                                    hook_fun_ori_address = hook_fun_address;
                                    break;
                                }
                            }

                        }

                        return EXCEPTION_CONTINUE_EXECUTION;
                    }

                    pExceptionInfo->ContextRecord->EFlags |= 0x100;

                }

                result = EXCEPTION_CONTINUE_EXECUTION;
            }

            else if (pExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_SINGLE_STEP)
            {
                DWORD dwOld;
                VirtualProtect((LPVOID)hook_fun_address, 1, hook_fun_memory_proct | PAGE_GUARD, &dwOld);
                result = EXCEPTION_CONTINUE_EXECUTION;
            }

            else
            {
                result = EXCEPTION_CONTINUE_SEARCH;
            }

            return result;

        }

        DWORD init(LPVOID parameter) {

            MSG_LOG("win7 进入安装流程");



            VirtualAlloc = get_function_ptr("kernel32.dll", VirtualAlloc);
            VirtualProtect = get_function_ptr("kernel32.dll", VirtualProtect);
            RtlAddVectoredExceptionHandler = get_function_ptr("ntdll.dll", RtlAddVectoredExceptionHandler);
            RtlRemoveVectoredExceptionHandler = get_function_ptr("ntdll.dll", RtlRemoveVectoredExceptionHandler);
  
            if ((!VirtualAlloc)||(!VirtualProtect)||(!RtlAddVectoredExceptionHandler)||(!RtlRemoveVectoredExceptionHandler)) {
                MSG_LOG("%d行",__LINE__);
                return 0;

            }

            {
                quick_import_function("dxgi.dll", CreateDXGIFactory);

                GUID IID_IDXGIFactory = { 0x7b7166ec,0x21c7,0x44ae,0xb2,0x1a,0xc9,0xae,0x32,0x1a,0xe3,0x69 };
                //GUID IID_IDXGIFactory1 = { 0x770aae78,0xf26f,0x4dba,0xa8,0x29,0x25,0x3c,0x83,0xd1,0xb3,0x87 };
                
                com_ptr<IDXGIFactory>dxgiFactory;

                auto hr = CreateDXGIFactory(IID_IDXGIFactory, &dxgiFactory);

                if (hr != S_OK) {
                    MSG_LOG("%d行", __LINE__);
                    return 0;
                }

                com_ptr<IDXGIFactoryDWM>dxgiFactoryDWM;

                TEMP_GUID(IID_IDXGIFactoryDWM, 0x713F394E, 0x92CA, 0x47E7,0xAB,0x81,0x11,0x59,0xC2,0x79,0x1E,0x54);
                hr = dxgiFactory->QueryInterface(IID_IDXGIFactoryDWM, (void**)&dxgiFactoryDWM);

                if (hr != S_OK) {
                    MSG_LOG("%d行", __LINE__);
                    return 0;
                }





                //com_ptr < IDXGIAdapter1 > test_pAdapter;
                //hr = dxgiFactory->EnumAdapters1(1, &test_pAdapter);
                //if (hr != S_OK) {
                //    MSG_LOG("Adapters %d行 %x", __LINE__, hr);
                //    return 0;
                //}




                com_ptr<ID3D10Device1>d3d10Device;
                quick_import_function("d3d10_1.dll", D3D10CreateDevice1);

                UINT createDeviceFlags = 0;
                createDeviceFlags |= D3D10_CREATE_DEVICE_BGRA_SUPPORT;
                //createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
                hr = D3D10CreateDevice1(0, D3D10_DRIVER_TYPE_HARDWARE, 00, createDeviceFlags, D3D10_FEATURE_LEVEL_9_3, 0x20, &d3d10Device);

                if (hr != S_OK) {
                    MSG_LOG("%d行 %x", __LINE__, hr);
                    return 0;
                }

                com_ptr<IDXGIDevice>dxgiDevice;
                GUID IID_IDXGIDevice = { 0x54ec77fa, 0x1377, 0x44e6, 0x8c, 0x32, 0x88, 0xfd, 0x5f, 0x44, 0xc8, 0x4c };
                hr = d3d10Device->QueryInterface(IID_IDXGIDevice, (void**)&dxgiDevice);
                if (hr != S_OK) {
                    MSG_LOG("%d行", __LINE__);
                    return 0;
                }

                com_ptr<IDXGIAdapter>pAdapter;
                hr = dxgiDevice->GetAdapter(&pAdapter);
                if (hr != S_OK) {
                    MSG_LOG("%d行", __LINE__);
                    return 0;
                }
                com_ptr<IDXGIOutput>pOutput;
                hr = pAdapter->EnumOutputs(0, &pOutput);
                if (hr != S_OK) {
                    MSG_LOG("%d行", __LINE__);
                    return 0;
                }


                DXGI_SWAP_CHAIN_DESC sd;
                ZeroMemory(&sd, sizeof(sd));
                sd.BufferDesc.Width = 0;
                sd.BufferDesc.Width = 0;
                sd.BufferDesc.RefreshRate.Numerator = 0;
                sd.BufferDesc.RefreshRate.Denominator = 1;
                sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
                sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
                sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
                sd.SampleDesc.Count = 1;
                sd.SampleDesc.Quality = 0;
                sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER;
                sd.BufferCount = 3;
                sd.OutputWindow = nullptr;
                sd.Windowed = false;
                sd.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
                sd.Flags = DXGI_SWAP_CHAIN_FLAG_NONPREROTATED;


                com_ptr<IDXGISwapChainDWM>pSwapChainDWM;
                hr = dxgiFactoryDWM->CreateSwapChain(dxgiDevice.Get(), &sd, pOutput.Get(), &pSwapChainDWM);

                if (hr != S_OK) {
                    MSG_LOG("%d行", __LINE__);
                    return 0;
                }

                uintptr_t* vtable = *(uintptr_t**)(pSwapChainDWM.Get());

                uintptr_t present_address = vtable[8];
                hook_fun_address = (char*)present_address;

                //MSG_LOG("hook address %p", hook_fun_address);

                //查询属性
                MEMORY_BASIC_INFORMATION mem_info;
                memset(&mem_info, 0, sizeof(mem_info));
                VirtualQuery((LPCVOID)hook_fun_address, &mem_info, sizeof(mem_info));
                hook_fun_memory_proct = mem_info.Protect;
                
                //安装hook
                PVOID veh_hanle = RtlAddVectoredExceptionHandler(1, veh_handler);
                if (!veh_hanle) {
                    ERO_LOG("VEH ADD ERRO");
                }


                VirtualProtect((LPVOID)hook_fun_address, 1, mem_info.Protect | PAGE_GUARD, &hook_fun_memory_proct);

                quick_import_function("Kernel32.dll",Sleep);
              


                misc::timer::mill_recorder recorder;
                do
                {
                    Sleep(16);
                } while (!hook_fun_ori_address && (int64_t)recorder < 1000 * 5);

                //MSG_LOG("Remove Vectored Exception Handler");
                RtlRemoveVectoredExceptionHandler(veh_hanle);

            }



            return hook_fun_ori_address ? 1 : -1;
        }
    };

};