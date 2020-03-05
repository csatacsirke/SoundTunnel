#pragma once

#include "AudioApiUtils.h"
#include "AudioFileHandler.h"
#include "AudioSinkDevice.h"

#include <Mfidl.h>
#pragma comment(lib, "Mf.lib")

//#include <set>
#include <vector>
#include <list>
//#include <thread>
#include <future>



class AudioStreamer {
	
	std::list<AudioSinkDevice> sinkDevices;
public:
	AudioStreamer();

	HRESULT PlayFile(CString fileName);

	HRESULT PlayFileOnDevice(AudioFile& audioFile, AudioSinkDevice& device);

	void AddSinkDevice(CComPtr<IMMDevice> device);

	HRESULT AddDefaultDevices();

private:
	HRESULT Init();

	size_t CalculateChunkSize(const WAVEFORMATEX& waveFormat);
};




