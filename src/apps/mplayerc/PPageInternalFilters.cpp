

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

// PPageInternalFilters.cpp : implementation file
//

#include "stdafx.h"
#include "mplayerc.h"
#include "PPageInternalFilters.h"
#include "ComPropertySheet.h"
#include "..\..\filters\filters.h"
#include "internal_filter_config.h"

static struct filter_t
{
	LPCTSTR label; 
	int type; 
	int flag; 
	UINT nHintID; 
	CUnknown* (WINAPI * CreateInstance)(LPUNKNOWN lpunk, HRESULT* phr);
}
s_filters[] = 
{
	{_T("AVI"), 0, SRC_AVI, IDS_SRC_AVI, NULL},
	{_T("CDDA (Audio CD)"), 0, SRC_CDDA, IDS_SRC_CDDA, NULL},
	{_T("CDXA (VCD/SVCD/XCD)"), 0, SRC_CDXA, IDS_SRC_CDXA, NULL},
	__if_exists(CDiracSplitterFilter) {{_T("Dirac"), 0, SRC_DIRAC, IDS_SRC_DIRAC, NULL},}
	{_T("DirectShow Media"), 0, SRC_DSM, IDS_SRC_DSM, NULL},
	{_T("DTS/AC3"), 0, SRC_DTSAC3, IDS_SRC_DTSAC3, NULL},
	{_T("DVD Video Title Set"), 0, SRC_VTS, IDS_SRC_VTS, NULL},
	{_T("DVD2AVI Project File"), 0, SRC_D2V, IDS_SRC_D2V, NULL},
	{_T("FLI/FLC"), 0, SRC_FLIC, IDS_SRC_FLIC, NULL},
	{_T("FLV"), 0, SRC_FLV, IDS_SRC_FLV, NULL},
	{_T("Matroska"), 0, SRC_MATROSKA, IDS_SRC_MATROSKA, NULL},
	{_T("MP4/MOV"), 0, SRC_MP4, IDS_SRC_MP4, NULL},
	{_T("MPEG Audio"), 0, SRC_MPA, IDS_SRC_MPA, NULL},
	{_T("MPEG PS/TS/PVA"), 0, SRC_MPEG, 0, NULL},
	__if_exists(CNutSplitterFilter) {{_T("Nut"), 0, SRC_NUT, IDS_SRC_NUT, NULL},}
	{_T("Ogg"), 0, SRC_OGG, IDS_SRC_OGG, NULL},
	{_T("RealMedia"), 0, SRC_REALMEDIA, IDS_SRC_REALMEDIA, NULL},
	{_T("RoQ"), 0, SRC_ROQ, IDS_SRC_ROQ, NULL},
	{_T("SHOUTcast"), 0, SRC_SHOUTCAST, IDS_SRC_SHOUTCAST, NULL},
	__if_exists(CRadGtSplitterFilter) {{_T("Smacker/Bink"), 0, SRC_RADGT, IDS_SRC_RADGT, NULL},}
	{_T("AAC"), 1, TRA_AAC, IDS_TRA_AAC, CreateInstance<CMpaDecFilter>},
	{_T("AC3"), 1, TRA_AC3, IDS_TRA_AC3, CreateInstance<CMpaDecFilter>},
	{_T("DTS"), 1, TRA_DTS, IDS_TRA_DTS, CreateInstance<CMpaDecFilter>},
	__if_exists(CDiracVideoDecoder) {{_T("Dirac"), 1, TRA_DIRAC, IDS_TRA_DIRAC, NULL},}
	//{_T("FLV4"), 1, TRA_FLV4, 0, NULL},
	{_T("LPCM"), 1, TRA_LPCM, IDS_TRA_LPCM, CreateInstance<CMpaDecFilter>},
	{_T("MPEG Audio"), 1, TRA_MPA, IDS_TRA_MPA, CreateInstance<CMpaDecFilter>},
	{_T("MPEG-1 Video"), 1, TRA_MPEG1, IDS_TRA_MPEG1, CreateInstance<CMpeg2DecFilter>},
	{_T("MPEG-2 Video"), 1, TRA_MPEG2, IDS_TRA_MPEG2, CreateInstance<CMpeg2DecFilter>},
	{_T("PS2 Audio (PCM/ADPCM)"), 1, TRA_PS2AUD, IDS_TRA_PS2AUD, CreateInstance<CMpaDecFilter>},
	{_T("RealVideo"), 1, TRA_RV, IDS_TRA_RV, NULL},
	{_T("RealAudio"), 1, TRA_RA, IDS_TRA_RA, NULL},
	{_T("Vorbis"), 1, TRA_VORBIS, 0, NULL /* TODO: CreateInstance<CMpaDecFilter>*/},
	//{_T("VP62"), 1, TRA_VP62, 0, NULL},

#if INTERNAL_DECODER_H264_DXVA
	{_T("H264/AVC (DXVA)"), 2, MPCDXVA_H264, IDS_TRA_FFMPEG, CreateInstance<CMPCVideoDecFilter>},
#endif
#if INTERNAL_DECODER_H264
	{_T("H264/AVC (FFmpeg)"), 3, FFM_H264, IDS_TRA_FFMPEG, CreateInstance<CMPCVideoDecFilter>},
#endif
#if INTERNAL_DECODER_VC1_DXVA
	{_T("VC1 (DXVA)"), 2, MPCDXVA_VC1, IDS_TRA_FFMPEG, CreateInstance<CMPCVideoDecFilter>},
#endif
#if INTERNAL_DECODER_VC1
	{_T("VC1 (FFmpeg)"), 3, FFM_VC1, IDS_TRA_FFMPEG, CreateInstance<CMPCVideoDecFilter>},
#endif
#if INTERNAL_DECODER_XVID
	{_T("Xvid/MPEG-4"), 3, FFM_XVID, IDS_TRA_FFMPEG, CreateInstance<CMPCVideoDecFilter>},
#endif
#if INTERNAL_DECODER_DIVX
	{_T("DivX"), 3, FFM_DIVX, IDS_TRA_FFMPEG, CreateInstance<CMPCVideoDecFilter>},
#endif
#if INTERNAL_DECODER_MSMPEG4
	{_T("MS MPEG-4"), 3, FFM_MSMPEG4, IDS_TRA_FFMPEG, CreateInstance<CMPCVideoDecFilter>},
#endif
#if INTERNAL_DECODER_FLV
	{_T("FLV1/4"), 3, FFM_FLV4, IDS_TRA_FFMPEG, CreateInstance<CMPCVideoDecFilter>},
#endif
#if INTERNAL_DECODER_VP6
	{_T("VP5/6"), 3, FFM_VP62, IDS_TRA_FFMPEG, CreateInstance<CMPCVideoDecFilter>},	
#endif
#if INTERNAL_DECODER_WMV
	{_T("WMV1/2/3"), 3, FFM_WMV, IDS_TRA_FFMPEG, CreateInstance<CMPCVideoDecFilter>},	
#endif
#if INTERNAL_DECODER_SVQ
	{_T("SVQ1/3"), 3, FFM_SVQ3, IDS_TRA_FFMPEG, CreateInstance<CMPCVideoDecFilter>},
#endif
#if INTERNAL_DECODER_H263
	{_T("H263"), 3, FFM_H263, IDS_TRA_FFMPEG, CreateInstance<CMPCVideoDecFilter>},
#endif
#if INTERNAL_DECODER_AMVV
	{_T("AMV video"), 3, FFM_AMVV, IDS_TRA_FFMPEG, CreateInstance<CMPCVideoDecFilter>},
#endif
#if INTERNAL_DECODER_THEORA
	{_T("Theora"), 3, FFM_THEORA, IDS_TRA_FFMPEG, CreateInstance<CMPCVideoDecFilter>},
#endif
};

