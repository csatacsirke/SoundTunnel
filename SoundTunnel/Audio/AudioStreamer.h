#pragma once

#include "AudioApi.h"

#include <Mfidl.h>
#pragma comment(lib, "Mf.lib")

#include <set>
#include <list>


class AudioStreamer {
	set<CComPtr<IMMDevice>> sinkDevices;
public:
	AudioStreamer() {
		HRESULT hr = Init();
		ASSERT(SUCCEEDED(hr));
	}

	HRESULT PlayFile(CString fileName) {
		HRESULT hr = S_OK;

		return hr;
	}

	void AddSinkDevice(CComPtr<IMMDevice> device) {
		// TODO okosan, mert felsorolásnál ha megjön ez akkor
		// invalidak lesznek az iterátorok
		sinkDevices.insert(device);
	}

	void RemoveSinkDevice(CComPtr<IMMDevice> device) {
		sinkDevices.erase(device);
	}

private:
	HRESULT Init() {
		HRESULT hr;

		hr = CoInitialize(NULL);
		EXIT_ON_ERROR(hr);


		CComPtr<IMMDeviceEnumerator> pEnumerator;
		hr = CoCreateInstance(
			CLSID_MMDeviceEnumerator, NULL,
			CLSCTX_ALL, IID_IMMDeviceEnumerator,
			(void**)&pEnumerator
		);
		EXIT_ON_ERROR(hr);


		// render 'hook' init

		CComPtr<IMMDevice> pDefaultRenderDevice;
		hr = pEnumerator->GetDefaultAudioEndpoint(
			eRender, eConsole, &pDefaultRenderDevice);
		EXIT_ON_ERROR(hr);

		this->sinkDevices.insert(pDefaultRenderDevice);

		// capture 'hook' init

		CComPtr<IMMDevice> pDefaultCaptureDevice;
		hr = pEnumerator->GetDefaultAudioEndpoint(
			eRender, eConsole, &pDefaultCaptureDevice);
		EXIT_ON_ERROR(hr);

		this->sinkDevices.insert(pDefaultCaptureDevice);

		return hr;
	}

};




