/* 
 * $Id: RenderedHdmvSubtitle.h 1647 2010-02-09 20:12:31Z xhmikosr $
 *
 * (C) 2006-2010 see AUTHORS
 *
 * This file is part of mplayerc.
 *
 * Mplayerc is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mplayerc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#pragma once

#include "Rasterizer.h"
#include "../SubPic/ISubPic.h"
#include "HdmvSub.h"
#include "BaseSub.h"


[uuid("FCA68599-C83E-4ea5-94A3-C2E1B0E326B9")]
class CRenderedHdmvSubtitle : public ISubPicProviderImpl, public ISubStream
{
public:
	CRenderedHdmvSubtitle(CCritSec* pLock, SUBTITLE_TYPE nType);
	~CRenderedHdmvSubtitle(void);

	DECLARE_IUNKNOWN
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

	// ISubPicProvider
	STDMETHODIMP_(POSITION) GetStartPosition(REFERENCE_TIME rt, double fps);
	STDMETHODIMP_(POSITION) GetNext(POSITION pos);
	STDMETHODIMP_(REFERENCE_TIME) GetStart(POSITION pos, double fps);
	STDMETHODIMP_(REFERENCE_TIME) GetStop(POSITION pos, double fps);
	STDMETHODIMP_(bool) IsAnimated(POSITION pos);
	STDMETHODIMP Render(SubPicDesc& spd, REFERENCE_TIME rt, double fps, RECT& bbox);
	STDMETHODIMP GetTextureSize (POSITION pos, SIZE& MaxTextureSize, SIZE& VirtualSize, POINT& VirtualTopLeft);

	// IPersist
	STDMETHODIMP GetClassID(CLSID* pClassID);

	// ISubStream
	STDMETHODIMP_(int) GetStreamCount();
	STDMETHODIMP GetStreamInfo(int i, WCHAR** ppName, LCID* pLCID);
	STDMETHODIMP_(int) GetStream();
	STDMETHODIMP SetStream(int iStream);
	STDMETHODIMP Reload();

	HRESULT ParseSample (IMediaSample* pSample);
	HRESULT	NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);

private :
	CString			m_name;
	LCID			m_lcid;
	REFERENCE_TIME	m_rtStart;

	CBaseSub*		m_pSub;
	CCritSec		m_csCritSec;
};
