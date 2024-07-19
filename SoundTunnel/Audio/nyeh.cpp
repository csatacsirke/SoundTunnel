
#include <Audio/IAudioDuplicator.h>


#include <atlbase.h>
#include "AudioApiUtils.h"

using namespace System;
using namespace System::Collections::Generic;

namespace Bch {


	public ref class BCAudioDevice {
	private:
		IMMDevice* m_device;
	public:

		BCAudioDevice(IMMDevice* device) : m_device(device) {
			device->AddRef();
		}

		~BCAudioDevice() {
			m_device->Release();
		}

		String^ GetName() {

			HRESULT hr = S_OK;
			CComPtr<IPropertyStore> defaultDeviceProperties;

			hr = m_device->OpenPropertyStore(STGM_READ, &defaultDeviceProperties);
			if (FAILED(hr)) {
				return gcnew String(L"");
			}


			CPropVariant varFriendlyName;
			PropVariantInit(&varFriendlyName);

			hr = defaultDeviceProperties->GetValue(PKEY_Device_FriendlyName, &varFriendlyName);
			if (FAILED(hr)) {
				return gcnew String(L"");
			}

			return gcnew String(varFriendlyName.pwszVal);
		}

		IMMDevice* GetNativeDevice() {
			return m_device;
		}

	};

	template<class T>
	public ref class SharedPtr {
		std::shared_ptr<T>* m_ptr;
	public:
		SharedPtr(std::shared_ptr<T> ptr) {
			m_ptr = new std::shared_ptr<T>(ptr);
		}

		T* operator -> () {
			return m_ptr->get();
		}
	};

	public ref class BCSoundTunnel
	{
		
		SharedPtr<IAudioDuplicator>^ m_audioDuplicator;
		BCAudioDevice^ m_sourceDevice;
		BCAudioDevice^ m_sinkDevice;
	public:
		BCSoundTunnel(BCAudioDevice^ source, BCAudioDevice^ sink) 
			: m_sourceDevice(source), m_sinkDevice(sink)
		{
			// void
			std::shared_ptr<IAudioDuplicator> audioDuplicator = CreateAudioDuplicator(source->GetNativeDevice(), sink->GetNativeDevice());
			m_audioDuplicator = gcnew SharedPtr<IAudioDuplicator>(audioDuplicator);
		}

		bool Run() {
			return m_audioDuplicator->Run();
		}

		void Stop() {
			m_audioDuplicator->Stop();
		}


		static BCAudioDevice^ GetDefaultDevice() {
			auto devices = ::EnumerateDevices();
			return gcnew BCAudioDevice(devices->defaultDevice);
		}

		static List<BCAudioDevice^>^ EnumerateDevices() {
			auto devices = ::EnumerateDevices();

			List<BCAudioDevice^>^ list = gcnew List<BCAudioDevice^>(0);
			for (auto& device : devices->devices) {
				list->Add(gcnew BCAudioDevice(device));
			}
			
			return list;
		}

	};

}