IMPLEMENT_DYNAMIC(CPPageInternalFiltersListBox, CCheckListBox)
CPPageInternalFiltersListBox::CPPageInternalFiltersListBox()
	: CCheckListBox()
{
}

void CPPageInternalFiltersListBox::PreSubclassWindow()
{
	__super::PreSubclassWindow();
	EnableToolTips(TRUE);
}

INT_PTR CPPageInternalFiltersListBox::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	BOOL b = FALSE;
	int row = ItemFromPoint(point, b);
	if(row < 0) return -1;

	CRect r;
	GetItemRect(row, r);
	pTI->rect = r;
	pTI->hwnd = m_hWnd;
	pTI->uId = (UINT)row;
	pTI->lpszText = LPSTR_TEXTCALLBACK;
	pTI->uFlags |= TTF_ALWAYSTIP;

	return pTI->uId;
}

BEGIN_MESSAGE_MAP(CPPageInternalFiltersListBox, CCheckListBox)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNotify)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNotify)
END_MESSAGE_MAP()

BOOL CPPageInternalFiltersListBox::OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;

	filter_t* f = (filter_t*)GetItemDataPtr(pNMHDR->idFrom);
	if(f->nHintID == 0) return FALSE;

	::SendMessage(pNMHDR->hwndFrom, TTM_SETMAXTIPWIDTH, 0, (LPARAM)(INT)1000);

	static CStringA m_strTipTextA;
	static CStringW m_strTipTextW;
	
	m_strTipTextA = CString(MAKEINTRESOURCE(f->nHintID));
	m_strTipTextW = CString(MAKEINTRESOURCE(f->nHintID));

	if(pNMHDR->code == TTN_NEEDTEXTA) pTTTA->lpszText = (LPSTR)(LPCSTR)m_strTipTextA;
	else pTTTW->lpszText = (LPWSTR)(LPCWSTR)m_strTipTextW;

	*pResult = 0;

	return TRUE;    // message was handled
}

void CPPageInternalFiltersListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	CFont* pOldFont = NULL;
	
	if(((filter_t*)lpDrawItemStruct->itemData)->CreateInstance)
	{
		if(!(HFONT)m_bold)
		{
			CFont* pFont = pDC->GetCurrentFont();

			LOGFONT lf;
			pFont->GetLogFont(&lf);
			lf.lfWeight = FW_BOLD;

			m_bold.CreateFontIndirect(&lf);
		}

		if((HFONT)m_bold)
		{
			pOldFont = pDC->SelectObject(&m_bold);
		}
	}

	__super::DrawItem(lpDrawItemStruct);

	if(pOldFont)
	{
		pDC->SelectObject(pOldFont);
	}
}

// CPPageInternalFilters dialog

IMPLEMENT_DYNAMIC(CPPageInternalFilters, CPPageBase)
CPPageInternalFilters::CPPageInternalFilters()
	: CPPageBase(CPPageInternalFilters::IDD, CPPageInternalFilters::IDD)
{
}

CPPageInternalFilters::~CPPageInternalFilters()
{
}


void CPPageInternalFilters::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listSrc);
	DDX_Control(pDX, IDC_LIST2, m_listTra);
}

BEGIN_MESSAGE_MAP(CPPageInternalFilters, CPPageBase)
	ON_LBN_DBLCLK(IDC_LIST1, &CPPageInternalFilters::OnLbnDblclkList1)
	ON_LBN_DBLCLK(IDC_LIST2, &CPPageInternalFilters::OnLbnDblclkList2)
END_MESSAGE_MAP()

// CPPageInternalFilters message handlers

BOOL CPPageInternalFilters::OnInitDialog()
{
	__super::OnInitDialog();

	AppSettings& s = AfxGetAppSettings();

	for(int i = 0; i < countof(s_filters); i++)
	{
		CCheckListBox* l;
		UINT* pflags;

		switch(s_filters[i].type)
		{
		case 0: // source filter
			l = &m_listSrc;
			pflags = &s.SrcFilters;
			break;
		case 1: // decoder
			l = &m_listTra;
			pflags = &s.TraFilters;
			break;
		case 2: // dxva decoder
			l = &m_listTra;
			pflags = &s.DXVAFilters;
			break;
		case 3: // ffmpeg decoder
			l = &m_listTra;
			pflags = &s.FFmpegFilters;
			break;
		default:
			l = NULL;
			pflags = NULL;
		}

		if(l && pflags)
		{
			int Index = l->AddString(s_filters[i].label);
			l->SetCheck(Index, !!(*pflags & s_filters[i].flag));
			l->SetItemDataPtr(Index, &s_filters[i]);
		}
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPPageInternalFilters::OnApply()
{
	UpdateData();

	AppSettings& s = AfxGetAppSettings();

	s.SrcFilters = s.TraFilters = 0;

	CList<filter_t*> fl;
	for(int i = 0; i < m_listSrc.GetCount(); i++)
		if(m_listSrc.GetCheck(i))
			fl.AddTail((filter_t*)m_listSrc.GetItemDataPtr(i));
	for(int i = 0; i < m_listTra.GetCount(); i++)
		if(m_listTra.GetCheck(i))
			fl.AddTail((filter_t*)m_listTra.GetItemDataPtr(i));

	POSITION pos = fl.GetHeadPosition();
	while(pos)
	{
		filter_t* f = fl.GetNext(pos);

		UINT* pflags = 
			f->type == 0 ? &s.SrcFilters : 
			f->type == 1 ? &s.TraFilters : 
			NULL; 

		if(pflags)
			*pflags |= f->flag;
	}

	return __super::OnApply();
}

void CPPageInternalFilters::ShowPPage(CPPageInternalFiltersListBox& l)
{
	int i = l.GetCurSel();
	if(i < 0) return;

	filter_t* f = (filter_t*)l.GetItemDataPtr(i);
	if(!f || !f->CreateInstance) return;

	HRESULT hr;
	CUnknown* pObj = f->CreateInstance(NULL, &hr);
	if(!pObj) return;

	CComPtr<IUnknown> pUnk = (IUnknown*)(INonDelegatingUnknown*)pObj;

	if(SUCCEEDED(hr))
	{
		if(CComQIPtr<ISpecifyPropertyPages> pSPP = pUnk)
		{
			CComPropertySheet ps(ResStr(IDS_PROPSHEET_PROPERTIES), this);
			ps.AddPages(pSPP);
			ps.DoModal();
		}
	}
}

void CPPageInternalFilters::OnLbnDblclkList1()
{
	ShowPPage(m_listSrc);
}

void CPPageInternalFilters::OnLbnDblclkList2()
{
	ShowPPage(m_listTra);
}
