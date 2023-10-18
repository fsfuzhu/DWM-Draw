#pragma once
#include <Windows.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <wrl.h>
#include "misc/importer.hpp"
namespace dwm
{
    template <typename T>
    using com_ptr = Microsoft::WRL::ComPtr<T>;

    extern PVOID NTAPI RtlAddVectoredExceptionHandler(IN ULONG FirstHandler, IN PVECTORED_EXCEPTION_HANDLER VectoredHandler);
    extern ULONG NTAPI RtlRemoveVectoredExceptionHandler(IN PVOID VectoredHandlerHandle);
    extern int __CRTDECL sprintf_s(
        _Out_writes_(_BufferCount) _Always_(_Post_z_) char* const _Buffer,
        _In_                                          size_t      const _BufferCount,
        _In_z_ _Printf_format_string_                 char const* const _Format,
        ...);


    template <typename... Args>
    void R3DbgPrint(bool is_erro, const char* format, Args... args)
    {
        static_function_ptr(OutputDebugStringA);
        static_function_ptr(sprintf_s);
        static_function_ptr(strlen);

        if (!OutputDebugStringA) {
            OutputDebugStringA = get_function_ptr("kernel32.dll", OutputDebugStringA);
        }
        if (!sprintf_s) {
            sprintf_s = get_function_ptr("msvcrt.dll", sprintf_s);
        }
        if (!strlen) {
            strlen = get_function_ptr("msvcrt.dll", strlen);
        }

        char buffer[2500] = { '\0' };
        sprintf_s(buffer, 2500, "[¡Þ] [%s] [dwm] ", is_erro ? "!" : "+");
        sprintf_s(buffer + strlen(buffer), 2500 - strlen(buffer), format, args...);
        sprintf_s(buffer + strlen(buffer), 2500 - strlen(buffer), "\n");
        OutputDebugStringA(buffer);

    }

    //constexpr GUID IID_IDXGISwapChainDWM = { 0xF69F223B,0x45D3,0x4AA0,{0x98,0xC8,0xC4,0x0C,0x2B,0x23,0x10,0x29} };
    //constexpr GUID IID_IDXGIFactoryDWM   = { 0x713F394E,0x92CA,0x47E7,{0xAB,0x81,0x11,0x59,0xC2,0x79,0x1E,0x54} };
    //constexpr GUID IID_IDXGIFactoryDWM8  = { 0x1DDD77AA, 0x9A4Au,0x4CC8,0x9Eu,0x55,0x98u,0xC1u,0x96u,0xBAu,0xFCu,0x8Fu };

    struct IDXGISwapChainDWM : public IDXGIDeviceSubObject
    {
       
        virtual HRESULT STDMETHODCALLTYPE Present(UINT SyncInterval, UINT Flags) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetBuffer(UINT Buffer, REFIID riid, void** ppSurface) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetDesc(DXGI_SWAP_CHAIN_DESC* pDesc) = 0;
        virtual HRESULT STDMETHODCALLTYPE ResizeBuffers(UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) = 0;
        virtual HRESULT STDMETHODCALLTYPE ResizeTarget(const DXGI_MODE_DESC* pNewTargetParameters) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetContainingOutput(IDXGIOutput** ppOutput) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetFrameStatistics(DXGI_FRAME_STATISTICS* pStats) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetLastPresentCount(UINT* pLastPresentCount) = 0;

        virtual HRESULT STDMETHODCALLTYPE SetFullscreenState(BOOL Fullscreen, IDXGIOutput* pTarget) = 0;
        virtual HRESULT STDMETHODCALLTYPE GetFullscreenState(BOOL* pFullscreen, IDXGIOutput** ppTarget) = 0;
    };

    struct IDXGIFactoryDWM : public IUnknown
    {
        

        virtual HRESULT STDMETHODCALLTYPE CreateSwapChain(IUnknown* pDevice,
            DXGI_SWAP_CHAIN_DESC* pDesc, IDXGIOutput* pOutput,
            IDXGISwapChainDWM** ppSwapChain) = 0;
    };


    struct IDXGISwapChainDWM8 : public IDXGIObject
    {
        virtual HRESULT STDMETHODCALLTYPE GetDevice(
            REFIID riid,
            void** ppvObject
        ) = 0;

        virtual HRESULT STDMETHODCALLTYPE Present(
            /* [annotation][in] */
            _In_  UINT SyncInterval,
            /* [annotation][in] */
            _In_  UINT Flags
        ) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetBuffer(
            /* [annotation][in] */
            _In_  UINT Buffer,
            /* [annotation][in] */
            _In_  REFIID riid,
            /* [annotation][retval][out] */
            _Out_  void** ppSurface) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetDesc(
            /* [annotation][retval][out] */
            _Out_  DXGI_SWAP_CHAIN_DESC* pDesc
        ) = 0;

