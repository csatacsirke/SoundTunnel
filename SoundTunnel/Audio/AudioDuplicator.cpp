#include "stdafx.h"

#include "AudioDuplicator.h"
#include "AudioApi.h"


#pragma comment(lib, "avrt.lib")
#include <avrt.h>

#include <set>
#include <cmath>

static const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
static const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
static const IID IID_IAudioClient = __uuidof(IAudioClient);
static const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);
static const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);


using namespace AudioApi;


AudioDuplicator::AudioDuplicator(IMMDevice* sourceDevice, IMMDevice* sinkDevice) {

	m_sourceDevice = sourceDevice;
	m_sinkDevice = sinkDevice;

	Init();

}

AudioDuplicator::~AudioDuplicator() {
	StopInternal();
}

bool AudioDuplicator::Run() {
	RunInternal();
	return false;
}

void AudioDuplicator::Stop() {
	StopInternal();
}

HRESULT AudioDuplicator::Init() {

	HRESULT hr = S_OK;

	// Common init 
	HRESULT hr_co_init = CoInitialize(NULL);
	//EXIT_ON_ERROR(hr_co_init); // lehet hogy már be volt állítva másra?

	

	//hr = InitDefaultDevices();
	//EXIT_ON_ERROR(hr);


	return hr;
}

//HRESULT AudioDuplicator::SetSourceDevice(CComPtr<IMMDevice> device) {
//	this->m_sourceDevice = device;
//	return S_OK;
//}
//
//HRESULT AudioDuplicator::SetSinkDevice(CComPtr<IMMDevice> device) {
//	this->m_sinkDevice = device;
//	return S_OK;
//}

namespace {

	class ScopedThreadCharacteristics {
		HANDLE mmcss = 0;
	public:
		ScopedThreadCharacteristics() {

			DWORD taskIndex = 0;
			mmcss = AvSetMmThreadCharacteristicsW(L"Pro Audio", &taskIndex);
			ASSERT(mmcss);
		}

		~ScopedThreadCharacteristics() {
			if (mmcss) {
				AvRevertMmThreadCharacteristics(mmcss);
			};
		}
	};
}


