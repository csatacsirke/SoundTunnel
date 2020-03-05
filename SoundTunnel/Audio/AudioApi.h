#pragma once

#include <Mmdeviceapi.h>

#include <vector>



namespace AudioApi {

	HRESULT EnumerateDevices(std::vector<CComPtr<IMMDevice>>& devices);


	HRESULT GetFriendlyName(CComPtr<IMMDevice> device, CString& friendlyName);


}





