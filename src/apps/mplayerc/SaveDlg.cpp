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

// SaveDlg.cpp : implementation file
//

#include "stdafx.h"
#include "mplayerc.h"
#include "SaveDlg.h"
#include "..\..\filters\filters.h"


// CSaveDlg dialog

IMPLEMENT_DYNAMIC(CSaveDlg, CCmdUIDialog)
CSaveDlg::CSaveDlg(CString in, CString out, CWnd* pParent /*=NULL*/)
	: CCmdUIDialog(CSaveDlg::IDD, pParent)
	, m_in(in), m_out(out)
	, m_nIDTimerEvent(-1)
{
}

CSaveDlg::~CSaveDlg()
{
}

void CSaveDlg::DoDataExchange(CDataExchange* pDX)
{
	CCmdUIDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ANIMATE1, m_anim);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
	DDX_Control(pDX, IDC_REPORT, m_report);
	DDX_Control(pDX, IDC_FROMTO, m_fromto);
}


BEGIN_MESSAGE_MAP(CSaveDlg, CCmdUIDialog)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_MESSAGE(WM_GRAPHNOTIFY, OnGraphNotify)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CSaveDlg message handlers

BOOL CSaveDlg::OnInitDialog()
{
	CCmdUIDialog::OnInitDialog();

	//m_anim.Open(IDR_AVI_FILECOPY);
	//m_anim.Play(0, -1, -1);

	CString str, in = m_in, out = m_out;
	if(in.GetLength() > 60) in = in.Left(17) + _T("..") + in.Right(43);
	if(out.GetLength() > 60) out = out.Left(17) + _T("..") + out.Right(43);
	str.Format(_T("%s\r\n%s"), in, out);
	m_fromto.SetWindowText(str);

	m_progress.SetRange(0, 100);

	if(FAILED(pGB.CoCreateInstance(CLSID_FilterGraph)) || !(pMC = pGB) || !(pME = pGB) || !(pMS = pGB)
	|| FAILED(pME->SetNotifyWindow((OAHWND)m_hWnd, WM_GRAPHNOTIFY, 0)))
	{
		m_report.SetWindowText(_T("Error"));
		return FALSE;
	}

	HRESULT hr;

	CStringW fnw = m_in;
	CComPtr<IFileSourceFilter> pReader;

	if(!pReader && m_in.Mid(m_in.ReverseFind('.')+1).MakeLower() == _T("cda"))
	{
		hr = S_OK;
		CComPtr<IUnknown> pUnk = (IUnknown*)(INonDelegatingUnknown*)new CCDDAReader(NULL, &hr);
		if(FAILED(hr) || !(pReader = pUnk) || FAILED(pReader->Load(fnw, NULL)))
			pReader.Release();
	}

	if(!pReader)
	{
		hr = S_OK;
		CComPtr<IUnknown> pUnk = (IUnknown*)(INonDelegatingUnknown*)new CCDXAReader(NULL, &hr);
		if(FAILED(hr) || !(pReader = pUnk) || FAILED(pReader->Load(fnw, NULL)))
			pReader.Release();
	}

	if(!pReader /*&& ext == _T("ifo")*/)
	{
		hr = S_OK;
		CComPtr<IUnknown> pUnk = (IUnknown*)(INonDelegatingUnknown*)new CVTSReader(NULL, &hr);
		if(FAILED(hr) || !(pReader = pUnk) || FAILED(pReader->Load(fnw, NULL)))
			pReader.Release();
		else
		{
			CPath pout(m_out);
			pout.RenameExtension(_T(".ifo"));
			CopyFile(m_in, pout, FALSE);
		}
	}

	if(!pReader)
	{
		hr = S_OK;
		CComPtr<IUnknown> pUnk;
		pUnk.CoCreateInstance(CLSID_AsyncReader);
		if(FAILED(hr) || !(pReader = pUnk) || FAILED(pReader->Load(fnw, NULL)))
			pReader.Release();
	}

	if(!pReader)
	{
		hr = S_OK;
		CComPtr<IUnknown> pUnk;
		pUnk.CoCreateInstance(CLSID_URLReader);
		if(CComQIPtr<IBaseFilter> pSrc = pUnk) // url reader has to be in the graph to load the file
		{
			pGB->AddFilter(pSrc, fnw);
			if(FAILED(hr) || !(pReader = pUnk) || FAILED(hr = pReader->Load(fnw, NULL)))
			{
				pReader.Release();
				pGB->RemoveFilter(pSrc);
			}
		}
	}

	CComQIPtr<IBaseFilter> pSrc = pReader;
	if(FAILED(pGB->AddFilter(pSrc, fnw)))
	{
		m_report.SetWindowText(_T("Sorry, can't save this file, press cancel"));
		return FALSE;
	}

	CComQIPtr<IBaseFilter> pMid = new CStreamDriveThruFilter(NULL, &hr);
	if(FAILED(pGB->AddFilter(pMid, L"StreamDriveThru")))
	{
		m_report.SetWindowText(_T("Error"));
		return FALSE;
	}

	CComQIPtr<IBaseFilter> pDst;
	pDst.CoCreateInstance(CLSID_FileWriter);
	CComQIPtr<IFileSinkFilter2> pFSF = pDst;
	pFSF->SetFileName(CStringW(m_out), NULL);
	pFSF->SetMode(AM_FILE_OVERWRITE);
	if(FAILED(pGB->AddFilter(pDst, L"File Writer")))
	{
		m_report.SetWindowText(_T("Error"));
		return FALSE;
	}

	hr = pGB->Connect(
		GetFirstPin((pSrc), PINDIR_OUTPUT), 
		GetFirstPin((pMid), PINDIR_INPUT));

	hr = pGB->Connect(
		GetFirstPin((pMid), PINDIR_OUTPUT), 
		GetFirstPin((pDst), PINDIR_INPUT));

pMS = pMid;

	pMC->Run();

	m_nIDTimerEvent = SetTimer(1, 1000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSaveDlg::OnBnClickedCancel()
{
	if(pMC) pMC->Stop();

	OnCancel();
}

LRESULT CSaveDlg::OnGraphNotify(WPARAM wParam, LPARAM lParam)
{
	LONG evCode, evParam1, evParam2;
    while(pME && SUCCEEDED(pME->GetEvent(&evCode, (LONG_PTR*)&evParam1, (LONG_PTR*)&evParam2, 0)))
    {
		HRESULT hr = pME->FreeEventParams(evCode, evParam1, evParam2);

        if(EC_COMPLETE == evCode)
        {
			EndDialog(IDOK);
		}
		else if(EC_ERRORABORT == evCode)
		{
			TRACE(_T("CSaveDlg::OnGraphNotify / EC_ERRORABORT, hr = %08x\n"), (HRESULT)evParam1);
			m_report.SetWindowText(_T("Copying unexpectedly terminated!"));
		}
	}

	return 0;
}

void CSaveDlg::OnTimer(UINT nIDEvent)
{
	if(nIDEvent == m_nIDTimerEvent && pGB)
	{
		if(pMS)
		{
			CString str;
			REFERENCE_TIME pos = 0, dur = 0;
			pMS->GetCurrentPosition(&pos);
			pMS->GetDuration(&dur);
			REFERENCE_TIME time = 0;
			CComQIPtr<IMediaSeeking>(pGB)->GetCurrentPosition(&time);
			REFERENCE_TIME speed = time > 0 ? pos*10000000/time / 1024 : 0i64;
			str.Format(_T("%I64d/%I64d KB, %I64d KB/s, %I64d s"), 
				pos/1024, dur/1024, speed, speed > 0 ? (dur-pos)/1024 / speed : 0);
			m_report.SetWindowText(str);

			m_progress.SetPos(dur > 0 ? (int)(100*pos/dur) : 0);
		}
	}

	CCmdUIDialog::OnTimer(nIDEvent);
}
