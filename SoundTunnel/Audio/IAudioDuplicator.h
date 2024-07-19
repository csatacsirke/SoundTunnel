#pragma once


#include <Mmdeviceapi.h>
#include <atlcomcli.h>

#include <memory>
#include <vector>


class IAudioDuplicator {
public:
	virtual bool Run() = 0;
	virtual void Stop() = 0;
};


struct Devices {
	CComPtr<IMMDevice> defaultDevice;
	std::vector<CComPtr<IMMDevice>> devices;
};


std::shared_ptr<IAudioDuplicator> CreateAudioDuplicator(IMMDevice* sourceDevice, IMMDevice* sinkDevice);
std::shared_ptr<Devices> EnumerateDevices();
