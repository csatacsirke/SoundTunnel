#include "stdafx.h"
#include "AudioApi.h"

HRESULT AudioApi::EnumerateDevices(std::vector<CComPtr<IMMDevice>>& devices) {

	HRESULT hr = S_OK;


	hr = CoInitialize(NULL);
	EXIT_ON_ERROR(hr);


	CComPtr<IMMDeviceEnumerator> pEnumerator;
	hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator, NULL,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)&pEnumerator
	);
	EXIT_ON_ERROR(hr);

	CComPtr<IMMDeviceCollection> deviceCollection;
	hr = pEnumerator->EnumAudioEndpoints(eAll, DEVICE_STATE_ACTIVE, &deviceCollection);
	EXIT_ON_ERROR(hr);


	UINT deviceCount;
	hr = deviceCollection->GetCount(&deviceCount);
	EXIT_ON_ERROR(hr);

	for (int i = 0; i < int(deviceCount); ++i) {
		CComPtr<IMMDevice> device;
		hr = deviceCollection->Item(i, &device);
		EXIT_ON_ERROR(hr);

		devices.push_back(device);

	}

	return hr;
}

HRESULT AudioApi::GetFriendlyName(CComPtr<IMMDevice> device, CString & friendlyName) {
	HRESULT hr = S_OK;
	CComPtr<IPropertyStore> defaultDeviceProperties;

	hr = device->OpenPropertyStore(STGM_READ, &defaultDeviceProperties);
	EXIT_ON_ERROR(hr);


	//PROPVARIANT varFriendlyName;
	CPropVariant varFriendlyName;
	PropVariantInit(&varFriendlyName);
	//hr = ppProperties->GetValue(PKEY_DeviceInterface_FriendlyName, &varFriendlyName);
	hr = defaultDeviceProperties->GetValue(PKEY_Device_FriendlyName, &varFriendlyName);
	EXIT_ON_ERROR(hr);

	friendlyName = varFriendlyName.pwszVal;
	return hr;
	//return varFriendlyName.pwszVal;

}
