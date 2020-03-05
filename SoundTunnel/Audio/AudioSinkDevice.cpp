#include "stdafx.h"


#include "AudioSinkDevice.h"
#include "AudioApi.h"


AudioSinkDevice::AudioSinkDevice(CComPtr<IMMDevice> sinkDevice) : sinkDevice(sinkDevice) {
	AudioApi::GetFriendlyName(sinkDevice, friendlyDeviceName);
	NULL;
}

AudioSinkDevice::~AudioSinkDevice() {
	Destroy();
}

HRESULT AudioSinkDevice::Create(const WAVEFORMATEX * pwfx) {

	HRESULT hr = S_OK;

	//CComPtr<IAudioClient> sinkAudioClient;
	hr = sinkDevice->Activate(IID_IAudioClient, CLSCTX_ALL, nullptr, (void**)&sinkAudioClient);
	EXIT_ON_ERROR(hr);


	// TODO a négyes csodakonstanssal valamit kezdeni
	REFERENCE_TIME hnsRequestedDuration = BUFFER_DURATION_HNS;

	//DWORD audclnt_streamflags = AUDCLNT_STREAMFLAGS_EVENTCALLBACK;

	//WAVEFORMATEX* pClosestMatch;
	//CComHeapPtr<WAVEFORMATEX> pClosestMatch;
	//hr = sinkAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, pwfx, &pClosestMatch);
	//HRESULT isFormatSupported = hr;
	//EXIT_ON_ERROR(hr);

	//WAVEFORMATEX closestMatch;
	//closestMatch = *pClosestMatch;
	//CoTaskMemFree(pClosestMatch);

	//AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM
	// NOTE: ez elvileg a win10 sdk-ban már benne van!
	static const DWORD _AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM = 0x80000000;
	static const DWORD _AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY = 0x08000000;

	DWORD audclnt_streamflags = AUDCLNT_STREAMFLAGS_EVENTCALLBACK | _AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | _AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY | AUDCLNT_STREAMFLAGS_LOOPBACK;
	//AUDCLNT_STREAMFLAGS_LOOPBACK



	//hr = sinkAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, audclnt_streamflags, hnsRequestedDuration * 10, 0, pClosestMatch, NULL);
	hr = sinkAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, audclnt_streamflags, hnsRequestedDuration * 4, 0, pwfx, NULL);
	//hr = pSecondaryAudioClient->Initialize(AUDCLNT_SHAREMODE_EXCLUSIVE, audclnt_streamflags, hnsRequestedDuration * 4, 0, pwfx, NULL);
	EXIT_ON_ERROR(hr);

	//waveFormat = *pwfx;
	blockAlign = pwfx->nBlockAlign;


	eventHandle.Attach(CreateEventW(NULL, FALSE, FALSE, NULL));
	hr = sinkAudioClient->SetEventHandle(eventHandle);
	EXIT_ON_ERROR(hr);


	hr = sinkAudioClient->GetService(
		IID_IAudioRenderClient,
		(void**)&sinkAudioRenderClient);
	EXIT_ON_ERROR(hr);

	/////////////////////////// proba: start elött initelni a buffert 0 ra
	//CComHeapPtr<WAVEFORMATEX> waveFormat;
	//hr = sinkAudioClient->GetMixFormat(&waveFormat); EXIT_ON_ERROR(hr);

	//UINT32 total;
	//hr = sinkAudioClient->GetBufferSize(&total); EXIT_ON_ERROR(hr);

	//UINT32 padding;
	//hr = sinkAudioClient->GetCurrentPadding(&padding); EXIT_ON_ERROR(hr);

	//UINT32 buffer_size = total-padding;
	//byte* buffer;
	//hr = sinkAudioRenderClient->GetBuffer(buffer_size, &buffer); EXIT_ON_ERROR(hr);

	//
	//size_t size = buffer_size*pwfx->nBlockAlign;
	////size_t size = buffer_size*waveFormat->nBlockAlign;
	//memset(buffer, 0, size);

	//hr = sinkAudioRenderClient->ReleaseBuffer(buffer_size, 0); EXIT_ON_ERROR(hr);

	///////////////////////////


	hr = sinkAudioClient->Start();
	EXIT_ON_ERROR(hr);




	return S_OK;
}

HRESULT AudioSinkDevice::AddSamples(BufferIterator & bufferIterator) {
	HRESULT hr = S_OK;

	UINT32  totalFrames;
	hr = sinkAudioClient->GetBufferSize(&totalFrames);
	EXIT_ON_ERROR(hr);

	UINT32 occupiedFrames;
	hr = sinkAudioClient->GetCurrentPadding(&occupiedFrames);
	EXIT_ON_ERROR(hr);


	UINT32 numFramesAvailable = totalFrames - occupiedFrames;
	if (numFramesAvailable == 0) {
		return hr;
	}

	// TODO csak proba
	numFramesAvailable /= 6;

	size_t size = numFramesAvailable * blockAlign;
	Chunk chunk = bufferIterator.NextChunk(size);


	numFramesAvailable = UINT32(chunk.size / blockAlign);

	if (numFramesAvailable == 0) {
		// illett volna leelenorizni hogy buffer.hasnext()
		ASSERT(0);
		return hr;
		//return S_FALSE;
	}


	BYTE* pRenderData;
	hr = sinkAudioRenderClient->GetBuffer(numFramesAvailable, &pRenderData);
	EXIT_ON_ERROR(hr);


	memcpy(pRenderData, chunk.ptr, chunk.size);
	//memcpy(pRenderData, data, numFramesAvailable * pwfx->nBlockAlign);



	DWORD renderFlags = 0;
	hr = sinkAudioRenderClient->ReleaseBuffer(numFramesAvailable, renderFlags);
	EXIT_ON_ERROR(hr);


	return hr;
}

HRESULT AudioSinkDevice::Destroy() {
	HRESULT hr = S_OK;

	if (sinkAudioClient) {
		hr = sinkAudioClient->Stop();
		EXIT_ON_ERROR(hr);
	}


	return S_OK;
}

HRESULT AudioSinkDevice::WaitUntilBufferIsAvailable(DWORD timeout_millisecs) {
	HRESULT hr = S_OK;
	DWORD retval = WaitForSingleObject(eventHandle, timeout_millisecs);
	if (retval != WAIT_OBJECT_0) {
		hr = ERROR_TIMEOUT;
		return hr;
	}
	return hr;

}

HRESULT AudioSinkDevice::WaitUntilPlaybackEnd(DWORD timeout_millisecs) {
	HRESULT hr = S_OK;

	while (true) {
		UINT32 occupiedFrames;
		hr = sinkAudioClient->GetCurrentPadding(&occupiedFrames);
		EXIT_ON_ERROR(hr);

		if (occupiedFrames == 0) {
			break;
		}

		HRESULT hr = S_OK;
		DWORD retval = WaitForSingleObject(eventHandle, timeout_millisecs);
		if (retval != WAIT_OBJECT_0) {
			hr = ERROR_TIMEOUT;
			return hr;
		}
	}

	return hr;

}
