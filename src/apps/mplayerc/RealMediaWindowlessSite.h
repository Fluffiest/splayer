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
#include "..\..\..\include\RealMedia\rmaevent.h"

namespace DSObjects
{

struct REGION
{
	REGION() : rects(0), pOSRegion(0) {}
	long size;
	long numRects;
	PNxRect* rects;
	PNxRect extents;
	void* pOSRegion;
};

void ExtractRects(REGION* pRegion);
REGION* RMACreateRectRegion(int left, int top, int right, int bottom);
void RMASubtractRegion(REGION* regM, REGION* regS, REGION* regD);
void RMAUnionRegion(REGION* reg1, REGION* reg2, REGION* regD);
void RMAIntersectRegion(REGION* reg1, REGION* reg2, REGION* regD);
BOOL RMAEqualRegion(REGION* reg1, REGION* reg2);
void RMADestroyRegion(REGION* reg);
REGION* RMACreateRegion();

class CRealMediaWindowlessSite;

//
// CRealMediaVideoSurface
//

class CRealMediaWindowlessSite;

class CRealMediaVideoSurface
	: public CUnknown
	, public IRMAVideoSurface
{
    void IntersectRect(PNxRect* pRect, PNxRect* pBox, PNxRect* pRetVal);

protected:
    CComPtr<IUnknown> m_pContext;
    CRealMediaWindowlessSite* m_pSiteWindowless;
    RMABitmapInfoHeader m_bitmapInfo;
    RMABitmapInfoHeader m_lastBitmapInfo;

public:
    CRealMediaVideoSurface(IUnknown* pContext, CRealMediaWindowlessSite* pSiteWindowless);
    virtual ~CRealMediaVideoSurface();

    DECLARE_IUNKNOWN;
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

	// IRMAVideoSurface

	STDMETHODIMP Blt(UCHAR*	/*IN*/ pImageData, RMABitmapInfoHeader* /*IN*/ pBitmapInfo, REF(PNxRect) /*IN*/ inDestRect, REF(PNxRect) /*IN*/ inSrcRect);
	STDMETHODIMP BeginOptimizedBlt(RMABitmapInfoHeader* /*IN*/ pBitmapInfo);
	STDMETHODIMP OptimizedBlt(UCHAR* /*IN*/ pImageBits, REF(PNxRect) /*IN*/ rDestRect, REF(PNxRect) /*IN*/ rSrcRect);
	STDMETHODIMP EndOptimizedBlt();
	STDMETHODIMP GetOptimizedFormat(REF(RMA_COMPRESSION_TYPE) /*OUT*/ ulType);
    STDMETHODIMP GetPreferredFormat(REF(RMA_COMPRESSION_TYPE) /*OUT*/ ulType);
};

//
// CRealMediaWindowlessSite
//

class CRealMediaWindowlessSite
	: public CUnknown
	, public IRMASite
	, public IRMASite2
	, public IRMASiteWindowless
	, public IRMAVideoSurface
{
	CComQIPtr<IRMACommonClassFactory, &IID_IRMACommonClassFactory> m_pCCF;
    CComPtr<IUnknown> m_pContext;
    CComPtr<IRMAValues> m_pValues;

    CComPtr<IRMASiteUser> m_pUser;

	CRealMediaWindowlessSite* m_pParentSite;
	CInterfaceList<IRMASite, &IID_IRMASite> m_pChildren;

    CComPtr<IRMASiteWatcher> m_pWatcher;
	CInterfaceList<IRMAPassiveSiteWatcher, &IID_IRMAPassiveSiteWatcher> m_pPassiveWatchers;

    PNxSize m_size;
    PNxPoint m_position;
	bool m_fDamaged, m_fInRedraw, m_fIsVisible;
	INT32 m_lZOrder;

	//

    REGION* m_pRegion;
    REGION* m_pRegionWithoutChildren;

	void RecomputeRegion();
	void InternalRecomputeRegion();
	void ComputeRegion();
	void SubtractSite(REGION* pRegion);

	void UpdateZOrder(CRealMediaWindowlessSite* pUpdatedChildSite, INT32 lOldZOrder, INT32 lNewZOrder);
    void SetInternalZOrder(INT32 lZOrder);

public:
	CRealMediaWindowlessSite(HRESULT& hr, IUnknown* pContext, CRealMediaWindowlessSite* pParentSite = NULL, IUnknown* pUnkOuter = NULL);
	virtual ~CRealMediaWindowlessSite();

    DECLARE_IUNKNOWN;
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

	void GetTopLeft(PNxPoint* pPoint);
    REGION* GetRegion();

    // IRMASiteWindowless

    STDMETHODIMP EventOccurred(PNxEvent* /*IN*/ pEvent);
    STDMETHODIMP_(PNxWindow*) GetParentWindow();

    // IRMASite

    STDMETHODIMP AttachUser(IRMASiteUser* /*IN*/ pUser);
    STDMETHODIMP DetachUser();
    STDMETHODIMP GetUser(REF(IRMASiteUser*) /*OUT*/ pUser);

    STDMETHODIMP CreateChild(REF(IRMASite*) /*OUT*/ pChildSite);
	STDMETHODIMP DestroyChild(IRMASite* /*IN*/ pChildSite);

    STDMETHODIMP AttachWatcher(IRMASiteWatcher* /*IN*/ pWatcher);
    STDMETHODIMP DetachWatcher();

    STDMETHODIMP SetPosition(PNxPoint position);
    STDMETHODIMP GetPosition(REF(PNxPoint) position);
	STDMETHODIMP SetSize(PNxSize size);
    STDMETHODIMP GetSize(REF(PNxSize) size);

    STDMETHODIMP DamageRect(PNxRect rect);
    STDMETHODIMP DamageRegion(PNxRegion region);
    STDMETHODIMP ForceRedraw();

	// IRMASite2

	STDMETHODIMP UpdateSiteWindow(PNxWindow* /*IN*/ pWindow);
    STDMETHODIMP ShowSite(BOOL bShow);
	STDMETHODIMP_(BOOL) IsSiteVisible();
    STDMETHODIMP SetZOrder(INT32 lZOrder);
    STDMETHODIMP GetZOrder(REF(INT32) lZOrder);
    STDMETHODIMP MoveSiteToTop();
	STDMETHODIMP GetVideoSurface(REF(IRMAVideoSurface*) pSurface);
    STDMETHODIMP_(UINT32) GetNumberOfChildSites();

    STDMETHODIMP AddPassiveSiteWatcher(IRMAPassiveSiteWatcher* pWatcher);
    STDMETHODIMP RemovePassiveSiteWatcher(IRMAPassiveSiteWatcher* pWatcher);

	STDMETHODIMP SetCursor(PNxCursor cursor, REF(PNxCursor) oldCursor);

private:
    void IntersectRect(PNxRect* pRect, PNxRect* pBox, PNxRect* pRetVal);

protected:
    RMABitmapInfoHeader m_bitmapInfo;
    RMABitmapInfoHeader m_lastBitmapInfo;

	CComPtr<IRMAVideoSurface> m_pBltService;

public:
    bool GetBltService(IRMAVideoSurface** ppBltService);
    void SetBltService(IRMAVideoSurface* ppBltService);

	// IRMAVideoSurface

	STDMETHODIMP Blt(UCHAR*	/*IN*/ pImageData, RMABitmapInfoHeader* /*IN*/ pBitmapInfo, REF(PNxRect) /*IN*/ inDestRect, REF(PNxRect) /*IN*/ inSrcRect);
	STDMETHODIMP BeginOptimizedBlt(RMABitmapInfoHeader* /*IN*/ pBitmapInfo);
	STDMETHODIMP OptimizedBlt(UCHAR* /*IN*/ pImageBits, REF(PNxRect) /*IN*/ rDestRect, REF(PNxRect) /*IN*/ rSrcRect);
	STDMETHODIMP EndOptimizedBlt();
	STDMETHODIMP GetOptimizedFormat(REF(RMA_COMPRESSION_TYPE) /*OUT*/ ulType);
    STDMETHODIMP GetPreferredFormat(REF(RMA_COMPRESSION_TYPE) /*OUT*/ ulType);
};

}
using namespace DSObjects;