        virtual HRESULT STDMETHODCALLTYPE ResizeBuffers(
            /* [annotation][in] */
            _In_ UINT BufferCount,
            /* [annotation][in] */
            _In_ UINT Width,
            /* [annotation][in] */
            _In_ UINT Height,
            /* [annotation][in] */
            _In_ DXGI_FORMAT NewFormat,
            /* [annotation][in] */
            _In_ UINT SwapChainFlags
        ) = 0;

        virtual HRESULT STDMETHODCALLTYPE ResizeTarget(
            /* [annotation][in] */
            _In_  const DXGI_MODE_DESC* pNewTargetParameters) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetContainingOutput(
            /* [annotation][retval][out] */
            _Out_ IDXGIOutput1** ppOutput) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetFrameStatistics(
            /* [annotation][retval][out] */
            _Out_ DXGI_FRAME_STATISTICS* pStats) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetLastPresentCount(
            /* [annotation][retval][out] */
            _Out_ UINT* pLastPresentCount) = 0;

        virtual HRESULT STDMETHODCALLTYPE Present1( //equals to : http://msdn.microsoft.com/en-us/library/windows/desktop/hh446797%28v=vs.85%29.aspx
            /* [annotation][in] */
            _In_  UINT SyncInterval,
            /* [annotation][in] */
            _In_  UINT Flags,
            /* [annotation][in] */
            _In_  UINT  DirtyRectsCount,
            /* [annotation][in] */
            _In_  RECT* pDirtyRects,
            /* [annotation][in] */
            _In_  RECT* pScrollRect,
            /* [annotation][in] */
            _In_  POINT* pScrollOffset
        ) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetLogicalSurfaceHandle(
            LPVOID pVoid
        ) = 0;

        //
        //Called by the Desktop Window Manager (DWM) to verify that the user-mode driver supports Direct Flip operations, in which video memory is seamlessly flipped between an application's 
        //managed primary allocations and the DWM's managed primary allocations.
        //
        virtual HRESULT STDMETHODCALLTYPE CheckDirectFlipSupport(
            ULONG uLong,
            IDXGIResource* pResource,
            PULONG puLong
        ) = 0;

        virtual HRESULT STDMETHODCALLTYPE Present2(
            /* [annotation][in] */
            _In_  UINT SyncInterval,
            /* [annotation][in] */
            _In_  UINT Flags,
            /* [annotation][in] */
            _In_  UINT  DirtyRectsCount,
            /* [annotation][in] */
            _In_  RECT* pDirtyRects,
            /* [annotation][in] */
            _In_  RECT* pScrollRect,
            /* [annotation][in] */
            _In_  POINT* pScrollOffset,
            /* [annotation][in] */
            _In_  IDXGIResource* pResource
        ) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetCompositionSurface(
            LPVOID* ppVoid
        ) = 0;
    };
    struct IDXGIFactoryDWM8 : public IUnknown
    {
       

        virtual HRESULT STDMETHODCALLTYPE CreateSwapChainDWM(
            /* [annotation][in] */
            _In_ IUnknown* pDevice,
            /* [annotation][in] */
            _In_ DXGI_SWAP_CHAIN_DESC1* pSwapChainDesc1,
            /* [annotation][in] */
            _In_ DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pSwapChainFullScreenDesc1,
            /* [annotation][in] */
            _In_ IDXGIOutput* pOutput,
            /* [annotation][out] */
            _Out_ IDXGISwapChainDWM8** ppSwapChainDWM1
        ) = 0;

        virtual HRESULT STDMETHODCALLTYPE CreateSwapChainDDA(
            /* [annotation][in] */
            _In_ IUnknown* pDevice,
            /* [annotation][in] */
            _In_ DXGI_SWAP_CHAIN_DESC1* pSwapChainDesc1,
            /* [annotation][in] */
            _In_ IDXGIOutput* pOutput,
            /* [annotation][out] */
            _Out_ IDXGISwapChainDWM8** ppSwapChainDWM8
        ) = 0;
    };




    namespace win7 {
    
        DWORD init(LPVOID parameter);
    
    };
    namespace win10 {

        DWORD init(LPVOID parameter);

    };

















};

#ifdef DEBUG
#define MSG_LOG(...) dwm::R3DbgPrint(false,__VA_ARGS__)
#define ERO_LOG(...) dwm::R3DbgPrint(true ,__VA_ARGS__)
#else
#define MSG_LOG(...) 
#define ERO_LOG(...) 
#endif // _DEBUG

#define TEMP_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)\
GUID name = {l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8};