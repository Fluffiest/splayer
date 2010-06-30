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
#include <windows.h>
#include <commdlg.h>
#include "mplayerc.h"
#include "mainfrm.h"
#include "TextPassThruFilter.h"
#include "..\..\..\include\moreuuids.h"
#include "..\..\DSUtil\DSUtil.h"
#include "..\..\subtitles\SubtitleInputPin.h"

//
// CTextPassThruInputPin
//

class CTextPassThruInputPin : public CSubtitleInputPin
{
	CTextPassThruFilter* m_pTPTFilter;
	CComPtr<ISubStream> m_pSubStreamOld;

protected:
	void AddSubStream(ISubStream* pSubStream)
	{
		if(m_pSubStreamOld)
		{
			if(pSubStream) m_pTPTFilter->m_pMainFrame->ReplaceSubtitle(m_pSubStreamOld, pSubStream);
			m_pSubStreamOld = NULL;
		}
	}

	void RemoveSubStream(ISubStream* pSubStream)
	{
		m_pSubStreamOld = pSubStream;
	}

	void InvalidateSubtitle(REFERENCE_TIME rtStart, ISubStream* pSubStream)
	{
		m_pTPTFilter->m_pMainFrame->InvalidateSubtitle((DWORD_PTR)pSubStream, rtStart);
	}

public:
    CTextPassThruInputPin(CTextPassThruFilter* pTPTFilter, CCritSec* pLock, CCritSec* pSubLock, HRESULT* phr);
	STDMETHODIMP NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);
	STDMETHODIMP Receive(IMediaSample* pSample);
    STDMETHODIMP EndOfStream();
    STDMETHODIMP BeginFlush();
    STDMETHODIMP EndFlush();
};

//
// CTextPassThruOutputPin
//

class CTextPassThruOutputPin : public CBaseOutputPin
{
	CTextPassThruFilter* m_pTPTFilter;

public:
    CTextPassThruOutputPin(CTextPassThruFilter* pTPTFilter, CCritSec* pLock, HRESULT* phr);

    HRESULT CheckMediaType(const CMediaType* mtOut);
	HRESULT DecideBufferSize(IMemAllocator* pAllocator, ALLOCATOR_PROPERTIES* pProperties);
	HRESULT GetMediaType(int iPosition, CMediaType* pmt);
	STDMETHODIMP Notify(IBaseFilter* pSender, Quality q) {return S_OK;}
};

///////////

CTextPassThruInputPin::CTextPassThruInputPin(CTextPassThruFilter* pTPTFilter, CCritSec* pLock, CCritSec* pSubLock, HRESULT* phr)
	: CSubtitleInputPin(pTPTFilter, pLock, pSubLock, phr)
	, m_pTPTFilter(pTPTFilter)
{
}

STDMETHODIMP CTextPassThruInputPin::NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate)
{
	HRESULT hr = __super::NewSegment(tStart, tStop, dRate);
	if(FAILED(hr)) return hr;
	return m_pTPTFilter->m_pOutput->DeliverNewSegment(tStart, tStop, dRate);
}

STDMETHODIMP CTextPassThruInputPin::Receive(IMediaSample* pSample)
{
	HRESULT hr = __super::Receive(pSample);
	if(FAILED(hr)) return hr;
	return m_pTPTFilter->m_pOutput->Deliver(pSample);
}

STDMETHODIMP CTextPassThruInputPin::EndOfStream()
{
	HRESULT hr = __super::EndOfStream();
	if(FAILED(hr)) return hr;
	return m_pTPTFilter->m_pOutput->DeliverEndOfStream();
}

STDMETHODIMP CTextPassThruInputPin::BeginFlush()
{
	HRESULT hr = __super::BeginFlush();
	if(FAILED(hr)) return hr;
	return m_pTPTFilter->m_pOutput->DeliverBeginFlush();
}

STDMETHODIMP CTextPassThruInputPin::EndFlush()
{
	HRESULT hr = __super::EndFlush();
	if(FAILED(hr)) return hr;
	return m_pTPTFilter->m_pOutput->DeliverEndFlush();
}

//

