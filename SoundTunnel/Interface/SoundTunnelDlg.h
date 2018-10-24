
// SoundTunnelDlg.h : header file
//

#pragma once

#include "Audio\AudioDuplicator.h"

#include <thread>
#include <future>
#include <memory>


// CSoundTunnelDlg dialog
class CSoundTunnelDlg : public CDialogEx
{
// Construction
public:
	CSoundTunnelDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SOUNDTUNNEL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:

	shared_ptr<AudioDuplicator> audioDuplicator = make_shared<AudioDuplicator>();
	//thread audioThread;

	void Start();
	void Stop();
};
