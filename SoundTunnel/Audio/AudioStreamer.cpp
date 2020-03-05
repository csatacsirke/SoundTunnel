#include "stdafx.h"
#include "AudioStreamer.h"

AudioStreamer::AudioStreamer() {
	HRESULT hr = Init();
	ASSERT(SUCCEEDED(hr));
}

HRESULT AudioStreamer::PlayFile(CString fileName) {
	HRESULT hr = S_OK;

	AudioFile audioFile;

	hr = audioFile.Open(fileName);
	EXIT_ON_ERROR(hr);


	list<future<HRESULT>> threadResults;
	for (AudioSinkDevice& device : sinkDevices) {
		//auto fnc = std::bind(&PlayFileOnDevice, this, audioFile, device);
		future<HRESULT> result = std::async(&AudioStreamer::PlayFileOnDevice, this, audioFile, device);
		threadResults.push_back(std::move(result));
	}

	for (future<HRESULT>& threadResult_packed : threadResults) {
		threadResult_packed.wait();
		HRESULT threadResult = threadResult_packed.get();
		if (FAILED(threadResult)) {
			hr = threadResult;
		}
	}

	return hr;
}

HRESULT AudioStreamer::PlayFileOnDevice(AudioFile & audioFile, AudioSinkDevice & device) {
	HRESULT hr = S_OK;


	const auto& waveFormat = audioFile.GetWaveFormat();
	const auto& data = audioFile.GetData();

	//size_t chunkSize = 1 / 0; // TODO
	size_t chunkSize = CalculateChunkSize(waveFormat);
	BufferIterator bufferIterator(data);

	hr = device.Create(&waveFormat);
	EXIT_ON_ERROR(hr);

	//for (auto& device : sinkDevices) {
	//	hr = device.Create(&waveFormat);
	//	EXIT_ON_ERROR(hr);
	//}

	// TODO osztályvaltozo  + stop fv
	volatile bool stop = false;
	while (!stop && bufferIterator.HasNext()) {

		//const Chunk chunk = bufferIterator.NextChunk();
		//hr = device.AddSamples(chunk.ptr, chunk.size, &waveFormat);
		//hr = device.AddSamples(bufferIterator, &waveFormat);
		//hr = device.AddSamples(data.data(), data.size(), &waveFormat);
		hr = device.AddSamples(bufferIterator);
		EXIT_ON_ERROR(hr);



		device.WaitUntilBufferIsAvailable();
		EXIT_ON_ERROR(hr);

		//REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
		////DWORD hnsActualDuration = DWORD(double(REFTIMES_PER_SEC) * bufferFrameCount / pwfx->nSamplesPerSec);
		//DWORD hnsActualDuration = hnsRequestedDuration;
		//Sleep(hnsActualDuration / REFTIMES_PER_MILLISEC / 2);

	}

	device.WaitUntilPlaybackEnd();
	EXIT_ON_ERROR(hr);



	return hr;
}

void AudioStreamer::AddSinkDevice(CComPtr<IMMDevice> device) {
	// TODO okosan, mert felsorolásnál ha megjön ez akkor
	// invalidak lesznek az iterátorok

	// csak simán kéne ide egy mutex meg a playfilebe
	//sinkDevices.insert(device);
	sinkDevices.push_back(device);
}

HRESULT AudioStreamer::AddDefaultDevices() {
	HRESULT hr;

	CComPtr<IMMDeviceEnumerator> pEnumerator;
	hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator, NULL,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)&pEnumerator
	);
	EXIT_ON_ERROR(hr);


	// render 'hook' init
#if 0
	CComPtr<IMMDevice> pDefaultRenderDevice;
	hr = pEnumerator->GetDefaultAudioEndpoint(
		eRender, eConsole, &pDefaultRenderDevice);
	EXIT_ON_ERROR(hr);

	//this->sinkDevices.insert(pDefaultRenderDevice);
	AddSinkDevice(pDefaultRenderDevice);
#endif
	// capture 'hook' init

	CComPtr<IMMDevice> pDefaultCaptureDevice;
	hr = pEnumerator->GetDefaultAudioEndpoint(
		eRender, eConsole, &pDefaultCaptureDevice);
	EXIT_ON_ERROR(hr);

	//this->sinkDevices.insert(pDefaultCaptureDevice);
	AddSinkDevice(pDefaultCaptureDevice);

	return hr;
}

HRESULT AudioStreamer::Init() {
	HRESULT hr;

	hr = CoInitialize(NULL);
	EXIT_ON_ERROR(hr);

	return hr;
}

size_t AudioStreamer::CalculateChunkSize(const WAVEFORMATEX & waveFormat) {
	const REFERENCE_TIME hnsRequestedDuration = BUFFER_DURATION_HNS;
	const double length_seconds = double(hnsRequestedDuration) / double(__REFTIMES_PER_SEC);
	const double samplesPerSec = waveFormat.nSamplesPerSec;
	const int blockSize = waveFormat.nBlockAlign;
	const size_t chunkSize = size_t(length_seconds * samplesPerSec) * blockSize;

	return chunkSize;
}
