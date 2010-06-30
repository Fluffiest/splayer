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

#include "stdafx.h"
#include "shockwavegraph.h"
#include "resource.h"
#include "..\..\DSUtil\DSUtil.h"
#include "..\..\svplib\svplib.h"

CShockwaveGraph::CShockwaveGraph(HWND hParent, HRESULT& hr)
	: m_fs(State_Stopped)
{
	hr = S_OK;

	if(!m_wndWindowFrame.Create(NULL, NULL, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
		CRect(0, 0, 0, 0), CWnd::FromHandle(hParent), 0, NULL))
	{
		hr = E_FAIL;
		return;
	}

	if(!m_wndDestFrame.Create(NULL, WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, 
		CRect(0, 0, 0, 0), &m_wndWindowFrame, 0))
	{
		hr = E_FAIL;
		return;
	}
}

CShockwaveGraph::~CShockwaveGraph()
{
	m_wndDestFrame.DestroyWindow();
	m_wndWindowFrame.DestroyWindow();
}

// IGraphBuilder
STDMETHODIMP CShockwaveGraph::RenderFile(LPCWSTR lpcwstrFile, LPCWSTR lpcwstrPlayList)
{
	try {m_wndDestFrame.LoadMovie(0, CString(lpcwstrFile));m_wndDestFrame.put_ScaleMode(0);}
	catch(CException* e) {e->Delete(); return E_FAIL;}
	return S_OK;
}

// IMediaControl
STDMETHODIMP CShockwaveGraph::Run()
{
	try {if(m_fs != State_Running) m_wndDestFrame.Play();}
	catch(CException* e) {e->Delete(); return E_FAIL;}
	m_fs = State_Running;
	m_wndWindowFrame.EnableWindow();
//	m_wndDestFrame.EnableWindow();
	return S_OK;
}
STDMETHODIMP CShockwaveGraph::Pause()
{
	try {if(m_fs == State_Running) m_wndDestFrame.Stop();}
	catch(CException* e) {e->Delete(); return E_FAIL;}
	m_fs = State_Paused;
	return S_OK;
}
STDMETHODIMP CShockwaveGraph::Stop()
{
	try {m_wndDestFrame.Stop();}
	catch(CException* e) {e->Delete(); return E_FAIL;}
	m_fs = State_Stopped;
	return S_OK;
}
STDMETHODIMP CShockwaveGraph::GetState(LONG msTimeout, OAFilterState* pfs)
{
	OAFilterState fs = m_fs;

	try
	{
		if(m_wndDestFrame.IsPlaying() && m_fs == State_Stopped) m_fs = State_Running;
		else if(!m_wndDestFrame.IsPlaying() && m_fs == State_Running) m_fs = State_Stopped;
		fs = m_fs;
	}
	catch(CException* e)
	{
		e->Delete();
		return E_FAIL;
	}

	return pfs ? *pfs = fs, S_OK : E_POINTER;
}

// IMediaSeeking
STDMETHODIMP CShockwaveGraph::IsFormatSupported(const GUID* pFormat)
{
	return !pFormat ? E_POINTER : *pFormat == TIME_FORMAT_FRAME ? S_OK : S_FALSE;
}
STDMETHODIMP CShockwaveGraph::GetTimeFormat(GUID* pFormat)
{
	return pFormat ? *pFormat = TIME_FORMAT_FRAME, S_OK : E_POINTER;
}
STDMETHODIMP CShockwaveGraph::GetDuration(LONGLONG* pDuration)
{
	CheckPointer(pDuration, E_POINTER);
	*pDuration = 0;
	try {if(m_wndDestFrame.get_ReadyState() >= READYSTATE_COMPLETE) *pDuration = m_wndDestFrame.get_TotalFrames();}
	catch(CException* e) {e->Delete(); return E_FAIL;}
	return S_OK;
}
STDMETHODIMP CShockwaveGraph::GetCurrentPosition(LONGLONG* pCurrent)
{
	CheckPointer(pCurrent, E_POINTER);
	*pCurrent = 0;
	try {if(m_wndDestFrame.get_ReadyState() >= READYSTATE_COMPLETE) *pCurrent = m_wndDestFrame.get_FrameNum();}
	catch(CException* e) {e->Delete(); return E_FAIL;}
	return S_OK;
}
STDMETHODIMP CShockwaveGraph::SetPositions(LONGLONG* pCurrent, DWORD dwCurrentFlags, LONGLONG* pStop, DWORD dwStopFlags)
{
	if(dwCurrentFlags&AM_SEEKING_AbsolutePositioning)
	{
		m_wndDestFrame.put_FrameNum(*pCurrent);

		if(m_fs == State_Running && !m_wndDestFrame.IsPlaying()) 
			m_wndDestFrame.Play();
		else if((m_fs == State_Paused || m_fs == State_Stopped) && m_wndDestFrame.IsPlaying())
			m_wndDestFrame.Stop();

		m_wndDestFrame.put_Quality(1); // 0=Low, 1=High, 2=AutoLow, 3=AutoHigh

		return S_OK;
	}

	return E_INVALIDARG;
}