CTextPassThruOutputPin::CTextPassThruOutputPin(CTextPassThruFilter* pTPTFilter, CCritSec* pLock, HRESULT* phr)
	: CBaseOutputPin(NAME(""), pTPTFilter, pLock, phr, L"Out")
	, m_pTPTFilter(pTPTFilter)
{
}

HRESULT CTextPassThruOutputPin::CheckMediaType(const CMediaType* mtOut)
{
	CMediaType mt;
	return S_OK == m_pTPTFilter->m_pInput->ConnectionMediaType(&mt) && mt == *mtOut
		? S_OK 
		: E_FAIL;
}

HRESULT CTextPassThruOutputPin::DecideBufferSize(IMemAllocator* pAllocator, ALLOCATOR_PROPERTIES* pProperties)
{
	if(m_pTPTFilter->m_pInput->IsConnected() == FALSE)
		return E_UNEXPECTED;

	CComPtr<IMemAllocator> pAllocatorIn;
	m_pTPTFilter->m_pInput->GetAllocator(&pAllocatorIn);
	if(!pAllocatorIn) return E_UNEXPECTED;

	pAllocatorIn->GetProperties(pProperties);

	HRESULT hr;
	ALLOCATOR_PROPERTIES Actual;
	if(FAILED(hr = pAllocator->SetProperties(pProperties, &Actual))) 
		return hr;

	return(pProperties->cBuffers > Actual.cBuffers || pProperties->cbBuffer > Actual.cbBuffer
		? E_FAIL
		: NOERROR);
}

HRESULT CTextPassThruOutputPin::GetMediaType(int iPosition, CMediaType* pmt)
{
	if(m_pTPTFilter->m_pInput->IsConnected() == FALSE)
		return E_UNEXPECTED;

	if(iPosition < 0) return E_INVALIDARG;
	if(iPosition > 0) return VFW_S_NO_MORE_ITEMS;

	m_pTPTFilter->m_pInput->ConnectionMediaType(pmt);

	return S_OK;
}

//
// CTextPassThruFilter
//

CTextPassThruFilter::CTextPassThruFilter(CMainFrame* pMainFrame) 
	: CBaseFilter(NAME("CTextPassThruFilter"), NULL, this, __uuidof(this))
	, m_pMainFrame(pMainFrame)
{
	HRESULT hr;
	m_pInput = new CTextPassThruInputPin(this, this, &m_pMainFrame->m_csSubLock, &hr);
	m_pOutput = new CTextPassThruOutputPin(this, this, &hr);
}

CTextPassThruFilter::~CTextPassThruFilter()
{
	delete m_pInput; m_pInput = NULL;
	delete m_pOutput; m_pOutput = NULL;
}

STDMETHODIMP CTextPassThruFilter::NonDelegatingQueryInterface(REFIID riid, void** ppv)
{
	if(m_pInput && riid == __uuidof(ISubStream))
	{
		if(CComPtr<ISubStream> pSubStream = m_pInput->GetSubStream())
		{
			*ppv = pSubStream.Detach();
			return S_OK;
		}
	}

	return __super::NonDelegatingQueryInterface(riid, ppv);
}

int CTextPassThruFilter::GetPinCount()
{
	return 2;
}

CBasePin* CTextPassThruFilter::GetPin(int n)
{
	if(n == 0) return m_pInput;
	else if(n == 1) return m_pOutput;
	return NULL;
}

/*
void CTextPassThruFilter::SetName()
{
	CRenderedTextSubtitle* pRTS = (CRenderedTextSubtitle*)(ISubStream*)m_pRTS;

	CAutoLock cAutoLock(&m_pMainFrame->m_csSubLock);

	if(CComQIPtr<IPropertyBag> pPB = m_pTPTInput->GetConnected())
	{
		CComVariant var;
		if(SUCCEEDED(pPB->Read(L"LANGUAGE", &var, NULL)))
		{
			pRTS->m_name = CString(var.bstrVal) + _T(" (embeded)");
		}
	}

	if(pRTS->m_name.IsEmpty())
	{
		CPinInfo pi;
		m_pTPTInput->GetConnected()->QueryPinInfo(&pi);
		pRTS->m_name = CString(CStringW(pi.achName)) + _T(" (embeded)");

	}
}
*/
