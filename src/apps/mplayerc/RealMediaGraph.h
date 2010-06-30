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

#pragma once

#include "BaseGraph.h"

#include "DX7AllocatorPresenter.h"
#include "DX9AllocatorPresenter.h"

#include "..\..\..\include\RealMedia\pntypes.h"
#include "..\..\..\include\RealMedia\pnwintyp.h"
#include "..\..\..\include\RealMedia\pncom.h"
#include "..\..\..\include\RealMedia\rmapckts.h"
#include "..\..\..\include\RealMedia\rmacomm.h"
#include "..\..\..\include\RealMedia\rmamon.h"
#include "..\..\..\include\RealMedia\rmafiles.h"
#include "..\..\..\include\RealMedia\rmaengin.h"
#include "..\..\..\include\RealMedia\rmacore.h"
#include "..\..\..\include\RealMedia\rmaclsnk.h"
#include "..\..\..\include\RealMedia\rmaerror.h"
#include "..\..\..\include\RealMedia\rmaauth.h"
#include "..\..\..\include\RealMedia\rmawin.h"
#include "..\..\..\include\RealMedia\rmasite2.h"
#include "..\..\..\include\RealMedia\rmaausvc.h"
#include "..\..\..\include\RealMedia\rmavsurf.h"

namespace DSObjects
{

class CRealMediaGraph;

class CRealMediaPlayer
	: public CUnknown
	, public IRMAErrorSink
	, public IRMAClientAdviseSink
	, public IRMAAuthenticationManager
	, public IRMASiteSupplier
	, public IRMAPassiveSiteWatcher
	, public IRMAAudioHook

{
protected:
	friend class CRealMediaGraph;
	CRealMediaGraph* m_pRMG; // IMPORTANT: do not ever AddRef on this from here

	HWND m_hWndParent;
	CSize m_VideoSize;
	bool m_fVideoSizeChanged;

	//

	DWORD m_wndStyle;
	CPlayerWindow m_wndWindowFrame, m_wndDestFrame;

	//

	FPRMCREATEENGINE		m_fpCreateEngine;
	FPRMCLOSEENGINE	 		m_fpCloseEngine;
	FPRMSETDLLACCESSPATH	m_fpSetDLLAccessPath;
	HMODULE					m_hRealMediaCore;

	CComPtr<IRMAClientEngine> m_pEngine;
    CComPtr<IRMAPlayer> m_pPlayer;
	CComQIPtr<IRMAAudioPlayer, &IID_IRMAAudioPlayer> m_pAudioPlayer;
	CComPtr<IRMAVolume> m_pVolume;
    CComQIPtr<IRMASiteManager, &IID_IRMASiteManager> m_pSiteManager;
    CComQIPtr<IRMACommonClassFactory, &IID_IRMACommonClassFactory> m_pCommonClassFactory;

	CComQIPtr<IRMASite, &IID_IRMASite> m_pTheSite;
	CComQIPtr<IRMASite2, &IID_IRMASite2> m_pTheSite2;
	CMap<UINT32, UINT32&, IRMASite*, IRMASite*&> m_CreatedSites;

	//

	OAFilterState m_State, m_UserState;
	REFERENCE_TIME m_nCurrent, m_nDuration;

	UINT16 m_unPercentComplete;

	//

public:
	CRealMediaPlayer(HWND hWndParent, CRealMediaGraph* pRMG);
	virtual ~CRealMediaPlayer();

    DECLARE_IUNKNOWN;
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

	bool Init();
	void Deinit();

	virtual CSize GetVideoSize() {return(m_VideoSize);}
	virtual void SetWindowRect(CRect r) {}
	virtual void SetDestRect(CRect r) {}
	virtual bool CreateSite(IRMASite** pSite) = 0;
	virtual void DestroySite(IRMASite* pSite) = 0;

	// IRMAErrorSink
    STDMETHODIMP ErrorOccurred(const UINT8 unSeverity, const UINT32 ulRMACode, const UINT32 ulUserCode, const char* pUserString, const char* pMoreInfoURL);

    // IRMAClientAdviseSink
    STDMETHODIMP OnPosLength(UINT32 ulPosition, UINT32 ulLength);
    STDMETHODIMP OnPresentationOpened();
    STDMETHODIMP OnPresentationClosed();
    STDMETHODIMP OnStatisticsChanged();
    STDMETHODIMP OnPreSeek(UINT32 ulOldTime, UINT32 ulNewTime);
    STDMETHODIMP OnPostSeek(UINT32 ulOldTime, UINT32 ulNewTime);
    STDMETHODIMP OnStop();
    STDMETHODIMP OnPause(UINT32 ulTime);
    STDMETHODIMP OnBegin(UINT32 ulTime);
    STDMETHODIMP OnBuffering(UINT32 ulFlags, UINT16 unPercentComplete);
    STDMETHODIMP OnContacting(const char* pHostName);

	// IRMAAuthenticationManager
    STDMETHODIMP HandleAuthenticationRequest(IRMAAuthenticationManagerResponse* pResponse);

	// IRMASiteSupplier
    STDMETHODIMP SitesNeeded(UINT32 uRequestID, IRMAValues* pSiteProps);
    STDMETHODIMP SitesNotNeeded(UINT32 uRequestID);
    STDMETHODIMP BeginChangeLayout();
    STDMETHODIMP DoneChangeLayout();

	// IRMAPassiveSiteWatcher
    STDMETHODIMP PositionChanged(PNxPoint* pos);
	STDMETHODIMP SizeChanged(PNxSize* size);

	// IRMAAudioHook
	STDMETHODIMP OnBuffer(RMAAudioData* pAudioInData, RMAAudioData* pAudioOutData);
	STDMETHODIMP OnInit(RMAAudioFormat* pFormat);
};

class CRealMediaPlayerWindowed
	: public CRealMediaPlayer
{
public:
	CRealMediaPlayerWindowed(HWND hWndParent, CRealMediaGraph* pRMG);
	virtual ~CRealMediaPlayerWindowed();

	void SetWindowRect(CRect r);
	void SetDestRect(CRect r);

	bool CreateSite(IRMASite** pSite);
	void DestroySite(IRMASite* pSite);
};

class CRealMediaPlayerWindowless
	: public CRealMediaPlayer
{
	CComPtr<ISubPicAllocatorPresenterRender> m_pRMAP;

public:
	CRealMediaPlayerWindowless(HWND hWndParent, CRealMediaGraph* pRMG);
	virtual ~CRealMediaPlayerWindowless();

    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

	bool CreateSite(IRMASite** pSite);
	void DestroySite(IRMASite* pSite);

	STDMETHODIMP SizeChanged(PNxSize* size);
};

class CRealMediaGraph : public CBaseGraph
{
	CRealMediaPlayer* m_pRMP; // TODO: access m_pRMP through a private interface

