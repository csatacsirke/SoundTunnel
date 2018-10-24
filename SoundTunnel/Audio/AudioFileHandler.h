#pragma once

#include "AudioApi.h"


// forrás másolva:
// https://msdn.microsoft.com/en-us/library/windows/desktop/dd757929(v=vs.85).aspx (2017.11.02)


#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <stdio.h>
#include <mferror.h>


#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")





// Note  This function is not defined in an SDK header.To use this function, you must define it in your own code.
// Many of the code examples in this documentation use the following function to release COM interface pointers.
template <class T> void SafeRelease(T **ppT) {
	if (*ppT) {
		(*ppT)->Release();
		*ppT = NULL;
	}
}

#ifndef FOURCC

// Note  A FOURCC is a DWORD formed by concatenating four ASCII characters.
#define FOURCC DWORD

#endif



//-------------------------------------------------------------------
// FixUpChunkSizes
//
// Writes the file-size information into the WAVE file header.
//
// WAVE files use the RIFF file format. Each RIFF chunk has a data
// size, and the RIFF header has a total file size.
//-------------------------------------------------------------------

HRESULT FixUpChunkSizes(
	HANDLE hFile,           // Output file.
	DWORD cbHeader,         // Size of the 'fmt ' chuck.
	DWORD cbAudioData       // Size of the 'data' chunk.
);






//-------------------------------------------------------------------
// WriteWaveData
//
// Decodes PCM audio data from the source file and writes it to
// the WAVE file.
//-------------------------------------------------------------------

HRESULT WriteWaveData(
	HANDLE hFile,               // Output file.
	IMFSourceReader *pReader,   // Source reader.
	DWORD cbMaxAudioData,       // Maximum amount of audio data (bytes).
	DWORD *pcbDataWritten       // Receives the amount of data written.
);




//-------------------------------------------------------------------
// CalculateMaxAudioDataSize
//
// Calculates how much audio to write to the WAVE file, given the
// audio format and the maximum duration of the WAVE file.
//-------------------------------------------------------------------

DWORD CalculateMaxAudioDataSize(
	IMFMediaType *pAudioType,    // The PCM audio format.
	DWORD cbHeader,              // The size of the WAVE file header.
	DWORD msecAudioData          // Maximum duration, in milliseconds.
);



//-------------------------------------------------------------------
//
// Writes a block of data to a file
//
// hFile: Handle to the file.
// p: Pointer to the buffer to write.
// cb: Size of the buffer, in bytes.
//
//-------------------------------------------------------------------

HRESULT WriteToFile(HANDLE hFile, void* p, DWORD cb);

//-------------------------------------------------------------------
// WriteWaveHeader
//
// Write the WAVE file header.
//
// Note: This function writes placeholder values for the file size
// and data size, as these values will need to be filled in later.
//-------------------------------------------------------------------

HRESULT WriteWaveHeader(
	HANDLE hFile,               // Output file.
	IMFMediaType *pMediaType,   // PCM audio format.
	DWORD *pcbWritten           // Receives the size of the header.
);


//-------------------------------------------------------------------
// ConfigureAudioStream
//
// Selects an audio stream from the source file, and configures the
// stream to deliver decoded PCM audio.
//-------------------------------------------------------------------

HRESULT ConfigureAudioStream(
	IMFSourceReader *pReader,   // Pointer to the source reader.
	IMFMediaType **ppPCMAudio   // Receives the audio format.
);


HRESULT WriteWaveFile(
	IMFSourceReader *pReader,   // Pointer to the source reader.
	HANDLE hFile,               // Handle to the output file.
	LONG msecAudioData          // Maximum amount of audio data to write, in msec.
);


int __wmain(int argc, wchar_t* argv[]);


class AudioFile {
	vector<byte> data;
	WAVEFORMATEX waveFormat;

public:
	AudioFile() {}
	~AudioFile() {}
public:
	HRESULT Open(CString fileName) {
		HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);


		const WCHAR *wszSourceFile = fileName;

		const LONG MAX_AUDIO_DURATION_MSEC = 5000; // 5 seconds

		HRESULT hr = S_OK;

		IMFSourceReader *pReader = NULL;

		// Initialize the COM library.
		hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

		// Initialize the Media Foundation platform.
		if (SUCCEEDED(hr)) {
			hr = MFStartup(MF_VERSION);
		}

		// Create the source reader to read the input file.
		if (SUCCEEDED(hr)) {
			hr = MFCreateSourceReaderFromURL(wszSourceFile, NULL, &pReader);
			if (FAILED(hr)) {
				printf("Error opening input file: %S %d \n", wszSourceFile, hr);
			}
		}

		
		// Write the WAVE file.
		if (SUCCEEDED(hr)) {
			hr = this->ReadWaveFile(pReader);
		}

		if (FAILED(hr)) {
			printf("Failed, hr = 0x%X\n", hr);
		}


