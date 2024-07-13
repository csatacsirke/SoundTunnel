#pragma once

#include "AudioApiUtils.h"



class AudioSinkDevice {
	//CComPtr<IMMDevice> device
	CComPtr<IAudioClient> sinkAudioClient;
	CComPtr<IAudioRenderClient> sinkAudioRenderClient;
	CComPtr<IMMDevice> m_sinkDevice;
	CHandle eventHandle;
	//WAVEFORMATEX waveFormat;
	int blockAlign; // frame size in bytes

	CString friendlyDeviceName;
public:
	AudioSinkDevice(CComPtr<IMMDevice> m_sinkDevice);

	~AudioSinkDevice();

	HRESULT Create(const WAVEFORMATEX* pwfx);

	HRESULT AddSamples(BufferIterator& bufferIterator);

	HRESULT Destroy();

	HRESULT WaitUntilBufferIsAvailable(DWORD timeout_millisecs = 2000);

	HRESULT WaitUntilPlaybackEnd(DWORD timeout_millisecs = 2000);



};