	CStringW m_fn;

public:
	CRealMediaGraph(HWND hWndParent, HRESULT& hr); // in windowless mode IVideoWindow::* will return E_NOTIMPL, use ISubPicAllocatorPresenter instead
	virtual ~CRealMediaGraph();

    DECLARE_IUNKNOWN;
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

protected:
	// IGraphBuilder
    STDMETHODIMP RenderFile(LPCWSTR lpcwstrFile, LPCWSTR lpcwstrPlayList);

	// IMediaControl
    STDMETHODIMP Run();
    STDMETHODIMP Pause();
    STDMETHODIMP Stop();
	STDMETHODIMP GetState(LONG msTimeout, OAFilterState* pfs);

	// IMediaSeeking
	STDMETHODIMP GetDuration(LONGLONG* pDuration);
	STDMETHODIMP GetCurrentPosition(LONGLONG* pCurrent);
	STDMETHODIMP SetPositions(LONGLONG* pCurrent, DWORD dwCurrentFlags, LONGLONG* pStop, DWORD dwStopFlags);

	// IVideoWindow
    STDMETHODIMP SetWindowPosition(long Left, long Top, long Width, long Height);

	// IBasicVideo
    STDMETHODIMP SetDestinationPosition(long Left, long Top, long Width, long Height);
    STDMETHODIMP GetVideoSize(long* pWidth, long* pHeight);

	// IBasicAudio
    STDMETHODIMP put_Volume(long lVolume);
    STDMETHODIMP get_Volume(long* plVolume);

	// IAMOpenProgress
	STDMETHODIMP QueryProgress(LONGLONG* pllTotal, LONGLONG* pllCurrent);

	// IGraphEngine
	STDMETHODIMP_(engine_t) GetEngine();
};

}
using namespace DSObjects;