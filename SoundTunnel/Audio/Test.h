#pragma once


#include "AudioStreamer.h"


static HRESULT Test_AudioStreamer() {

	HRESULT hr = S_OK;
	CString testFileName = L"d:\\Sounds\\de kar hogy nem voltam itt tudom az ellenatkot ami megmenthette volna.wav";

	AudioStreamer audioStreamer;
	hr = audioStreamer.AddDefaultDevices();
	EXIT_ON_ERROR(hr);
	hr = audioStreamer.PlayFile(testFileName);
	EXIT_ON_ERROR(hr);

	return hr;
}

static void Test() {
	HRESULT hr = Test_AudioStreamer();
	if (FAILED(hr)) {
		CString errormsg = L"Player exception";
		MessageBox(0, errormsg, 0, 0);
	}
}

static void ___DoEverythingLofasz() {
	std::shared_ptr<AudioDuplicator> recorder = std::make_shared<AudioDuplicator>();


	std::thread myThread([&recorder] {
		recorder->Run();

	});

	MessageBox(NULL, L"Press ok to stop", 0, MB_ICONASTERISK);
	recorder->Stop();
	myThread.join();

}


