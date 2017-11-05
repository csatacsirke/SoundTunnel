
// SoundTunnel.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CSoundTunnelApp:
// See SoundTunnel.cpp for the implementation of this class
//

class CSoundTunnelApp : public CWinApp
{
public:
	CSoundTunnelApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CSoundTunnelApp theApp;

// Saját

//#include "Audio\AudioApi.h"

#include "Audio\AudioDuplicator.h"
#include "Audio\AudioStreamer.h"
#include "Audio\AudioFileHandler.h"
//#include "Audio\MicrophoneOut.h"







