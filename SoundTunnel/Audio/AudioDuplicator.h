#pragma once


#include "AudioApiUtils.h"

#include <Utils/Semaphore.h>

#include <thread>


class AudioDuplicator {
	CComPtr<IMMDevice> sourceDevice;
	CComPtr<IMMDevice> sinkDevice;


	volatile bool stop = false;
public:
	AudioDuplicator();
	~AudioDuplicator();

	// TODO ezt kitenni interface-re
	HRESULT InitDefaultDevices();

	
	HRESULT SetSourceDevice(CComPtr<IMMDevice> device);
	HRESULT SetSinkDevice(CComPtr<IMMDevice> device);

	HRESULT Run();
	HRESULT RunAsync();
	void Stop();
	void WaitForDestroy();
private:
	std::thread backgroundThread;
private:
	HRESULT SelectDevice(CComPtr<IMMDeviceEnumerator> pEnumerator, const CString& deviceNameSubstring, _Out_ CComPtr<IMMDevice>& device);
	HRESULT Init();
};





