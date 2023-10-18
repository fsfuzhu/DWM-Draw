#include "communication.h"
#include "misc/importer.hpp"
#include <wrl.h>
#include <d3d11.h>
#include <dxgi.h>
#include <cstdio>
//namespace api {
//
//    function_ptr(D3D11CreateDevice);
//    function_ptr(CreateDXGIFactory1);
//    
//};
namespace communication
{
    int64_t make_combination_handle(int64_t handle, int32_t device_id)
    {
        *((int*)(&handle) + 1) = (int)device_id;
        return handle;
    }

    void extract_combination_handle(int64_t combination, int64_t* handle, int32_t* device_id) {

        if(device_id)
            *device_id = *((int32_t*)(&combination) + 1);
        if (handle) {
            *handle = 0;
            uint32_t temp = *((uint32_t*)(&combination));
            *handle = static_cast<int64_t>(temp);
        }
    }

    bool buffer::update_data(void* buf, size_t buf_size)
    {
        if (buf_size > this->buffer_size) {
            return false;
        }


        D3D11_MAPPED_SUBRESOURCE mapped_resource{};
        auto hr = d3dDeviceContext->Map(pLocalTexture.Get(),0, D3D11_MAP_READ_WRITE, 0, &mapped_resource);
        if (hr == S_OK) {
            //printf("mapped_resource.pData      %p\n", mapped_resource.pData);
            //printf("mapped_resource.pData      %d\n", mapped_resource.RowPitch);
            //printf("mapped_resource.DepthPitch %d\n", mapped_resource.DepthPitch);
            if (buf && buf_size <= mapped_resource.DepthPitch) {
                memset(mapped_resource.pData,0, mapped_resource.DepthPitch);
                memcpy(mapped_resource.pData, buf, buf_size);
            }
            else
            {
                memset(mapped_resource.pData, 0, mapped_resource.DepthPitch);
            }

            d3dDeviceContext->Unmap(pLocalTexture.Get(), 0);
        }

        if (pMutex->AcquireSync(0, 1000) == S_OK) {
            d3dDeviceContext->CopyResource(pSharedTexture.Get(), pLocalTexture.Get());
            pMutex->ReleaseSync(0);
        }
        else
        {
            return false;
        }
        
        return true;
    }
    bool buffer::get_data(std::vector<int8_t>& data)
    {
        if (pMutex->AcquireSync(0, 1000) == S_OK) {
            d3dDeviceContext->CopyResource(pLocalTexture.Get(), pSharedTexture.Get());
            pMutex->ReleaseSync(0);
        }
        else
        {
            return false;
        }


        D3D11_MAPPED_SUBRESOURCE mapped_resource{};
        auto hr = d3dDeviceContext->Map(pLocalTexture.Get(), 0, D3D11_MAP_READ_WRITE, 0, &mapped_resource);
        if (hr == S_OK) {

            data.resize(mapped_resource.DepthPitch,0x00);
            memcpy(data.data(),mapped_resource.pData, mapped_resource.DepthPitch);

            d3dDeviceContext->Unmap(pLocalTexture.Get(), 0);
        }
        else
        {
            return false;
        }

        return true;
    }

