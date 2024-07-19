#pragma once

#include "IAudioDuplicator.h"
#include "AudioApiUtils.h"



class AudioDuplicator  : public IAudioDuplicator {
	CComPtr<IMMDevice> m_sourceDevice;
	CComPtr<IMMDevice> m_sinkDevice;


	//std::atomic_bool stop = false;
	CEvent m_cancellationEvent;
public:
	AudioDuplicator(IMMDevice* sourceDevice, IMMDevice* sinkDevice);
	~AudioDuplicator();


	bool Run() override;
	void Stop() override;

	// TODO ezt kitenni interface-re
	//HRESULT InitDefaultDevices();

	
	//HRESULT SetSourceDevice(CComPtr<IMMDevice> device);
	//HRESULT SetSinkDevice(CComPtr<IMMDevice> device);

	//HRESULT RunAsync();
private:
	HRESULT RunInternal();
	void StopInternal();

	HRESULT Init();
};