HRESULT AudioDuplicator::RunInternal() {

	ScopedThreadCharacteristics _scopedThreadCharacteristics;



	HRESULT hr = S_OK;

	// Notify the audio sink which format to use.
	//hr = pMySink->SetFormat(pwfx);
	//EXIT_ON_ERROR(hr);


	m_cancellationEvent.ResetEvent();
	CEvent sourceAudioEvent{};
	


	CComPtr<IAudioClient> pSourceAudioClient;
	hr = m_sourceDevice->Activate(IID_IAudioClient, CLSCTX_ALL, nullptr, (void**)&pSourceAudioClient);
	EXIT_ON_ERROR(hr);


	// TODO ezt amugy releaselni kéne... de hát a faszom...
	//WAVEFORMATEX* pwfx = nullptr;
	CComHeapPtr<WAVEFORMATEX> pwfx;
	hr = pSourceAudioClient->GetMixFormat(&pwfx);
	EXIT_ON_ERROR(hr);

	// HSN: HUNDRED NANO SECONDS
	REFERENCE_TIME hnsRequestedBufferDuration = BUFFER_DURATION_HNS;
	hr = pSourceAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK | AUDCLNT_STREAMFLAGS_EVENTCALLBACK, hnsRequestedBufferDuration, 0, pwfx, NULL);
	EXIT_ON_ERROR(hr);


	hr = pSourceAudioClient->SetEventHandle(sourceAudioEvent);
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
	hr = m_sinkDevice->Activate(IID_IAudioClient, CLSCTX_ALL, nullptr, (void**)&pDestinationAudioClient);
	EXIT_ON_ERROR(hr);

	// TODO a négyes csodakonstanssal valamit kezdeni
	DWORD audclnt_streamflags = 0;
	hr = pDestinationAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, audclnt_streamflags, hnsRequestedBufferDuration * 4, 0, pwfx, NULL);
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



	
	const int64_t targetMaxFramesInCaptureBuffer = static_cast<int64_t>(pwfx->nSamplesPerSec * 0.060);

	// Calculate the actual duration of the allocated buffer.
	//REFERENCE_TIME hnsActualDuration = REFERENCE_TIME(double(REFTIMES_PER_SEC) * bufferFrameCount / pwfx->nSamplesPerSec);
	//DWORD hnsActualDuration = DWORD(double(__REFTIMES_PER_SEC) * bufferFrameCount / pwfx->nSamplesPerSec);

	// Each loop fills about half of the shared buffer.
	while (true) {
		// Sleep for half the buffer duration.
		//Sleep(hnsActualDuration / __REFTIMES_PER_MILLISEC / 2);

		using Handles = HANDLE[];
		const DWORD WAIT_OBJECT_CANCEL = WAIT_OBJECT_0;
		const DWORD WAIT_OBJECT_SOURCE_AUDIO_EVENT = WAIT_OBJECT_0 + 1;

		const DWORD wait_result = ::WaitForMultipleObjects(2, Handles{ m_cancellationEvent.m_hObject, sourceAudioEvent.m_hObject }, FALSE, INFINITE);
		if (wait_result == WAIT_OBJECT_CANCEL) {
			break;
		}

		if (wait_result != WAIT_OBJECT_SOURCE_AUDIO_EVENT) {
			// ??? some kind of error
			break;
		}


		// Get the available data in the shared buffer.
		UINT32 numFramesAvailable;
		DWORD flags;
		BYTE* pCaptureData;

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

			const UINT32 outputBufferSize = targetMaxFramesInCaptureBuffer;

			BYTE* pRenderData = nullptr;
			hr = pRenderClient->GetBuffer(outputBufferSize, &pRenderData);

			if (hr != AUDCLNT_E_BUFFER_TOO_LARGE) {
				EXIT_ON_ERROR(hr);
			}

			if (!pRenderData) {
				ASSERT(hr == AUDCLNT_E_BUFFER_TOO_LARGE);
				continue;
			}


			if (numFramesAvailable < outputBufferSize) {
				// audio latency is not yet too big
				memcpy(pRenderData, pCaptureData, numFramesAvailable * pwfx->nBlockAlign);
			}
			else {
				float* out = reinterpret_cast<float*>(pRenderData);
				const float* in = reinterpret_cast<float*>(pCaptureData);

				const WORD channelCount = pwfx->nChannels;

				for (size_t outputFrameIndex = 0; outputFrameIndex < outputBufferSize; ++outputFrameIndex) {
					for (UINT32 channelIndex = 0; channelIndex < channelCount; ++channelIndex) {

						const UINT32 inputFrameIndexFloor = outputFrameIndex * numFramesAvailable / outputBufferSize;
						const double inputFrameIndexTheoretical = double(outputFrameIndex) * double(numFramesAvailable) / double(outputBufferSize);

						const UINT32 inputFrameIndex_1 = inputFrameIndexFloor;
						const UINT32 inputFrameIndex_2 = std::min(inputFrameIndexFloor + 1, numFramesAvailable);

						const float sample_1 = pCaptureData[inputFrameIndex_1];
						const float sample_2 = pCaptureData[inputFrameIndex_2];

						const double t = inputFrameIndexTheoretical - double(inputFrameIndexFloor);

						const float output_sample = (float)std::lerp<double>(sample_1, sample_2, t);

						pRenderData[outputFrameIndex * channelCount + channelIndex] = output_sample;
					}
				}
			}
			
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

	}

	hr = pSourceAudioClient->Stop();  // Stop recording.
	EXIT_ON_ERROR(hr);


	hr = pDestinationAudioClient->Stop();
	EXIT_ON_ERROR(hr);


	return S_OK;
}

//HRESULT AudioDuplicator::RunAsync() {
//	backgroundThread = std::thread([this]{
//		if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL)) {
//			ASSERT(0);
//		}
//
//
//		bool retry = true;
//		while (retry) {
//			HRESULT hr = Run();
//			retry = FAILED(hr) && !stop;
//			if (retry) {
//				Sleep(1000);
//			}
//		}
//	});
//
//	return S_OK;
//}


void AudioDuplicator::StopInternal() {
	//stop = true;
	m_cancellationEvent.SetEvent();
}


//HRESULT AudioDuplicator::InitDefaultDevices() {
//
//	HRESULT hr = S_OK;
//
//	CComPtr<IMMDeviceEnumerator> pEnumerator;
//	hr = CoCreateInstance(
//		CLSID_MMDeviceEnumerator, NULL,
//		CLSCTX_ALL, IID_IMMDeviceEnumerator,
//		(void**)&pEnumerator
//	);
//	EXIT_ON_ERROR(hr);
//
//
//	// render 'hook' init
//
//	CComPtr<IMMDevice> pDefaultDevice;
//	hr = pEnumerator->GetDefaultAudioEndpoint(
//		eRender, eConsole, &pDefaultDevice);
//	EXIT_ON_ERROR(hr);
//
//	CComHeapPtr<WCHAR> defaultDeviceId;
//	pDefaultDevice->GetId(&defaultDeviceId);
//	this->m_sourceDevice = pDefaultDevice;
//
//	std::vector<CComPtr<IMMDevice>> devices;
//	AudioApi::EnumerateDevices(devices, EDataFlow::eRender);
//	EXIT_ON_ERROR(hr);
//
//	for (auto& device : devices) {
//		CComHeapPtr<WCHAR> deviceId;
//		device->GetId(&deviceId);
//		EXIT_ON_ERROR(hr);
//
//		if (StrCmpW((LPWSTR)deviceId, (LPWSTR)defaultDeviceId) != 0) {
//			this->m_sinkDevice = device;
//			return S_OK;
//		}
//		
//	}
//
//
//	return E_FAIL;
//}