    buffer* communica::create_buffer() {
    
       // DebugBreak();
        quick_import_function("d3d11.dll",D3D11CreateDevice);
        //quick_import_function("dxgi.dll",CreateDXGIFactory1);

        com_ptr < ID3D11Device > d3dDevice = 0;
        com_ptr < ID3D11DeviceContext > d3dDeviceContext = 0;
        UINT createDeviceFlags = 0;
        createDeviceFlags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;

        D3D_FEATURE_LEVEL featureLevel;
        const D3D_FEATURE_LEVEL featureLevelArray[4] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1,D3D_FEATURE_LEVEL_10_0 ,D3D_FEATURE_LEVEL_9_3 };

       
        auto hr = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 4, 7, &d3dDevice, &featureLevel, &d3dDeviceContext);

        if (hr != S_OK) {
            return nullptr;
        }


        com_ptr < IDXGIDevice > dxgiDevice = 0;
        hr = d3dDevice.As(&dxgiDevice);

        if (hr != S_OK) {
            return nullptr;
        }

        com_ptr < IDXGIAdapter > dxgiAdapter = 0;
        hr = dxgiDevice->GetAdapter(&dxgiAdapter);

        if (hr != S_OK) {
            return nullptr;
        }

        DXGI_ADAPTER_DESC adapter_desc{};
        hr = dxgiAdapter->GetDesc(&adapter_desc);

        if (hr != S_OK) {
            return nullptr;
        }

        D3D11_TEXTURE2D_DESC share_desc;
        share_desc.Width = 512;
        share_desc.Height = 512;
        share_desc.MipLevels = 1;
        share_desc.ArraySize = 1;
        share_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        share_desc.SampleDesc.Count = 1;
        share_desc.SampleDesc.Quality = 0;
        share_desc.Usage = D3D11_USAGE_DEFAULT;
        share_desc.BindFlags = 40;
        share_desc.CPUAccessFlags = 0;
        share_desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;

        com_ptr < ID3D11Texture2D > pSharedTexture = 0;
        hr = d3dDevice->CreateTexture2D(&share_desc, NULL, &pSharedTexture);
    
        if (hr != S_OK) {
            return nullptr;
        }


        D3D11_TEXTURE2D_DESC local_desc = share_desc;
        local_desc.Usage = D3D11_USAGE_STAGING;
        local_desc.BindFlags = 0;
        local_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
        local_desc.MiscFlags = 0;
        com_ptr < ID3D11Texture2D > pLocalTexture = 0;
        hr = d3dDevice->CreateTexture2D(&local_desc, NULL, &pLocalTexture);
        if (hr != S_OK) {
            return nullptr;
        }

    
        com_ptr < IDXGIResource > pResource = 0;
        hr = pSharedTexture.As(&pResource);

        if (hr != S_OK) {
            return nullptr;
        }

        HANDLE SharedTextureHandle = 0;
        hr = pResource->GetSharedHandle(&(SharedTextureHandle));

        if (hr != S_OK) {
            return nullptr;
        }

        com_ptr < IDXGIKeyedMutex > pSharedKeyedMutex = 0;
        hr = pResource.As(&pSharedKeyedMutex);

        if (hr != S_OK) {
            return nullptr;
        }


        buffer* ret = new buffer();

        ret->buffer_size = share_desc.Width * share_desc.Height * 4;
        ret->shared_handle = SharedTextureHandle;
        ret->d3dDevice = d3dDevice;
        ret->d3dDeviceContext = d3dDeviceContext;
        ret->pMutex = pSharedKeyedMutex;
        ret->pSharedTexture = pSharedTexture;
        ret->pLocalTexture = pLocalTexture;
        ret->combination_handle = make_combination_handle((int64_t)SharedTextureHandle, adapter_desc.DeviceId);

        return ret;
    }
   

    buffer* communica::open_buffer(int64_t combination_handle)
    {
        if (!combination_handle) {
            return nullptr;
        }

        quick_import_function("d3d11.dll", D3D11CreateDevice);
        quick_import_function("dxgi.dll", CreateDXGIFactory1);

        HANDLE shared_handle = 0;
        UINT   device_id = 0;

        extract_combination_handle(combination_handle,(int64_t*)&shared_handle,(int32_t*)&device_id);
        

        com_ptr< IDXGIFactory1 > pFactory = nullptr;
        com_ptr< IDXGIAdapter1 > pRecommendedAdapter = nullptr;

        GUID IID_IDXGIFactory1 = { 0x770aae78, 0xf26f, 0x4dba, 0xa8, 0x29, 0x25, 0x3c, 0x83, 0xd1, 0xb3, 0x87 };
        auto hr = CreateDXGIFactory1(IID_IDXGIFactory1, &pFactory);
		com_ptr < IDXGIAdapter1 > pAdapter;
		
		pFactory->EnumAdapters1(0, &pAdapter);
		DXGI_ADAPTER_DESC1 desc;
		pAdapter->GetDesc1(&desc);
		pRecommendedAdapter = pAdapter;

		if (SUCCEEDED(hr)) {
			com_ptr < IDXGIAdapter1 > pAdapter;
			UINT index = 0;
            while (pFactory->EnumAdapters1(index, &pAdapter) != DXGI_ERROR_NOT_FOUND) {
                DXGI_ADAPTER_DESC1 desc;
                pAdapter->GetDesc1(&desc);
                if (desc.DeviceId == device_id) {
                    pRecommendedAdapter = pAdapter;
                    break;
                }
              
                index++;
            }
        }
        else
        {
            return nullptr;
        }


        if (!pRecommendedAdapter) {
            return nullptr;
        }
    


        com_ptr < ID3D11Device > d3dDevice = 0;
        com_ptr < ID3D11DeviceContext > d3dDeviceContext = 0;


        UINT createDeviceFlags = 0;
        createDeviceFlags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;


        D3D_FEATURE_LEVEL featureLevel;
        const D3D_FEATURE_LEVEL featureLevelArray[4] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1,D3D_FEATURE_LEVEL_10_0 ,D3D_FEATURE_LEVEL_9_3 };
        hr = D3D11CreateDevice(pRecommendedAdapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, NULL, createDeviceFlags, featureLevelArray, 4, 7, &d3dDevice, &featureLevel, &d3dDeviceContext);
        
        if (hr != S_OK) {
            return nullptr;
        }


        com_ptr < IDXGIResource > pResource = 0;
        GUID IID_IDXGIResource = { 0x035f3ab4, 0x482e, 0x4e50, 0xb4, 0x1f, 0x8a, 0x7f, 0x8b, 0xd8, 0x96, 0x0b };
        hr = d3dDevice->OpenSharedResource(shared_handle, IID_IDXGIResource, &pResource);
        if (hr != S_OK) {
            return nullptr;
        }

        com_ptr < ID3D11Texture2D >pSharedTexture = 0;
        hr = pResource.As(&pSharedTexture);

        if (hr != S_OK) {
            return nullptr;
        }

        com_ptr < IDXGIKeyedMutex > pMutex = 0;
        hr = pResource.As(&pMutex);

        if (hr != S_OK) {
            return nullptr;
        }


        D3D11_TEXTURE2D_DESC share_desc;
        pSharedTexture->GetDesc(&share_desc);

        D3D11_TEXTURE2D_DESC local_desc = share_desc;
        local_desc.Usage = D3D11_USAGE_STAGING;
        local_desc.BindFlags = 0;
        local_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
        local_desc.MiscFlags = 0;
        com_ptr < ID3D11Texture2D > pLocalTexture = 0;
        hr = d3dDevice->CreateTexture2D(&local_desc, NULL, &pLocalTexture);
        if (hr != S_OK) {
            return nullptr;
        }

        buffer* ret = new buffer();

        ret->buffer_size = share_desc.Width * share_desc.Height * 4;
        ret->shared_handle = shared_handle;
        ret->d3dDevice = d3dDevice;
        ret->d3dDeviceContext = d3dDeviceContext;
        ret->pMutex = pMutex;
        ret->pSharedTexture = pSharedTexture;
        ret->pLocalTexture = pLocalTexture;
        ret->combination_handle = combination_handle;


        return ret;
    }
   

};