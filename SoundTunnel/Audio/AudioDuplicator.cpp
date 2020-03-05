#include "stdafx.h"

#include "AudioDuplicator.h"
#include "AudioApi.h"

#include <set>
// TODO lehetne hozzá gui-t írni



static const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
static const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
static const IID IID_IAudioClient = __uuidof(IAudioClient);
static const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);
static const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);


using namespace AudioApi;


AudioDuplicator::AudioDuplicator() {

	Init();

}

AudioDuplicator::~AudioDuplicator() {
	Stop();
}

HRESULT AudioDuplicator::Init() {

	HRESULT hr;

	// Common init 
	hr = CoInitialize(NULL);
	EXIT_ON_ERROR(hr);

	

	//hr = InitDefaultDevices();
	//EXIT_ON_ERROR(hr);


	return hr;
}

HRESULT AudioDuplicator::SetSourceDevice(CComPtr<IMMDevice> device) {
	this->sourceDevice = device;
	return S_OK;
}

HRESULT AudioDuplicator::SetSinkDevice(CComPtr<IMMDevice> device) {
	this->sinkDevice = device;
	return S_OK;
}



HRESULT AudioDuplicator::Run() {

	HRESULT hr;

	// Notify the audio sink which format to use.
	//hr = pMySink->SetFormat(pwfx);
	//EXIT_ON_ERROR(hr);





	CComPtr<IAudioClient> pSourceAudioClient;
	hr = sourceDevice->Activate(IID_IAudioClient, CLSCTX_ALL, nullptr, (void**)&pSourceAudioClient);
	EXIT_ON_ERROR(hr);


	// TODO ezt amugy releaselni kéne... de hát a faszom...
	//WAVEFORMATEX* pwfx = nullptr;
	CComHeapPtr<WAVEFORMATEX> pwfx;
	hr = pSourceAudioClient->GetMixFormat(&pwfx);
	EXIT_ON_ERROR(hr);

	// HSN: HUNDRED NANO SECONDS
	REFERENCE_TIME hnsRequestedDuration = BUFFER_DURATION_HNS;
	hr = pSourceAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, hnsRequestedDuration, 0, pwfx, NULL);
	EXIT_ON_ERROR(hr);


	// Get the size of the allocated buffer.
	UINT32 bufferFrameCount;
	hr = pSourceAudioClient->GetBufferSize(&bufferFrameCount);
	EXIT_ON_ERROR(hr);

	CComPtr<IAudioCaptureClient> pCaptureClient;
	hr = pSourceAudioClient->GetService(
		IID_IAudioCaptureClient,
		(void**)&pCaptureClient);
	EXIT_ON_ERROR(hr);

	// init renderer instance


	CComPtr<IAudioClient> pDestinationAudioClient;
	hr = sinkDevice->Activate(IID_IAudioClient, CLSCTX_ALL, nullptr, (void**)&pDestinationAudioClient);
	EXIT_ON_ERROR(hr);

	// TODO a négyes csodakonstanssal valamit kezdeni
	DWORD audclnt_streamflags = 0;
	hr = pDestinationAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, audclnt_streamflags, hnsRequestedDuration * 4, 0, pwfx, NULL);
	//hr = pSecondaryAudioClient->Initialize(AUDCLNT_SHAREMODE_EXCLUSIVE, audclnt_streamflags, hnsRequestedDuration * 4, 0, pwfx, NULL);
	EXIT_ON_ERROR(hr);

	CComPtr<IAudioRenderClient> pRenderClient;
	hr = pDestinationAudioClient->GetService(
		IID_IAudioRenderClient,
		(void**)&pRenderClient);
	EXIT_ON_ERROR(hr);




	//hr = pAudioClient->Start();  // Start recording.
	hr = pSourceAudioClient->Start();
	EXIT_ON_ERROR(hr);

	hr = pDestinationAudioClient->Start();
	EXIT_ON_ERROR(hr);


	// Calculate the actual duration of the allocated buffer.
	//REFERENCE_TIME hnsActualDuration = REFERENCE_TIME(double(REFTIMES_PER_SEC) * bufferFrameCount / pwfx->nSamplesPerSec);
	DWORD hnsActualDuration = DWORD(double(__REFTIMES_PER_SEC) * bufferFrameCount / pwfx->nSamplesPerSec);

	// Each loop fills about half of the shared buffer.
	while (!stop) {
		// Sleep for half the buffer duration.
		Sleep(hnsActualDuration / __REFTIMES_PER_MILLISEC / 2);

		UINT32 packetLength = 0;
		hr = pCaptureClient->GetNextPacketSize(&packetLength);
		EXIT_ON_ERROR(hr);



		while (packetLength != 0 && !stop) {

			// Get the available data in the shared buffer.
			UINT32 numFramesAvailable;
			DWORD flags;
			BYTE* pCaptureData;
			BYTE* pRenderData;

			hr = pCaptureClient->GetBuffer(
				&pCaptureData,
				&numFramesAvailable,
				&flags,
				NULL,
				NULL
			);
			EXIT_ON_ERROR(hr);

			BOOL bufferIsSilent = (flags & AUDCLNT_BUFFERFLAGS_SILENT);
			//if (flags & AUDCLNT_BUFFERFLAGS_SILENT) {
			//	pCaptureData = NULL;  // Tell CopyData to write silence.
			//}

			if (!bufferIsSilent) {



				hr = pRenderClient->GetBuffer(numFramesAvailable, &pRenderData);
				EXIT_ON_ERROR(hr);


				memcpy(pRenderData, pCaptureData, numFramesAvailable * pwfx->nBlockAlign);
				// Load the initial data into the shared buffer.
				//hr = pMySource->LoadData(bufferFrameCount, pData, &flags);
				//EXIT_ON_ERROR(hr);

				DWORD renderFlags = 0;
				hr = pRenderClient->ReleaseBuffer(numFramesAvailable, renderFlags);
				EXIT_ON_ERROR(hr);
				//pRenderClient->
				// Copy the available capture data to the audio sink.
				//hr = pMySink->CopyData(pData, numFramesAvailable, &bDone);
				//EXIT_ON_ERROR(hr);

			}


			hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
			EXIT_ON_ERROR(hr);


			hr = pCaptureClient->GetNextPacketSize(&packetLength);
			EXIT_ON_ERROR(hr);
		}
	}

	hr = pSourceAudioClient->Stop();  // Stop recording.
	EXIT_ON_ERROR(hr);


	hr = pDestinationAudioClient->Stop();
	EXIT_ON_ERROR(hr);


	return S_OK;
}

