#pragma once


#include "AudioApiUtils.h"



class AudioDuplicator {
	CComPtr<IMMDevice> m_sourceDevice;
	CComPtr<IMMDevice> m_sinkDevice;


	//std::atomic_bool stop = false;
	CEvent m_cancellationEvent;
public:
	AudioDuplicator(IMMDevice* sourceDevice, IMMDevice* sinkDevice);
	~AudioDuplicator();

	// TODO ezt kitenni interface-re
	HRESULT InitDefaultDevices();

	
	//HRESULT SetSourceDevice(CComPtr<IMMDevice> device);
	//HRESULT SetSinkDevice(CComPtr<IMMDevice> device);

	HRESULT Run();
	//HRESULT RunAsync();
	void Stop();
private:
	HRESULT Init();
};





