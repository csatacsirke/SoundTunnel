#pragma once


#include <Audioclient.h>
#include <Mmdeviceapi.h>

#include <Functiondiscoverykeys_devpkey.h>

#include <memory>
#include <exception>
#include <thread>


using namespace std;

// https://msdn.microsoft.com/en-us/library/windows/desktop/dd370800(v=vs.85).aspx

class AudioApiException : public exception {

};

static void EXIT_ON_ERROR(HRESULT hr) {
	// lehet hogy <= nek kéne lennie de akkor az error_not_found nem jó
	// lehet hogy az lresult?  majd egyszer nyomozd ki

	if (FAILED(hr)) {
	//if (hr != 0) {
		ASSERT(0);
		throw AudioApiException();
	}
}


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

static CString GetFriendlyName(CComPtr<IMMDevice> device) {
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

	return varFriendlyName.pwszVal;

}

static const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
static const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
static const IID IID_IAudioClient = __uuidof(IAudioClient);
static const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);
static const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);