HRESULT AudioDuplicator::RunAsync() {
	backgroundThread = std::thread([this]{
		if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL)) {
			ASSERT(0);
		}


		bool retry = true;
		while (retry) {
			HRESULT hr = Run();
			retry = FAILED(hr) && !stop;
			if (retry) {
				Sleep(1000);
			}
		}
	});

	return S_OK;
}


void AudioDuplicator::Stop() {
	stop = true;
}


void AudioDuplicator::WaitForDestroy() {
	if (backgroundThread.joinable()) {
		backgroundThread.join();
	}
}


HRESULT AudioDuplicator::InitDefaultDevices() {

	HRESULT hr;

	CComPtr<IMMDeviceEnumerator> pEnumerator;
	hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator, NULL,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)&pEnumerator
	);
	EXIT_ON_ERROR(hr);


	// render 'hook' init

	CComPtr<IMMDevice> pDefaultDevice;
	hr = pEnumerator->GetDefaultAudioEndpoint(
		eRender, eConsole, &pDefaultDevice);
	EXIT_ON_ERROR(hr);

	CComHeapPtr<WCHAR> defaultDeviceId;
	pDefaultDevice->GetId(&defaultDeviceId);
	this->sourceDevice = pDefaultDevice;

	std::vector<CComPtr<IMMDevice>> devices;
	AudioApi::EnumerateDevices(devices, EDataFlow::eRender);
	EXIT_ON_ERROR(hr);

	for (auto& device : devices) {
		CComHeapPtr<WCHAR> deviceId;
		device->GetId(&deviceId);
		EXIT_ON_ERROR(hr);

		if (StrCmpW((LPWSTR)deviceId, (LPWSTR)defaultDeviceId) != 0) {
			this->sinkDevice = device;
			return S_OK;
		}
		
	}




	return -1;
}
