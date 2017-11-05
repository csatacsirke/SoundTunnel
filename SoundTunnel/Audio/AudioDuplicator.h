#pragma once


#include "AudioApi.h"



class AudioDuplicator {
	CComPtr<IMMDevice> sourceDevice;
	CComPtr<IMMDevice> sinkDevice;


	volatile bool stop = false;
public:
	AudioDuplicator();

	
	HRESULT SetSourceDevice(CComPtr<IMMDevice> device);
	HRESULT SetSinkDevice(CComPtr<IMMDevice> device);

	HRESULT Run();
	void Stop();
private:

	HRESULT SelectDevice(CComPtr<IMMDeviceEnumerator> pEnumerator, CComPtr<IMMDevice>& device);
	HRESULT Init();
	HRESULT InitDefaultDevices();
};



static void DoEverythingLofasz() {
	try {
		shared_ptr<AudioDuplicator> recorder = make_shared<AudioDuplicator>();


		std::thread myThread([&recorder] {
			recorder->Run();

		});

		MessageBox(NULL, L"Press ok to stop", 0, MB_ICONASTERISK);
		recorder->Stop();
		myThread.join();


	}
	catch (AudioApiException) {
		MessageBox(0, L"Recorder exception", 0, 0);
	}



}





