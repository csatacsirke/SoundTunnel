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


class AudioFileReader {

};


