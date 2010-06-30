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

// CPPageLogo.cpp : implementation file
//

#include "stdafx.h"
#include "mplayerc.h"
#include "MainFrm.h"
#include "PPageLogo.h"

// CPPageLogo dialog

IMPLEMENT_DYNAMIC(CPPageLogo, CPPageBase)
CPPageLogo::CPPageLogo()
	: CPPageBase(CPPageLogo::IDD, CPPageLogo::IDD)
	, m_intext(0)
	, m_logofn(_T(""))
	, m_author(_T(""))
{
	//m_logoids.AddTail(IDF_LOGO0);
	//m_logoids.AddTail(IDF_LOGO1);
	//m_logoids.AddTail(IDF_LOGO2);
	//m_logoids.AddTail(IDF_LOGO3);
	//m_logoids.AddTail(IDF_LOGO4);
	//m_logoids.AddTail(IDF_LOGO5);
	//m_logoids.AddTail(IDF_LOGO6);
	m_logoids.AddTail(IDF_LOGO7);
}

CPPageLogo::~CPPageLogo()
{
}

void CPPageLogo::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO1, m_intext);
	DDX_Text(pDX, IDC_LOGOFILENAME, m_logofn);
	DDX_Control(pDX, IDC_LOGOPREVIEW, m_logopreview);
	DDX_Text(pDX, IDC_AUTHOR, m_author);
	DDX_Control(pDX, IDC_CHECK1, m_chkKeepLogoAS);
	DDX_Control(pDX, IDC_CHECK2, m_chkLogoStrech);
}


BEGIN_MESSAGE_MAP(CPPageLogo, CPPageBase)
	ON_BN_CLICKED(IDC_RADIO1, OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnBnClickedRadio2)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
END_MESSAGE_MAP()


// CPPageLogo message handlers

BOOL CPPageLogo::OnInitDialog()
{
	__super::OnInitDialog();

	AppSettings& s = AfxGetAppSettings();

	m_intext = s.logoext?1:0;
	m_logofn = s.logofn;
	m_logoidpos = NULL;
	
	m_chkLogoStrech.SetCheck(s.logostretch & 2);
	m_chkKeepLogoAS.SetCheck(s.logostretch & 1);
			
	UpdateData(FALSE);

	for(POSITION pos = m_logoids.GetHeadPosition(); pos; m_logoids.GetNext(pos)) 
	{
		if(m_logoids.GetAt(pos) == s.logoid)
		{
			m_logoidpos = pos;
			break;
		}
	}

	if(!m_logoidpos){
		m_logoidpos = m_logoids.GetHeadPosition();
	}
	if(!m_intext) OnBnClickedRadio1();
	else OnBnClickedRadio2();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPPageLogo::OnApply()
{
	UpdateData();

	AppSettings& s = AfxGetAppSettings();
	
	s.logostretch = m_chkKeepLogoAS.GetCheck()  | m_chkLogoStrech.GetCheck() << 1;
	if(s.logostretch >= 3 ){
		s.logostretch = 3;
	}
	s.logoext = !!m_intext;
	s.logofn = m_logofn;
	s.logoid = m_logoids.GetAt(m_logoidpos);

	((CMainFrame*)AfxGetMainWnd())->m_wndView.LoadLogo();

	return __super::OnApply();
}


void CPPageLogo::OnBnClickedRadio1()
{
	ASSERT(m_logoidpos);

	m_author.Empty();

	m_logobm.Destroy();
	UINT id = m_logoids.GetAt(m_logoidpos);
	if(IDF_LOGO0 != id)
	{
		m_logobm.LoadFromResource(id);
		// m_logobm.LoadFromResource(::AfxGetInstanceHandle(), id);
		if(!m_author.LoadString(id)) m_author = _T("Author unknown. Contact me if you made this logo!");
	}
	m_logopreview.SetBitmap(m_logobm);
	Invalidate();

	m_intext = 0;
	UpdateData(FALSE);

	SetModified();
}

void CPPageLogo::OnBnClickedRadio2()
{
	UpdateData();

	m_author.Empty();

	m_logobm.Destroy();
	if(AfxGetAppSettings().fXpOrBetter)
		m_logobm.Load(m_logofn);
	else if(HANDLE h = LoadImage(NULL, m_logofn, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE))
		m_logobm.Attach((HBITMAP)h);
	m_logopreview.SetBitmap(m_logobm);
	Invalidate();

	m_intext = 1;
	UpdateData(FALSE);

	SetModified();
}

void CPPageLogo::OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	if(pNMUpDown->iDelta < 0)
	{
		m_logoids.GetNext(m_logoidpos);
		if(!m_logoidpos) m_logoidpos = m_logoids.GetHeadPosition();
	}
	else
	{
		m_logoids.GetPrev(m_logoidpos);
		if(!m_logoidpos) m_logoidpos = m_logoids.GetTailPosition();
	}

	OnBnClickedRadio1();

	*pResult = 0;
}

void CPPageLogo::OnBnClickedButton2()
{
	CFileDialog dlg(TRUE, NULL, m_logofn, 
		OFN_EXPLORER|OFN_ENABLESIZING|OFN_HIDEREADONLY, 
		AfxGetAppSettings().fXpOrBetter 
		? _T("Images (*.bmp;*.jpg;*.gif;*.png)|*.bmp;*.jpg;*.gif;*.png|All files (*.*)|*.*||")
		: _T("Images (*.bmp)|*.bmp|All files (*.*)|*.*||")
		, this, 0);

	if(dlg.DoModal() == IDOK)
	{
		m_logofn = dlg.GetPathName();
		UpdateData(FALSE);
		OnBnClickedRadio2();
	}
}
