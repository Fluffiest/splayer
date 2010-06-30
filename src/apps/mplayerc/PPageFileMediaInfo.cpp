﻿// PPageFileMediaInfo.cpp : implementation file
//

#include "stdafx.h"
#include "mplayerc.h"
#include "PPageFileMediaInfo.h"

//#define  USE_MEDIAINFO_STATIC
#ifdef USE_MEDIAINFO_STATIC
//MD Static lib is too big
#include <MediaInfo.h>
//using namespace MediaInfoLib;
#else
#include <MediaInfoDLL.h>
using namespace MediaInfoDLL;
#endif
// CPPageFileMediaInfo dialog

IMPLEMENT_DYNAMIC(CPPageFileMediaInfo, CPropertyPage)
CPPageFileMediaInfo::CPPageFileMediaInfo(CString fn)
	: CPropertyPage(CPPageFileMediaInfo::IDD, CPPageFileMediaInfo::IDD)
	, m_fn(fn)
	, m_pCFont(NULL)
{
}

CPPageFileMediaInfo::~CPPageFileMediaInfo()
{
	if(m_pCFont) delete m_pCFont;
}

void CPPageFileMediaInfo::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MIEDIT, m_mediainfo);
}


BEGIN_MESSAGE_MAP(CPPageFileMediaInfo, CPropertyPage)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

// CPPageFileMediaInfo message handlers

BOOL CPPageFileMediaInfo::OnInitDialog()
{
	__super::OnInitDialog();

	if(!m_pCFont) m_pCFont = DNew CFont;
	if(!m_pCFont) return TRUE;
#ifdef USE_MEDIAINFO_STATIC
	MediaInfoLib::String f_name = m_fn;
	MediaInfoLib::MediaInfo MI;
	MI.Open(f_name);
	MI.Option(_T("Complete"));
	MI_Text = MI.Inform().c_str();
	MI.Close();
#else
	MediaInfoDLL::String f_name = m_fn;
	MediaInfo MI;
	MI.Open(f_name);
	MI.Option(_T("Complete"));
	MI_Text = MI.Inform().c_str();
	MI.Close();
#endif	
	if(!MI_Text.Find(_T("Unable to load"))) MI_Text = ResStr(IDS_FILEINFO_MSG_NEED_MEDIAINFO_DLL);

	LOGFONT lf;
	memset( &lf, 0, sizeof(lf) );
	lf.lfHeight = 10;
	lf.lfPitchAndFamily = DEFAULT_PITCH | FF_ROMAN;
	lstrcpy( lf.lfFaceName, _T("Lucida Console") );
	m_pCFont->CreateFontIndirect( &lf );
	m_mediainfo.SetFont( m_pCFont );
	m_mediainfo.SetWindowText(MI_Text);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPPageFileMediaInfo::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);
	
}