		SafeRelease(&pReader);
		MFShutdown();
		
		return hr;
	}

	const vector<byte>& GetData() {
		return data;
	}

	const WAVEFORMATEX& GetWaveFormat() {
		return waveFormat;
	}

	//struct Iterator {

	//	Iterator(const vector<byte>& buffer) {

	//	}
	//}

	//const vector<byte>& GetDataIterator() {
	//	return Iterator(data);
	//}
private:
	HRESULT ReadWaveFile(IMFSourceReader* pReader) {
		HRESULT hr = S_OK;

		DWORD cbHeader = 0;         // Size of the WAVE file header, in bytes.
		DWORD cbAudioData = 0;      // Total bytes of PCM audio data written to the file.
		DWORD cbMaxAudioData = 0;

		IMFMediaType *pAudioType = NULL;    // Represents the PCM audio format.

											// Configure the source reader to get uncompressed PCM audio from the source file.

		hr = ConfigureAudioStream(pReader, &pAudioType);

		// Write the WAVE file header.
		if (SUCCEEDED(hr)) {
			GetWaveFormat(pAudioType, this->waveFormat);
			//hr = WriteWaveHeader(hFile, pAudioType, &cbHeader);
		}

		// Calculate the maximum amount of audio to decode, in bytes.
		if (SUCCEEDED(hr)) {
			//cbMaxAudioData = CalculateMaxAudioDataSize(pAudioType, cbHeader, msecAudioData);

			// Decode audio data to the file.
			hr = ReadWaveData(pReader);
		}

		
		SafeRelease(&pAudioType);
		return hr;
	}


	HRESULT GetWaveFormat(IMFMediaType* pMediaType, WAVEFORMATEX& waveFormat) {
		HRESULT hr = S_OK;

		UINT32 cbFormat = 0;
		WAVEFORMATEX *pWav = NULL;

		// Convert the PCM audio format into a WAVEFORMATEX structure.
		hr = MFCreateWaveFormatExFromMFMediaType(pMediaType, &pWav, &cbFormat);

		// Write the 'RIFF' header and the start of the 'fmt ' chunk.
		if (SUCCEEDED(hr)) {
			waveFormat = *pWav;
		}

		
		CoTaskMemFree(pWav);
		return hr;
	}


	HRESULT ReadWaveData(IMFSourceReader* pReader) {
		HRESULT hr = S_OK;
		DWORD cbAudioData = 0;
		DWORD cbBuffer = 0;
		BYTE *pAudioData = NULL;

		IMFSample *pSample = NULL;
		IMFMediaBuffer *pBuffer = NULL;

		// Get audio samples from the source reader.
		while (true) {
			DWORD dwFlags = 0;

			// Read the next sample.
			hr = pReader->ReadSample(
				(DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM,
				0, NULL, &dwFlags, NULL, &pSample);

			if (FAILED(hr)) { break; }

			if (dwFlags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED) {
				printf("Type change - not supported by WAVE file format.\n");
				break;
			}
			if (dwFlags & MF_SOURCE_READERF_ENDOFSTREAM) {
				printf("End of input file.\n");
				break;
			}

			if (pSample == NULL) {
				printf("No sample\n");
				continue;
			}

			// Get a pointer to the audio data in the sample.

			hr = pSample->ConvertToContiguousBuffer(&pBuffer);

			if (FAILED(hr)) { break; }


			hr = pBuffer->Lock(&pAudioData, NULL, &cbBuffer);

			if (FAILED(hr)) { break; }


			// Make sure not to exceed the specified maximum size.
			//if (cbMaxAudioData - cbAudioData < cbBuffer) {
			//	cbBuffer = cbMaxAudioData - cbAudioData;
			//}

			// Write this data to the output file.
			//hr = WriteToFile(hFile, pAudioData, cbBuffer);
			hr = WriteData(pAudioData, cbBuffer);
			

			if (FAILED(hr)) { break; }

			// Unlock the buffer.
			hr = pBuffer->Unlock();
			pAudioData = NULL;

			if (FAILED(hr)) { break; }

			// Update running total of audio data.
			cbAudioData += cbBuffer;

			

			SafeRelease(&pSample);
			SafeRelease(&pBuffer);
		}

		if (SUCCEEDED(hr)) {
			printf("Wrote %d bytes of audio data.\n", cbAudioData);

			//*pcbDataWritten = cbAudioData;
		}

		if (pAudioData) {
			pBuffer->Unlock();
		}

		SafeRelease(&pBuffer);
		SafeRelease(&pSample);
		return hr;
	}

	HRESULT WriteData(byte* pAudioData, size_t cbBuffer) {
		HRESULT hr = S_OK;

		size_t offset = data.size();
		data.resize(offset + cbBuffer);
		memcpy(data.data() + offset, pAudioData, cbBuffer);
		
		return hr;
	}

};

class AudioFileReader {

};


