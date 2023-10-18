#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <wrl.h>
#include <vector>
namespace communication
{
	template <typename T>
	using com_ptr = Microsoft::WRL::ComPtr<T>;

	class communica;
	class buffer
	{
		friend class communica;
	public:
		
		~buffer() {}
		size_t size()const {
			return buffer_size;
		}
		int64_t get_combination_handle()const {
			return combination_handle;
		}
		HANDLE get_shared_handle()const {
			return shared_handle;
		}
		bool update_data(void* buf,size_t buf_size);
		bool get_data(std::vector<int8_t>& data);


	private:
		buffer() {}

		com_ptr < ID3D11Device > d3dDevice = 0;
		com_ptr < ID3D11DeviceContext > d3dDeviceContext = 0;
		com_ptr < ID3D11Texture2D > pSharedTexture = 0;
		com_ptr < ID3D11Texture2D > pLocalTexture = 0;
		com_ptr < IDXGIKeyedMutex > pMutex = 0;
		size_t buffer_size = 0;

		int64_t combination_handle = 0;
		HANDLE shared_handle = 0;
	};

	class communica {
	public:
		static buffer* create_buffer();
		static buffer* open_buffer(int64_t combination_handle);
	};

};

