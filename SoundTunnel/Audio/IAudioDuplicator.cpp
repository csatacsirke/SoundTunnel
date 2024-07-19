#include <stdafx.h>

#include "IAudioDuplicator.h"
#include "AudioDuplicator.h"
#include "AudioApi.h"

std::shared_ptr<IAudioDuplicator> CreateAudioDuplicator(IMMDevice* sourceDevice, IMMDevice* sinkDevice)
{
	return std::make_shared<AudioDuplicator>(sourceDevice, sinkDevice);
}




std::shared_ptr<Devices> EnumerateDevices() {

	HRESULT hr = S_OK;

	CComPtr<IMMDeviceEnumerator> pEnumerator;
	hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator, NULL,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)&pEnumerator
	);

	if (FAILED(hr)) {
		return nullptr;
	}


	// render 'hook' init

	CComPtr<IMMDevice> pDefaultDevice;
	hr = pEnumerator->GetDefaultAudioEndpoint(
		eRender, eConsole, &pDefaultDevice);

	if (FAILED(hr)) {
		return nullptr;
	}

	CComHeapPtr<WCHAR> defaultDeviceId;
	pDefaultDevice->GetId(&defaultDeviceId);

	Devices devices;
	devices.defaultDevice = pDefaultDevice;


	AudioApi::EnumerateDevices(devices.devices, EDataFlow::eRender);
	if (FAILED(hr)) {
		return nullptr;
	}

	return std::make_shared<Devices>(std::move(devices));
}