// IVideoWindow
STDMETHODIMP CShockwaveGraph::put_Visible(long Visible)
{
	if(IsWindow(m_wndDestFrame.m_hWnd))
		m_wndDestFrame.ShowWindow(Visible == OATRUE ? SW_SHOWNORMAL : SW_HIDE);
	return S_OK;
}
STDMETHODIMP CShockwaveGraph::get_Visible(long* pVisible)
{
	return pVisible ? *pVisible = (m_wndDestFrame.IsWindowVisible() ? OATRUE : OAFALSE), S_OK : E_POINTER;
}
STDMETHODIMP CShockwaveGraph::SetWindowPosition(long Left, long Top, long Width, long Height)
{
	if(IsWindow(m_wndWindowFrame.m_hWnd)){
		m_wndWindowFrame.MoveWindow(Left, Top, Width, Height);
	}

	return S_OK;
}

// IBasicVideo
STDMETHODIMP CShockwaveGraph::SetDestinationPosition(long Left, long Top, long Width, long Height)// {return E_NOTIMPL;}
{
	if(IsWindow(m_wndDestFrame.m_hWnd)){
		m_wndDestFrame.MoveWindow(Left, Top, Width, Height);
	}
	return S_OK;
}
STDMETHODIMP CShockwaveGraph::GetVideoSize(long* pWidth, long* pHeight)
{
	if(!pWidth || !pHeight) return E_POINTER;

	CRect r;
	m_wndWindowFrame.GetWindowRect(r);
	if(!r.IsRectEmpty() && 0 )
	{
		*pWidth = r.Width();
		*pHeight = r.Height();
	}
	else
	{
		// no call exists to determine these...
		*pWidth = max(50,  _wtof(m_wndDestFrame.TGetProperty(_T("_level0"),  8)) );//m_wndDestFrame.get_; 
		*pHeight = max(50, _wtof(m_wndDestFrame.TGetProperty(_T("_level0"),  9)) );

		//SVP_LogMsg5(_T("SWF width %d %s height %d %s ") ,*pWidth,m_wndDestFrame.TGetProperty(_T("_level0"),  8) , *pHeight , m_wndDestFrame.TGetProperty(_T("_level0"),  9));
		NotifyEvent(EC_BG_AUDIO_CHANGED, 2, 0);
	}
	
	return S_OK;
}

#include <math.h>

// IBasicAudio
STDMETHODIMP CShockwaveGraph::put_Volume(long lVolume)
{
	lVolume = (lVolume == -10000) ? 0 : (int)pow(10.0, ((double)lVolume)/5000+2);
	lVolume = lVolume*0x10000/100;
	lVolume = max(min(lVolume, 0xffff), 0);
	waveOutSetVolume(0, (lVolume<<16)|lVolume);

	return S_OK;
}
STDMETHODIMP CShockwaveGraph::get_Volume(long* plVolume)
{
	CheckPointer(plVolume, E_POINTER);

	waveOutGetVolume(0, (DWORD*)plVolume);
	*plVolume = (*plVolume&0xffff + ((*plVolume>>16)&0xffff)) / 2;
	*plVolume = *plVolume*100/0x10000;
	*plVolume = (int)((log10(1.0*(*plVolume))-2)*5000);
	*plVolume = max(min(*plVolume, 0), -10000);

	return S_OK;
}

// IAMOpenProgress
STDMETHODIMP CShockwaveGraph::QueryProgress(LONGLONG* pllTotal, LONGLONG* pllCurrent)
{
	*pllTotal = 100;
	*pllCurrent = m_wndDestFrame.PercentLoaded();
	return S_OK;
}

// IGraphEngine
STDMETHODIMP_(engine_t) CShockwaveGraph::GetEngine() {return ShockWave;}
