#pragma once


#include <Audioclient.h>
#include <Mmdeviceapi.h>

#include <Functiondiscoverykeys_devpkey.h>

#include <vector>
#include <memory>
#include <exception>
#include <thread>


using namespace std;

// https://msdn.microsoft.com/en-us/library/windows/desktop/dd370800(v=vs.85).aspx


class AudioApiException : public exception {

};


#define NO_AUDIO_API_EXCEPTIONS TRUE
#if NO_AUDIO_API_EXCEPTIONS

#define EXIT_ON_ERROR(hr) if(FAILED(hr)) {return hr;}

#else 

static void EXIT_ON_ERROR(HRESULT hr) {
	// lehet hogy <= nek kéne lennie de akkor az error_not_found nem jó
	// lehet hogy az lresult?  majd egyszer nyomozd ki

	if (FAILED(hr)) {
	//if (hr != 0) {
		ASSERT(0);
		throw AudioApiException();
	}
}

#endif


// TODO : ezt a példából másoltam, faszsetudja mi a jó
// update: minél kisebb annál kisebb a latency
// REFERENCE_TIME time units per second and per millisecond
#define REFTIMES_PER_SEC  (10000000/16)
#define REFTIMES_PER_MILLISEC  (REFTIMES_PER_SEC/1000)


//class AudioBuffer {
//	WAVEFORMATEX* format;
//
//
//};


class CPropVariant : public PROPVARIANT {
	
public:
	CPropVariant() {
		PropVariantInit(this);
	}

	~CPropVariant() {
		PropVariantClear(this);
	}

	

};

// TODO cpp file
namespace AudioApi {

	static HRESULT EnumerateDevices(std::vector<CComPtr<IMMDevice>>& devices) {

		HRESULT hr = S_OK;


		hr = CoInitialize(NULL);
		EXIT_ON_ERROR(hr);


		CComPtr<IMMDeviceEnumerator> pEnumerator;
		hr = CoCreateInstance(
			CLSID_MMDeviceEnumerator, NULL,
			CLSCTX_ALL, IID_IMMDeviceEnumerator,
			(void**)&pEnumerator
		);
		EXIT_ON_ERROR(hr);

		CComPtr<IMMDeviceCollection> deviceCollection;
		hr = pEnumerator->EnumAudioEndpoints(eAll, DEVICE_STATE_ACTIVE, &deviceCollection);
		EXIT_ON_ERROR(hr);


		UINT deviceCount;
		hr = deviceCollection->GetCount(&deviceCount);
		EXIT_ON_ERROR(hr);

		for (int i = 0; i < int(deviceCount); ++i) {
			CComPtr<IMMDevice> device;
			hr = deviceCollection->Item(i, &device);
			EXIT_ON_ERROR(hr);

			devices.push_back(device);

		}

		return hr;
	}


	static HRESULT GetFriendlyName(CComPtr<IMMDevice> device, CString& friendlyName) {
		HRESULT hr = S_OK;
		CComPtr<IPropertyStore> defaultDeviceProperties;

		hr = device->OpenPropertyStore(STGM_READ, &defaultDeviceProperties);
		EXIT_ON_ERROR(hr);


		//PROPVARIANT varFriendlyName;
		CPropVariant varFriendlyName;
		PropVariantInit(&varFriendlyName);
		//hr = ppProperties->GetValue(PKEY_DeviceInterface_FriendlyName, &varFriendlyName);
		hr = defaultDeviceProperties->GetValue(PKEY_Device_FriendlyName, &varFriendlyName);
		EXIT_ON_ERROR(hr);

		friendlyName = varFriendlyName.pwszVal;
		return hr;
		//return varFriendlyName.pwszVal;

	}

}





