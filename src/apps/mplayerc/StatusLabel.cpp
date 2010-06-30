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

// StatusLabel.cpp : implementation file
//

#include "stdafx.h"
#include "mplayerc.h"
#include "StatusLabel.h"
#include "SUIButton.h"

// CStatusLabel

IMPLEMENT_DYNAMIC(CStatusLabel, CStatic)
CStatusLabel::CStatusLabel(bool fRightAlign, bool fAddEllipses) 
	: m_fRightAlign(fRightAlign)
	, m_fAddEllipses(fAddEllipses)
{
	GetSystemFontWithScale(&m_font);
}

CStatusLabel::~CStatusLabel()
{
}

BEGIN_MESSAGE_MAP(CStatusLabel, CStatic)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CStatusLabel message handlers

void CStatusLabel::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);
		CString str;
		GetWindowText(str);
		CRect r;
		GetClientRect(&r);
		CFont* old = dc.SelectObject(&m_font);
		dc.SetTextColor(0xffffff);
		dc.SetBkColor(0);
		CSize size = dc.GetTextExtent(str);
		CPoint p = CPoint(m_fRightAlign ? r.Width() - size.cx : 0, (r.Height()-size.cy)/2);

		if(m_fAddEllipses)
		while(size.cx > r.Width()-3 && str.GetLength() > 3)
		{
			str = str.Left(str.GetLength()-4) + _T("...");
			size = dc.GetTextExtent(str);
		}

		dc.TextOut(p.x, p.y, str);
		dc.ExcludeClipRect(CRect(p, size));
		dc.SelectObject(&old);
		dc.FillSolidRect(&r, NEWUI_COLOR_TOOLBAR_UPPERBG);
	dc.Detach();
}

BOOL CStatusLabel::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	pDC->FillSolidRect(&r, NEWUI_COLOR_TOOLBAR_UPPERBG);
	return TRUE;
}

