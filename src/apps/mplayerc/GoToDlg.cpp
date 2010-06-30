/* 
 *	Copyright (C) 2003-2006 Gabest
 *	http://www.gabest.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *   
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *   
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. 
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

// GoToDlg.cpp : implementation file
//

#include "stdafx.h"
#include "mplayerc.h"
#include "GoToDlg.h"

#include "..\..\..\lib\ATL Server\include\atlrx.h"


// CGoToDlg dialog

IMPLEMENT_DYNAMIC(CGoToDlg, CDialog)
CGoToDlg::CGoToDlg(int time, float fps, CWnd* pParent /*=NULL*/)
	: CDialog(CGoToDlg::IDD, pParent)
	, m_timestr(_T(""))
	, m_framestr(_T(""))
	, m_time(time)
	, m_fps(fps)
{
	if(m_fps == 0)
	{
		CString str = AfxGetMyApp()->GetProfileString(ResStr(IDS_R_SETTINGS), _T("fps"), _T("0"));
		if(_stscanf(str, _T("%f"), &m_fps) != 1) m_fps = 0;
	}
}

CGoToDlg::~CGoToDlg()
{
}

void CGoToDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_timestr);
	DDX_Text(pDX, IDC_EDIT2, m_framestr);
	DDX_Control(pDX, IDC_EDIT1, m_timeedit);
	DDX_Control(pDX, IDC_EDIT2, m_frameedit);
}

BOOL CGoToDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(m_time >= 0)
	{
		m_timestr.Format(_T("%02d:%02d:%02d.%03d"), 
			(m_time/(1000*60*60))%60, (m_time/(1000*60))%60, (m_time/1000)%60, m_time%1000);

		if(m_fps > 0)
		{
			m_framestr.Format(_T("%d, %.3f"), (int)(m_fps*m_time/1000), m_fps);
		}

		UpdateData(FALSE);

		switch(AfxGetMyApp()->GetProfileInt(ResStr(IDS_R_SETTINGS), _T("gotoluf"), 0))
		{
		default:
		case 0: m_timeedit.SetFocus(); m_timeedit.SetSel(0, 0); break;
		case 1: m_frameedit.SetFocus(); m_frameedit.SetSel(0, m_framestr.Find(',')); break;
		}

	}

	return FALSE;

//	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


BEGIN_MESSAGE_MAP(CGoToDlg, CDialog)
	ON_BN_CLICKED(IDC_OK1, OnBnClickedOk1)
	ON_BN_CLICKED(IDC_OK2, OnBnClickedOk2)
END_MESSAGE_MAP()


// CGoToDlg message handlers

void CGoToDlg::OnBnClickedOk1()
{
	UpdateData();

	int hh, mm, ss, ms;
	hh = mm = ss = ms = 0;

	CAtlRegExp<> re;

	REParseError status = re.Parse(_T("{\\z}"), FALSE);
	if(REPARSE_ERROR_OK == status)
	{
		CAtlREMatchContext<> mc;
		const CAtlREMatchContext<>::RECHAR* s = m_timestr.GetBuffer();
		const CAtlREMatchContext<>::RECHAR* e = NULL;
		while(s && re.Match(s, &mc, &e))
		{
			const CAtlREMatchContext<>::RECHAR* szStart = 0;
			const CAtlREMatchContext<>::RECHAR* szEnd = 0;
			mc.GetMatch(0, &szStart, &szEnd);

			if(hh != 0 || hh > 59 || mm > 59 || ss > 59)
			{
				AfxMessageBox(_T("Error parsing entered time!"));
				return;
			}

			hh = mm;
			mm = ss;
			ss = ms;
			ms = _tcstol(szStart, (TCHAR**)&szStart, 10);

			s = e;
		}

		m_time = ((hh*60+mm)*60+ss)*1000+ms;

		AfxGetMyApp()->WriteProfileInt(ResStr(IDS_R_SETTINGS), _T("gotoluf"), 0);

		OnOK();
	}
}


void CGoToDlg::OnBnClickedOk2()
{
	UpdateData();

	int frame = 0;
	float fps = 0;

	CAtlRegExp<> re;

	REParseError status = re.Parse(_T("{\\z}[^0-9\\.]+{[0-9\\.]+}"), FALSE);
	if(REPARSE_ERROR_OK == status)
	{
		CAtlREMatchContext<> mc;
		const CAtlREMatchContext<>::RECHAR* s = m_framestr.GetBuffer();
		const CAtlREMatchContext<>::RECHAR* e = NULL;
		if(re.Match(s, &mc, &e))
		{
			const CAtlREMatchContext<>::RECHAR* szStart = 0;
			const CAtlREMatchContext<>::RECHAR* szEnd = 0;

			mc.GetMatch(0, &szStart, &szEnd);
			frame = _tcstol(szStart, (TCHAR**)&szStart, 10);

			mc.GetMatch(1, &szStart, &szEnd);
			if(_stscanf(szStart, _T("%f"), &fps) != 1) fps = 0;
			else AfxGetMyApp()->WriteProfileString(ResStr(IDS_R_SETTINGS), _T("fps"), szStart);
		}
		else
		{
			AfxMessageBox(_T("Error parsing entered text!"));
			return;
		}

		if(fps == 0)
		{
			AfxMessageBox(_T("Error parsing entered frame-rate!"));
			return;
		}

		m_time = (int)(1000.0*frame/fps) + 1;

		AfxGetMyApp()->WriteProfileInt(ResStr(IDS_R_SETTINGS), _T("gotoluf"), 1);

		OnOK();
	}
}

BOOL CGoToDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		if(*GetFocus() == m_timeedit) OnBnClickedOk1();
		else if(*GetFocus() == m_frameedit) OnBnClickedOk2();

		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}
