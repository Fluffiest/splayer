﻿/* 
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

// PPageAccelTbl.cpp : implementation file
//

#include "stdafx.h"
#include "mplayerc.h"
#include "PPageAccelTbl.h"

// CPPageAccelTbl dialog

IMPLEMENT_DYNAMIC(CPPageAccelTbl, CPPageBase)
CPPageAccelTbl::CPPageAccelTbl()
	: CPPageBase(CPPageAccelTbl::IDD, CPPageAccelTbl::IDD)
	, m_list(0)
	, m_counter(0)
	, m_fWinLirc(FALSE)
	, m_WinLircLink(_T("http://winlirc.sourceforge.net/"))
	, m_fUIce(FALSE)
	, m_UIceLink(_T("http://www.mediatexx.com/"))
{
}

CPPageAccelTbl::~CPPageAccelTbl()
{
}

BOOL CPPageAccelTbl::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN 
	&& (pMsg->hwnd == m_WinLircEdit.m_hWnd || pMsg->hwnd == m_UIceEdit.m_hWnd))
	{
		OnApply();
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}


void CPPageAccelTbl::SetupList()
{
// 	for(int row = 0; row < m_list.GetItemCount(); row++)
// 	{
// 		wmcmd& wc = m_wmcmds.GetAt((POSITION)m_list.GetItemData(row));
// 
// 		CString mod = MakeAccelModLabel(wc.fVirt);
// 		m_list.SetItemText(row, COL_MOD, mod);
// 
// 		CString key = MakeAccelVkeyLabel(wc.key, wc.fVirt&FVIRTKEY);
// 		m_list.SetItemText(row, COL_KEY, key);
// 
// 
// 		m_list.SetItemText(row, COL_MOUSE, MakeMouseButtonLabel(wc.mouse));
// 		CString type = (wc.fVirt&FVIRTKEY)?ResStr(IDS_ACCEL_HOTKEY_SET_BUTTON_KEY):ResStr(IDS_ACCEL_HOTKEY_SET_BUTTON_LETTERKEY);
// 		m_list.SetItemText(row, COL_TYPE, type);
// 
// 		CString id;
// 		id.Format(_T("%d"), wc.cmd);
// 		m_list.SetItemText(row, COL_ID, id);
// 
// 		m_list.SetItemText(row, COL_APPCMD, MakeAppCommandLabel(wc.appcmd));
// 
// 		m_list.SetItemText(row, COL_RMCMD, CString(wc.rmcmd));
// 
// 
// 		CString repcnt;
// 		repcnt.Format(_T("%d"), wc.rmrepcnt);
// 		m_list.SetItemText(row, COL_RMREPCNT, repcnt);
// 	}
}

CString CPPageAccelTbl::MakeAccelModLabel(BYTE fVirt)
{
	CString str;
	if(fVirt&FCONTROL) {if(!str.IsEmpty()) str += _T(" + "); str += _T("Ctrl");}
	if(fVirt&FALT) {if(!str.IsEmpty()) str += _T(" + "); str += _T("Alt");}
	if(fVirt&FSHIFT) {if(!str.IsEmpty()) str += _T(" + "); str += _T("Shift");}
	if(str.IsEmpty()) str = _T("None");
	return(str);
}

CString CPPageAccelTbl::MakeAccelVkeyLabel(WORD key, bool fVirtKey)
{
	CString str;

	if(fVirtKey)
	switch(key)
	{
	case VK_LBUTTON: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_MOUSELEFT); break;
	case VK_RBUTTON: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_MOUSERIGHT); break;
	case VK_CANCEL: str = _T("Ctrl-Break"); break;
	case VK_MBUTTON: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_MOUSEMIDDLE); break;
	case VK_XBUTTON1: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_MOUSE4); break;
	case VK_XBUTTON2: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_MOUSE5); break;
	case VK_BACK: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_BACKSPACE); break;
	case VK_TAB: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_TAB); break;
	case VK_CLEAR: str = _T("VK_CLEAR"); break;
	case VK_RETURN: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_RETURN); break;
	case VK_SHIFT: str = _T("Shift"); break;
	case VK_CONTROL: str = _T("Ctrl"); break;
	case VK_MENU: str = _T("VK_MENU"); break;
	case VK_PAUSE: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_PAUSE); break;
	case VK_CAPITAL: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_CAPITAL); break;
//	case VK_KANA: str = _T("VK_KANA"); break;
//	case VK_HANGEUL: str = _T("VK_HANGEUL"); break;
	case VK_HANGUL: str = _T("VK_HANGUL"); break;
	case VK_JUNJA: str = _T("VK_JUNJA"); break;
	case VK_FINAL: str = _T("VK_FINAL"); break;
//	case VK_HANJA: str = _T("VK_HANJA"); break;
	case VK_KANJI: str = _T("VK_KANJI"); break;
	case VK_ESCAPE: str = _T("Esc"); break;
	case VK_CONVERT: str = _T("VK_CONVERT"); break;
	case VK_NONCONVERT: str = _T("VK_NONCONVERT"); break;
	case VK_ACCEPT: str = _T("VK_ACCEPT"); break;
	case VK_MODECHANGE: str = _T("VK_MODECHANGE"); break;
	case VK_SPACE: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_SPACE); break;
	case VK_PRIOR: str = _T("VK_PRIOR"); break;
	case VK_NEXT: str = _T("VK_NEXT"); break;
	case VK_END: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_END); break;
	case VK_HOME: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_HOME); break;
	case VK_LEFT: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_LEFT_ARROW); break;
	case VK_UP: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_UP_ARROW); break;
	case VK_RIGHT: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_RIGHT_ARROW); break;
	case VK_DOWN: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_DOWN_ARROW); break;
	case VK_SELECT: str = _T("VK_SELECT"); break;
	case VK_PRINT: str = _T("VK_PRINT"); break;
	case VK_EXECUTE: str = _T("VK_EXECUTE"); break;
	case VK_SNAPSHOT: str = _T("VK_SNAPSHOT"); break;
	case VK_INSERT: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_INS_ARROW); break;
	case VK_DELETE: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_DEL_ARROW); break;
	case VK_HELP: str = _T("VK_HELP"); break;
	case VK_LWIN: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_LEFT_WIN); break;
	case VK_RWIN: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_RIGHT_WIN); break;
	case VK_APPS: str = _T("VK_APPS"); break;
	case VK_SLEEP: str = _T("VK_SLEEP"); break;
	case VK_NUMPAD0: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_NUMBERPAD_0); break;
	case VK_NUMPAD1: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_NUMBERPAD_1); break;
	case VK_NUMPAD2: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_NUMBERPAD_2); break;
	case VK_NUMPAD3: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_NUMBERPAD_3); break;
	case VK_NUMPAD4: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_NUMBERPAD_4); break;
	case VK_NUMPAD5: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_NUMBERPAD_5); break;
	case VK_NUMPAD6: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_NUMBERPAD_6); break;
	case VK_NUMPAD7: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_NUMBERPAD_7); break;
	case VK_NUMPAD8: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_NUMBERPAD_8); break;
	case VK_NUMPAD9: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_NUMBERPAD_9); break;
	case VK_MULTIPLY: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_NUMBERPAD_MUTIPLY); break;
	case VK_ADD: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_NUMBERPAD_PLUS); break;
	case VK_SEPARATOR: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_NUMBERPAD_RETURN); break;
	case VK_SUBTRACT: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_NUMBERPAD_MINUS); break;
	case VK_DECIMAL: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_NUMBERPAD_DOT); break;
	case VK_DIVIDE: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_NUMBERPAD_SLASH); break;
	case VK_F1: str = _T("F1"); break;
	case VK_F2: str = _T("F2"); break;
	case VK_F3: str = _T("F3"); break;
	case VK_F4: str = _T("F4"); break;
	case VK_F5: str = _T("F5"); break;
	case VK_F6: str = _T("F6"); break;
	case VK_F7: str = _T("F7"); break;
	case VK_F8: str = _T("F8"); break;
	case VK_F9: str = _T("F9"); break;
	case VK_F10: str = _T("F10"); break;
	case VK_F11: str = _T("F11"); break;
	case VK_F12: str = _T("F12"); break;
	case VK_F13: str = _T("F13"); break;
	case VK_F14: str = _T("F14"); break;
	case VK_F15: str = _T("F15"); break;
	case VK_F16: str = _T("F16"); break;
	case VK_F17: str = _T("F17"); break;
	case VK_F18: str = _T("F18"); break;
	case VK_F19: str = _T("F19"); break;
	case VK_F20: str = _T("F20"); break;
	case VK_F21: str = _T("F21"); break;
	case VK_F22: str = _T("F22"); break;
	case VK_F23: str = _T("F23"); break;
	case VK_F24: str = _T("F24"); break;
	case VK_NUMLOCK: str = _T("NUMLOCK"); break;
	case VK_SCROLL: str = _T("SCROLL"); break;
//	case VK_OEM_NEC_EQUAL: str = _T("VK_OEM_NEC_EQUAL"); break;
	case VK_OEM_FJ_JISHO: str = _T("VK_OEM_FJ_JISHO"); break;
	case VK_OEM_FJ_MASSHOU: str = _T("VK_OEM_FJ_MASSHOU"); break;
	case VK_OEM_FJ_TOUROKU: str = _T("VK_OEM_FJ_TOUROKU"); break;
	case VK_OEM_FJ_LOYA: str = _T("VK_OEM_FJ_LOYA"); break;
	case VK_OEM_FJ_ROYA: str = _T("VK_OEM_FJ_ROYA"); break;
	case VK_LSHIFT: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_SHIFT_LEFT); break;
	case VK_RSHIFT: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_SHIFT_RIGHT); break;
	case VK_LCONTROL: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_CTRL_LEFT); break;
	case VK_RCONTROL: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_CTRL_RIGHT); break;
	case VK_LMENU: str = _T("VK_LMENU"); break;
	case VK_RMENU: str = _T("VK_RMENU"); break;
	case VK_BROWSER_BACK: str = _T("VK_BROWSER_BACK"); break;
	case VK_BROWSER_FORWARD: str = _T("VK_BROWSER_FORWARD"); break;
	case VK_BROWSER_REFRESH: str = _T("VK_BROWSER_REFRESH"); break;
	case VK_BROWSER_STOP: str = _T("VK_BROWSER_STOP"); break;
	case VK_BROWSER_SEARCH: str = _T("VK_BROWSER_SEARCH"); break;
	case VK_BROWSER_FAVORITES: str = _T("VK_BROWSER_FAVORITES"); break;
	case VK_BROWSER_HOME: str = _T("VK_BROWSER_HOME"); break;
	case VK_VOLUME_MUTE: str = _T("VK_VOLUME_MUTE"); break;
	case VK_VOLUME_DOWN: str = _T("VK_VOLUME_DOWN"); break;
	case VK_VOLUME_UP: str = _T("VK_VOLUME_UP"); break;
	case VK_MEDIA_NEXT_TRACK: str = _T("VK_MEDIA_NEXT_TRACK"); break;
	case VK_MEDIA_PREV_TRACK: str = _T("VK_MEDIA_PREV_TRACK"); break;
	case VK_MEDIA_STOP: str = _T("VK_MEDIA_STOP"); break;
	case VK_MEDIA_PLAY_PAUSE: str = _T("VK_MEDIA_PLAY_PAUSE"); break;
	case VK_LAUNCH_MAIL: str = _T("VK_LAUNCH_MAIL"); break;
	case VK_LAUNCH_MEDIA_SELECT: str = _T("VK_LAUNCH_MEDIA_SELECT"); break;
	case VK_LAUNCH_APP1: str = _T("VK_LAUNCH_APP1"); break;
	case VK_LAUNCH_APP2: str = _T("VK_LAUNCH_APP2"); break;
	case VK_OEM_1: str = _T(";"); break;
	case VK_OEM_PLUS: str = _T("+"); break;
	case VK_OEM_COMMA: str = _T(","); break;
	case VK_OEM_MINUS: str = _T("-"); break;
	case VK_OEM_PERIOD: str = _T("."); break;
	case VK_OEM_2: str = _T("/"); break;
	case VK_OEM_3: str = _T("`"); break;
	case VK_OEM_4: str = _T("["); break;
	case VK_OEM_5: str = _T("\\"); break;
	case VK_OEM_6: str = _T("]"); break;
	case VK_OEM_7: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_QUOTE); break;
	case VK_OEM_8: str = _T("VK_OEM_8"); break;
	case VK_OEM_AX: str = _T("VK_OEM_AX"); break;
	case VK_OEM_102: str = _T("VK_OEM_102"); break;
	case VK_ICO_HELP: str = _T("VK_ICO_HELP"); break;
	case VK_ICO_00: str = _T("VK_ICO_00"); break;
	case VK_PROCESSKEY: str = _T("VK_PROCESSKEY"); break;
	case VK_ICO_CLEAR: str = _T("VK_ICO_CLEAR"); break;
	case VK_PACKET: str = _T("VK_PACKET"); break;
	case VK_OEM_RESET: str = _T("VK_OEM_RESET"); break;
	case VK_OEM_JUMP: str = _T("VK_OEM_JUMP"); break;
	case VK_OEM_PA1: str = _T("VK_OEM_PA1"); break;
	case VK_OEM_PA2: str = _T("VK_OEM_PA2"); break;
	case VK_OEM_PA3: str = _T("VK_OEM_PA3"); break;
	case VK_OEM_WSCTRL: str = _T("VK_OEM_WSCTRL"); break;
	case VK_OEM_CUSEL: str = _T("VK_OEM_CUSEL"); break;
	case VK_OEM_ATTN: str = _T("VK_OEM_ATTN"); break;
	case VK_OEM_FINISH: str = _T("VK_OEM_FINISH"); break;
	case VK_OEM_COPY: str = _T("VK_OEM_COPY"); break;
	case VK_OEM_AUTO: str = _T("VK_OEM_AUTO"); break;
	case VK_OEM_ENLW: str = _T("VK_OEM_ENLW"); break;
	case VK_OEM_BACKTAB: str = _T("VK_OEM_BACKTAB"); break;
	case VK_ATTN: str = _T("VK_ATTN"); break;
	case VK_CRSEL: str = _T("VK_CRSEL"); break;
	case VK_EXSEL: str = _T("VK_EXSEL"); break;
	case VK_EREOF: str = _T("VK_EREOF"); break;
	case VK_PLAY: str = _T("VK_PLAY"); break;
	case VK_ZOOM: str = _T("VK_ZOOM"); break;
	case VK_NONAME: str = _T("VK_NONAME"); break;
	case VK_PA1: str = _T("VK_PA1"); break;
	case VK_OEM_CLEAR: str = _T("VK_OEM_CLEAR"); break;
	default: 
		if('0' <= key && key <= '9' || 'A' <= key && key <= 'Z')
			str.Format(_T("%c"), (TCHAR)key);
		break;
	}

	if(str.IsEmpty() || !fVirtKey)
	{
		str.Format(_T("%c, 0x%02x"), (TCHAR)key, key); 
		return(str);
	}

	return(str);
}

CString CPPageAccelTbl::MakeAccelShortcutLabel(UINT id)
{
// 	CList<wmcmd>& wmcmds = AfxGetAppSettings().wmcmds;
// 	POSITION pos = wmcmds.GetHeadPosition();
// 	while(pos)
// 	{
// 		ACCEL& a = wmcmds.GetNext(pos);
// 		if(a.cmd == id)
// 			return(MakeAccelShortcutLabel(a));
// 	}
// 
	return(_T(""));
}

CString CPPageAccelTbl::MakeAccelShortcutLabel(ACCEL& a)
{
	CString str;

	if(a.fVirt&1)
	switch(a.key)
	{
	case VK_LBUTTON: str = _T("LBtn"); break;
	case VK_RBUTTON: str = _T("RBtn"); break;
	case VK_CANCEL: str = _T("Cancel"); break;
	case VK_MBUTTON: str = _T("MBtn"); break;
	case VK_XBUTTON1: str = _T("X1Btn"); break;
	case VK_XBUTTON2: str = _T("X2Btn"); break;
	case VK_BACK: str = _T("Back"); break;
	case VK_TAB: str = _T("Tab"); break;
	case VK_CLEAR: str = _T("Clear"); break;
	case VK_RETURN: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_RETURN); break;
	case VK_SHIFT: str = _T("Shift"); break;
	case VK_CONTROL: str = _T("Ctrl"); break;
	case VK_MENU: str = _T("Alt"); break;
	case VK_PAUSE: str = _T("Pause"); break;
	case VK_CAPITAL: str = _T("Capital"); break;
//	case VK_KANA: str = _T("Kana"); break;
//	case VK_HANGEUL: str = _T("Hangeul"); break;
	case VK_HANGUL: str = _T("Hangul"); break;
	case VK_JUNJA: str = _T("Junja"); break;
	case VK_FINAL: str = _T("Final"); break;
//	case VK_HANJA: str = _T("Hanja"); break;
	case VK_KANJI: str = _T("Kanji"); break;
	case VK_ESCAPE: str = _T("Esc"); break;
	case VK_CONVERT: str = _T("Convert"); break;
	case VK_NONCONVERT: str = _T("Non Convert"); break;
	case VK_ACCEPT: str = _T("Accept"); break;
	case VK_MODECHANGE: str = _T("Mode Change"); break;
	case VK_SPACE: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_SPACE); break;
	case VK_PRIOR: str = _T("PgUp"); break;
	case VK_NEXT: str = _T("PgDn"); break;
	case VK_END: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_END); break;
	case VK_HOME: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_HOME); break;
	case VK_LEFT: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_LEFT_ARROW); break;
	case VK_UP: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_UP_ARROW); break;
	case VK_RIGHT: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_RIGHT_ARROW); break;
	case VK_DOWN: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_DOWN_ARROW); break;
	case VK_SELECT: str = _T("Select"); break;
	case VK_PRINT: str = _T("Print"); break;
	case VK_EXECUTE: str = _T("Execute"); break;
	case VK_SNAPSHOT: str = _T("Snapshot"); break;
	case VK_INSERT: str = _T("Insert"); break;
	case VK_DELETE: str = _T("Delete"); break;
	case VK_HELP: str = _T("Help"); break;
	case VK_LWIN: str = _T("LWin"); break;
	case VK_RWIN: str = _T("RWin"); break;
	case VK_APPS: str = _T("Apps"); break;
	case VK_SLEEP: str = _T("Sleep"); break;
	case VK_NUMPAD0: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_NUMBERPAD_0); break;
	case VK_NUMPAD1: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_NUMBERPAD_1); break;
	case VK_NUMPAD2: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_NUMBERPAD_2); break;
	case VK_NUMPAD3: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_NUMBERPAD_3); break;
	case VK_NUMPAD4: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_NUMBERPAD_4); break;
	case VK_NUMPAD5: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_NUMBERPAD_5); break;
	case VK_NUMPAD6: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_NUMBERPAD_6); break;
	case VK_NUMPAD7: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_NUMBERPAD_7); break;
	case VK_NUMPAD8: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_NUMBERPAD_8); break;
	case VK_NUMPAD9: str = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_NUMBERPAD_9); break;
	case VK_MULTIPLY: str = _T("Multiply"); break;
	case VK_ADD: str = _T("Add"); break;
	case VK_SEPARATOR: str = _T("Separator"); break;
	case VK_SUBTRACT: str = _T("Subtract"); break;
	case VK_DECIMAL: str = _T("Decimal"); break;
	case VK_DIVIDE: str = _T("Divide"); break;
	case VK_F1: str = _T("F1"); break;
	case VK_F2: str = _T("F2"); break;
	case VK_F3: str = _T("F3"); break;
	case VK_F4: str = _T("F4"); break;
	case VK_F5: str = _T("F5"); break;
	case VK_F6: str = _T("F6"); break;
	case VK_F7: str = _T("F7"); break;
	case VK_F8: str = _T("F8"); break;
	case VK_F9: str = _T("F9"); break;
	case VK_F10: str = _T("F10"); break;
	case VK_F11: str = _T("F11"); break;
	case VK_F12: str = _T("F12"); break;
	case VK_F13: str = _T("F13"); break;
	case VK_F14: str = _T("F14"); break;
	case VK_F15: str = _T("F15"); break;
	case VK_F16: str = _T("F16"); break;
	case VK_F17: str = _T("F17"); break;
	case VK_F18: str = _T("F18"); break;
	case VK_F19: str = _T("F19"); break;
	case VK_F20: str = _T("F20"); break;
	case VK_F21: str = _T("F21"); break;
	case VK_F22: str = _T("F22"); break;
	case VK_F23: str = _T("F23"); break;
	case VK_F24: str = _T("F24"); break;
	case VK_NUMLOCK: str = _T("Numlock"); break;
	case VK_SCROLL: str = _T("Scroll"); break;
//	case VK_OEM_NEC_EQUAL: str = _T("OEM NEC Equal"); break;
	case VK_OEM_FJ_JISHO: str = _T("OEM FJ Jisho"); break;
	case VK_OEM_FJ_MASSHOU: str = _T("OEM FJ Msshou"); break;
	case VK_OEM_FJ_TOUROKU: str = _T("OEM FJ Touroku"); break;
	case VK_OEM_FJ_LOYA: str = _T("OEM FJ Loya"); break;
	case VK_OEM_FJ_ROYA: str = _T("OEM FJ Roya"); break;
	case VK_LSHIFT: str = _T("LShift"); break;
	case VK_RSHIFT: str = _T("RShift"); break;
	case VK_LCONTROL: str = _T("LCtrl"); break;
	case VK_RCONTROL: str = _T("RCtrl"); break;
	case VK_LMENU: str = _T("LAlt"); break;
	case VK_RMENU: str = _T("RAlt"); break;
	case VK_BROWSER_BACK: str = _T("Browser Back"); break;
	case VK_BROWSER_FORWARD: str = _T("Browser Forward"); break;
	case VK_BROWSER_REFRESH: str = _T("Browser Refresh"); break;
	case VK_BROWSER_STOP: str = _T("Browser Stop"); break;
	case VK_BROWSER_SEARCH: str = _T("Browser Search"); break;
	case VK_BROWSER_FAVORITES: str = _T("Browser Favorites"); break;
	case VK_BROWSER_HOME: str = _T("Browser Home"); break;
	case VK_VOLUME_MUTE: str = _T("Volume Mute"); break;
	case VK_VOLUME_DOWN: str = _T("Volume Down"); break;
	case VK_VOLUME_UP: str = _T("Volume Up"); break;
	case VK_MEDIA_NEXT_TRACK: str = _T("Media Next Track"); break;
	case VK_MEDIA_PREV_TRACK: str = _T("Media Prev Track"); break;
	case VK_MEDIA_STOP: str = _T("Media Stop"); break;
	case VK_MEDIA_PLAY_PAUSE: str = _T("Media Play/Pause"); break;
	case VK_LAUNCH_MAIL: str = _T("Launch Mail"); break;
	case VK_LAUNCH_MEDIA_SELECT: str = _T("Launch Media Select"); break;
	case VK_LAUNCH_APP1: str = _T("Launch App1"); break;
	case VK_LAUNCH_APP2: str = _T("Launch App2"); break;
	case VK_OEM_1: str = _T("OEM 1"); break;
	case VK_OEM_PLUS: str = _T("Plus"); break;
	case VK_OEM_COMMA: str = _T("Comma"); break;
	case VK_OEM_MINUS: str = _T("Minus"); break;
	case VK_OEM_PERIOD: str = _T("Period"); break;
	case VK_OEM_2: str = _T("OEM 2"); break;
	case VK_OEM_3: str = _T("OEM 3"); break;
	case VK_OEM_4: str = _T("OEM 4"); break;
	case VK_OEM_5: str = _T("OEM 5"); break;
	case VK_OEM_6: str = _T("OEM 6"); break;
	case VK_OEM_7: str = _T("OEM 7"); break;
	case VK_OEM_8: str = _T("OEM 8"); break;
	case VK_OEM_AX: str = _T("OEM AX"); break;
	case VK_OEM_102: str = _T("OEM 102"); break;
	case VK_ICO_HELP: str = _T("ICO Help"); break;
	case VK_ICO_00: str = _T("ICO 00"); break;
	case VK_PROCESSKEY: str = _T("Process Key"); break;
	case VK_ICO_CLEAR: str = _T("ICO Clear"); break;
	case VK_PACKET: str = _T("Packet"); break;
	case VK_OEM_RESET: str = _T("OEM Reset"); break;
	case VK_OEM_JUMP: str = _T("OEM Jump"); break;
	case VK_OEM_PA1: str = _T("OEM PA1"); break;
	case VK_OEM_PA2: str = _T("OEM PA2"); break;
	case VK_OEM_PA3: str = _T("OEM PA3"); break;
	case VK_OEM_WSCTRL: str = _T("OEM WSCtrl"); break;
	case VK_OEM_CUSEL: str = _T("OEM CUSEL"); break;
	case VK_OEM_ATTN: str = _T("OEM ATTN"); break;
	case VK_OEM_FINISH: str = _T("OEM Finish"); break;
	case VK_OEM_COPY: str = _T("OEM Copy"); break;
	case VK_OEM_AUTO: str = _T("OEM Auto"); break;
	case VK_OEM_ENLW: str = _T("OEM ENLW"); break;
	case VK_OEM_BACKTAB: str = _T("OEM Backtab"); break;
	case VK_ATTN: str = _T("ATTN"); break;
	case VK_CRSEL: str = _T("CRSEL"); break;
	case VK_EXSEL: str = _T("EXSEL"); break;
	case VK_EREOF: str = _T("EREOF"); break;
	case VK_PLAY: str = _T("Play"); break;
	case VK_ZOOM: str = _T("Zoom"); break;
	case VK_NONAME: str = _T("Noname"); break;
	case VK_PA1: str = _T("PA1"); break;
	case VK_OEM_CLEAR: str = _T("OEM Clear"); break;
	default: 
		if('0' <= a.key && a.key <= '9' || 'A' <= a.key && a.key <= 'Z')
			str.Format(_T("%c"), (TCHAR)a.key);
		break;
	}

	if(str.IsEmpty() || !(a.fVirt&1))
		str.Format(_T("%c"), (TCHAR)a.key);

	if(a.fVirt&(FCONTROL|FALT|FSHIFT)) 
		str = MakeAccelModLabel(a.fVirt) + _T(" + ") + str;

	str.Replace(_T(" + "), _T("+"));

	return(str);
}

CString CPPageAccelTbl::MakeMouseButtonLabel(UINT mouse)
{
	CString ret;
// 	switch(mouse)
// 	{
// 	case wmcmd::NONE: default: ret = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_NONE); break;
// 	case wmcmd::LDOWN: ret = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_MOUSE_LEFT_DOWN); break;
// 	case wmcmd::LUP: ret = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_MOUSE_LEFT_UP); break;
// 	case wmcmd::LDBLCLK: ret = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_MOUSE_LEFT_DBLCLK); break;
// 	case wmcmd::MDOWN: ret = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_MOUSE_MID_DOWN); break;
// 	case wmcmd::MUP: ret = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_MOUSE_MID_UP); break;
// 	case wmcmd::MDBLCLK: ret = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_MOUSE_MID_DBLCLK); break;
// 	case wmcmd::RDOWN: ret = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_MOUSE_RIGHT_DOWN); break;
// 	case wmcmd::RUP: ret = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_MOUSE_RIGHT_UP); break;
// 	case wmcmd::RDBLCLK: ret = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_MOUSE_RIGHT_DBLCLK); break;
// 	case wmcmd::X1DOWN: ret = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_MOUSE_4_DOWN); break;
// 	case wmcmd::X1UP: ret = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_MOUSE_4_UP); break;
// 	case wmcmd::X1DBLCLK: ret = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_MOUSE_4_DBLCLK); break;
// 	case wmcmd::X2DOWN: ret = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_MOUSE_5_DOWN); break;
// 	case wmcmd::X2UP: ret = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_MOUSE_5_UP); break;
// 	case wmcmd::X2DBLCLK: ret = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_MOUSE_5_DBLCLK); break;
// 	case wmcmd::WUP: ret = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_MWHEEL_UP); break;
// 	case wmcmd::WDOWN: ret = ResStr(IDS_ACCEL_HOTKEY_KEYNAME_MWHEEL_DOWN); break;
// 	}
	return ret;
}

CString CPPageAccelTbl::MakeAppCommandLabel(UINT id)
{
	CString str;

	ASSERT(id <= APPCOMMAND_LAST);

	switch(id)
	{
	default: str = _T(""); break;
	case APPCOMMAND_BROWSER_BACKWARD: str = _T("BROWSER_BACKWARD"); break;
	case APPCOMMAND_BROWSER_FORWARD: str = _T("BROWSER_FORWARD"); break;
	case APPCOMMAND_BROWSER_REFRESH: str = _T("BROWSER_REFRESH"); break;
	case APPCOMMAND_BROWSER_STOP: str = _T("BROWSER_STOP"); break;
	case APPCOMMAND_BROWSER_SEARCH: str = _T("BROWSER_SEARCH"); break;
	case APPCOMMAND_BROWSER_FAVORITES: str = _T("BROWSER_FAVORITES"); break;
	case APPCOMMAND_BROWSER_HOME: str = _T("BROWSER_HOME"); break;
	case APPCOMMAND_VOLUME_MUTE: str = _T("VOLUME_MUTE"); break;
	case APPCOMMAND_VOLUME_DOWN: str = _T("VOLUME_DOWN"); break;
	case APPCOMMAND_VOLUME_UP: str = _T("VOLUME_UP"); break;
	case APPCOMMAND_MEDIA_NEXTTRACK: str = _T("MEDIA_NEXTTRACK"); break;
	case APPCOMMAND_MEDIA_PREVIOUSTRACK: str = _T("MEDIA_PREVIOUSTRACK"); break;
	case APPCOMMAND_MEDIA_STOP: str = _T("MEDIA_STOP"); break;
	case APPCOMMAND_MEDIA_PLAY_PAUSE: str = _T("MEDIA_PLAY_PAUSE"); break;
	case APPCOMMAND_LAUNCH_MAIL: str = _T("LAUNCH_MAIL"); break;
	case APPCOMMAND_LAUNCH_MEDIA_SELECT: str = _T("LAUNCH_MEDIA_SELECT"); break;
	case APPCOMMAND_LAUNCH_APP1: str = _T("LAUNCH_APP1"); break;
	case APPCOMMAND_LAUNCH_APP2: str = _T("LAUNCH_APP2"); break;
	case APPCOMMAND_BASS_DOWN: str = _T("BASS_DOWN"); break;
	case APPCOMMAND_BASS_BOOST: str = _T("BASS_BOOST"); break;
	case APPCOMMAND_BASS_UP: str = _T("BASS_UP"); break;
	case APPCOMMAND_TREBLE_DOWN: str = _T("TREBLE_DOWN"); break;
	case APPCOMMAND_TREBLE_UP: str = _T("TREBLE_UP"); break;
	case APPCOMMAND_MICROPHONE_VOLUME_MUTE: str = _T("MICROPHONE_VOLUME_MUTE"); break;
	case APPCOMMAND_MICROPHONE_VOLUME_DOWN: str = _T("MICROPHONE_VOLUME_DOWN"); break;
	case APPCOMMAND_MICROPHONE_VOLUME_UP: str = _T("MICROPHONE_VOLUME_UP"); break;
	case APPCOMMAND_HELP: str = _T("HELP"); break;
	case APPCOMMAND_FIND: str = _T("FIND"); break;
	case APPCOMMAND_NEW: str = _T("NEW"); break;
	case APPCOMMAND_OPEN: str = _T("OPEN"); break;
	case APPCOMMAND_CLOSE: str = _T("CLOSE"); break;
	case APPCOMMAND_SAVE: str = _T("SAVE"); break;
	case APPCOMMAND_PRINT: str = _T("PRINT"); break;
	case APPCOMMAND_UNDO: str = _T("UNDO"); break;
	case APPCOMMAND_REDO: str = _T("REDO"); break;
	case APPCOMMAND_COPY: str = _T("COPY"); break;
	case APPCOMMAND_CUT: str = _T("CUT"); break;
	case APPCOMMAND_PASTE: str = _T("PASTE"); break;
	case APPCOMMAND_REPLY_TO_MAIL: str = _T("REPLY_TO_MAIL"); break;
	case APPCOMMAND_FORWARD_MAIL: str = _T("FORWARD_MAIL"); break;
	case APPCOMMAND_SEND_MAIL: str = _T("SEND_MAIL"); break;
	case APPCOMMAND_SPELL_CHECK: str = _T("SPELL_CHECK"); break;
	case APPCOMMAND_DICTATE_OR_COMMAND_CONTROL_TOGGLE: str = _T("DICTATE_OR_COMMAND_CONTROL_TOGGLE"); break;
	case APPCOMMAND_MIC_ON_OFF_TOGGLE: str = _T("MIC_ON_OFF_TOGGLE"); break;
	case APPCOMMAND_CORRECTION_LIST: str = _T("CORRECTION_LIST"); break;
	case APPCOMMAND_MEDIA_PLAY: str = _T("MEDIA_PLAY"); break;
	case APPCOMMAND_MEDIA_PAUSE: str = _T("MEDIA_PAUSE"); break;
	case APPCOMMAND_MEDIA_RECORD: str = _T("MEDIA_RECORD"); break;
	case APPCOMMAND_MEDIA_FAST_FORWARD: str = _T("MEDIA_FAST_FORWARD"); break;
	case APPCOMMAND_MEDIA_REWIND: str = _T("MEDIA_REWIND"); break;
	case APPCOMMAND_MEDIA_CHANNEL_UP: str = _T("MEDIA_CHANNEL_UP"); break;
	case APPCOMMAND_MEDIA_CHANNEL_DOWN: str = _T("MEDIA_CHANNEL_DOWN"); break;
	}

	return str;
}
void CPPageAccelTbl::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_WinLircAddr);
	DDX_Control(pDX, IDC_EDIT1, m_WinLircEdit);
	DDX_Control(pDX, IDC_STATICLINK, m_WinLircLink);
	DDX_Check(pDX, IDC_CHECK1, m_fWinLirc);
	DDX_Text(pDX, IDC_EDIT2, m_UIceAddr);
	DDX_Control(pDX, IDC_EDIT2, m_UIceEdit);
	DDX_Control(pDX, IDC_STATICLINK2, m_UIceLink);
	DDX_Check(pDX, IDC_CHECK9, m_fUIce);
}

BEGIN_MESSAGE_MAP(CPPageAccelTbl, CPPageBase)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_LIST1, OnBeginlabeleditList)
	ON_NOTIFY(LVN_DOLABELEDIT, IDC_LIST1, OnDolabeleditList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST1, OnEndlabeleditList)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

// CPPageAccelTbl message handlers

BOOL CPPageAccelTbl::OnInitDialog()
{
	__super::OnInitDialog();

	AppSettings& s = AfxGetAppSettings();

// 	m_wmcmds.RemoveAll();
// 	m_wmcmds.AddTail(&s.wmcmds);
	m_fWinLirc = s.fWinLirc;
	m_WinLircAddr = s.WinLircAddr;
	m_fUIce = s.fUIce;
	m_UIceAddr = s.UIceAddr;

	UpdateData(FALSE);

	CRect r;
	GetDlgItem(IDC_PLACEHOLDER)->GetWindowRect(r);
	ScreenToClient(r);

	m_list.CreateEx(
		WS_EX_CLIENTEDGE, 
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_TABSTOP|LVS_REPORT|LVS_AUTOARRANGE|LVS_SHOWSELALWAYS, 
		r, this, IDC_LIST1);

	m_list.SetExtendedStyle(m_list.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_DOUBLEBUFFER);

	for(int i = 0, j = m_list.GetHeaderCtrl()->GetItemCount(); i < j; i++) m_list.DeleteColumn(0);
	m_list.InsertColumn(COL_CMD, ResStr(IDS_ACCEL_HOTKEY_KEYNAME_COMMAND), LVCFMT_LEFT, 80);
	m_list.InsertColumn(COL_MOD, ResStr(IDS_ACCEL_HOTKEY_COLUMN_HEADER_VKEY), LVCFMT_LEFT, 40);
	m_list.InsertColumn(COL_KEY, ResStr(IDS_ACCEL_HOTKEY_COLUMN_HEADER_KEY), LVCFMT_LEFT, 40);
	m_list.InsertColumn(COL_MOUSE, ResStr(IDS_ACCEL_HOTKEY_COLUMN_HEADER_MOUSE), LVCFMT_LEFT, 80);
	m_list.InsertColumn(COL_TYPE, ResStr(IDS_ACCEL_HOTKEY_COLUMN_HEADER_TYPE), LVCFMT_LEFT, 40);
	m_list.InsertColumn(COL_ID, _T("ID"), LVCFMT_LEFT, 40);
	m_list.InsertColumn(COL_APPCMD, ResStr(IDS_ACCEL_HOTKEY_COLUMN_HEADER_MEDIAKEY), LVCFMT_LEFT, 90);
	m_list.InsertColumn(COL_RMCMD, _T("RemoteCmd"), LVCFMT_LEFT, 80);
	m_list.InsertColumn(COL_RMREPCNT, _T("RepCnt"), LVCFMT_CENTER, 60);

// 	POSITION pos = m_wmcmds.GetHeadPosition();
//     for(int i = 0; pos; i++)
// 	{
// 		int row = m_list.InsertItem(m_list.GetItemCount(), m_wmcmds.GetAt(pos).name, COL_CMD);
// 		m_list.SetItemData(row, (DWORD_PTR)pos);
// 		m_wmcmds.GetNext(pos);
// 	}

	SetupList();

	m_list.SetColumnWidth(COL_CMD, LVSCW_AUTOSIZE);
	m_list.SetColumnWidth(COL_MOD, LVSCW_AUTOSIZE);
	m_list.SetColumnWidth(COL_KEY, LVSCW_AUTOSIZE);
	m_list.SetColumnWidth(COL_TYPE, LVSCW_AUTOSIZE);
	m_list.SetColumnWidth(COL_ID, LVSCW_AUTOSIZE_USEHEADER);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPPageAccelTbl::OnApply()
{
	UpdateData();

	AppSettings& s = AfxGetAppSettings();

// 	s.wmcmds.RemoveAll();
// 	s.wmcmds.AddTail(&m_wmcmds);
// 
// 	CAtlArray<ACCEL> pAccel;
// 	pAccel.SetCount(m_wmcmds.GetCount());
// 	POSITION pos = m_wmcmds.GetHeadPosition();
// 	for(int i = 0; pos; i++) pAccel[i] = m_wmcmds.GetNext(pos);
// 	if(s.hAccel) DestroyAcceleratorTable(s.hAccel);
// 	s.hAccel = CreateAcceleratorTable(pAccel.GetData(), pAccel.GetCount());

 	GetParentFrame()->m_hAccelTable = s.hAccel;

// 	s.fWinLirc = !!m_fWinLirc;
// 	s.WinLircAddr = m_WinLircAddr;
// 	if(s.fWinLirc) s.WinLircClient.Connect(m_WinLircAddr);
// 	s.fUIce = !!m_fUIce;
// 	s.UIceAddr = m_UIceAddr;
// 	if(s.fUIce) s.UIceClient.Connect(m_UIceAddr);

	s.RegGlobalAccelKey(NULL);
	return __super::OnApply();
}

void CPPageAccelTbl::OnBnClickedButton1()
{
	m_list.SetFocus();

	for(int i = 0, j = m_list.GetItemCount(); i < j; i++)
	{
		m_list.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
	}
}

void CPPageAccelTbl::OnBnClickedButton2()
{
	m_list.SetFocus();

	POSITION pos = m_list.GetFirstSelectedItemPosition();
	if(!pos) return;

	AppSettings& s = AfxGetAppSettings();

// 	while(pos)
// 	{
// 		wmcmd& wc = m_wmcmds.GetAt((POSITION)m_list.GetItemData(m_list.GetNextSelectedItem(pos)));
// 		wc.Restore();
// 	}

	SetupList();

	SetModified();
}

void CPPageAccelTbl::OnBeginlabeleditList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM* pItem = &pDispInfo->item;

	*pResult = FALSE;

	if(pItem->iItem < 0) 
		return;

	if(pItem->iSubItem == COL_MOD || pItem->iSubItem == COL_KEY || pItem->iSubItem == COL_TYPE
	|| pItem->iSubItem == COL_MOUSE || pItem->iSubItem == COL_APPCMD 
	|| pItem->iSubItem == COL_RMCMD || pItem->iSubItem == COL_RMREPCNT)
	{
		*pResult = TRUE;
	}
}

static BYTE s_mods[] = {0,FALT,FCONTROL,FSHIFT,FCONTROL|FALT,FCONTROL|FSHIFT,FALT|FSHIFT,FCONTROL|FALT|FSHIFT};

void CPPageAccelTbl::OnDolabeleditList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM* pItem = &pDispInfo->item;

	*pResult = FALSE;

	if(pItem->iItem < 0) 
		return;

//   wmcmd& wc = m_wmcmds.GetAt((POSITION)m_list.GetItemData(pItem->iItem));
// 	ACCEL& a = wc;
// 
// 	CAtlList<CString> sl;
// 	int nSel = -1;
// 
// 	if(pItem->iSubItem == COL_MOD)
// 	{
// 		for(int i = 0; i < countof(s_mods); i++)
// 		{
// 			sl.AddTail(MakeAccelModLabel(s_mods[i]));
// 			if((a.fVirt&~3) == s_mods[i]) nSel = i;
// 		}
// 
// 		m_list.ShowInPlaceComboBox(pItem->iItem, pItem->iSubItem, sl, nSel);
// 
// 		*pResult = TRUE;
// 	}
// 	else if(pItem->iSubItem == COL_KEY)
// 	{
// 		for(int i = 0; i < 256; i++)
// 		{
// 			sl.AddTail(MakeAccelVkeyLabel(i, a.fVirt&FVIRTKEY));
// 			if(a.key == i) nSel = i;
// 		}
// 
// 		m_list.ShowInPlaceComboBox(pItem->iItem, pItem->iSubItem, sl, nSel);
// 
// 		*pResult = TRUE;
// 	}
// 	else if(pItem->iSubItem == COL_TYPE)
// 	{
// 		sl.AddTail(_T("VIRTKEY"));
// 		sl.AddTail(_T("ASCII"));
// 
// 		nSel = !(a.fVirt&FVIRTKEY);
// 
// 		m_list.ShowInPlaceComboBox(pItem->iItem, pItem->iSubItem, sl, nSel);
// 
// 		*pResult = TRUE;
// 	}
// 	else if(pItem->iSubItem == COL_MOUSE)
// 	{
// 		for(UINT i = 0; i < wmcmd::LAST; i++)
// 		{
// 			sl.AddTail(MakeMouseButtonLabel(i));
// 			if(wc.mouse == i) nSel = i;
// 		}
// 
// 		m_list.ShowInPlaceComboBox(pItem->iItem, pItem->iSubItem, sl, nSel);
// 
// 		*pResult = TRUE;
// 	}
// 	else if(pItem->iSubItem == COL_APPCMD)
// 	{
// 		for(int i = 0; i <= APPCOMMAND_LAST; i++)
// 		{
// 			sl.AddTail(MakeAppCommandLabel(i));
// 			if(wc.appcmd == i) nSel = i;
// 		}
// 
// 		m_list.ShowInPlaceComboBox(pItem->iItem, pItem->iSubItem, sl, nSel);
// 
// 		*pResult = TRUE;
// 	}
// 	else if(pItem->iSubItem == COL_RMCMD)
// 	{
// 		m_list.ShowInPlaceEdit(pItem->iItem, pItem->iSubItem);
// 
// 		*pResult = TRUE;
// 	}
// 	else if(pItem->iSubItem == COL_RMREPCNT)
// 	{
// 		m_list.ShowInPlaceEdit(pItem->iItem, pItem->iSubItem);
// 
// 		*pResult = TRUE;
// 	}
}

void CPPageAccelTbl::OnEndlabeleditList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM* pItem = &pDispInfo->item;

	*pResult = FALSE;

	if(!m_list.m_fInPlaceDirty)
		return;

	if(pItem->iItem < 0) 
		return;

// 	wmcmd& wc = m_wmcmds.GetAt((POSITION)m_list.GetItemData(pItem->iItem));
// 
// 	if(pItem->iSubItem == COL_MOD)
// 	{
// 		if(pItem->lParam >= 0 && pItem->lParam < countof(s_mods))
// 		{
// 			wc.fVirt = (wc.fVirt&3) | (s_mods[pItem->lParam]&~3);
// 			m_list.SetItemText(pItem->iItem, COL_MOD, pItem->pszText);
// 			*pResult = TRUE;
// 		}
// 	}
// 	else if(pItem->iSubItem == COL_KEY)
// 	{
// 		int i = pItem->lParam;
// 		if(i >= 0 && i < 256)
// 		{
// 			wc.key = (WORD)i;
// 			m_list.SetItemText(pItem->iItem, COL_KEY, pItem->pszText);
// 			*pResult = TRUE;
// 		}
// 	}
// 	else if(pItem->iSubItem == COL_TYPE)
// 	{
// 		int i = pItem->lParam;
// 		if(i >= 0 && i < 2)
// 		{
// 			wc.fVirt = (wc.fVirt&~FVIRTKEY) | (i == 0 ? FVIRTKEY : 0);
// 			m_list.SetItemText(pItem->iItem, COL_KEY, MakeAccelVkeyLabel(wc.key, wc.fVirt&FVIRTKEY));
// 			m_list.SetItemText(pItem->iItem, COL_TYPE, (wc.fVirt&FVIRTKEY)?_T("VIRTKEY"):_T("ASCII"));
// 			*pResult = TRUE;
// 		}
// 	}
// 	else if(pItem->iSubItem == COL_APPCMD)
// 	{
// 		int i = pItem->lParam;
// 		if(i >= 0 && i <= APPCOMMAND_LAST)
// 		{
// 			wc.appcmd = (WORD)i;
// 			m_list.SetItemText(pItem->iItem, COL_APPCMD, pItem->pszText);
// 			*pResult = TRUE;
// 		}
// 	}
// 	else if(pItem->iSubItem == COL_MOUSE)
// 	{
// 		wc.mouse = pItem->lParam;
// 		m_list.SetItemText(pItem->iItem, COL_MOUSE, pItem->pszText);
// 	}
// 	else if(pItem->iSubItem == COL_RMCMD)
// 	{
// 		wc.rmcmd = CStringA(CString(pItem->pszText)).Trim();
// 		wc.rmcmd.Replace(' ', '_');
// 		m_list.SetItemText(pItem->iItem, pItem->iSubItem, CString(wc.rmcmd));
// 		*pResult = TRUE;
// 	}
// 	else if(pItem->iSubItem == COL_RMREPCNT)
// 	{
// 		CString str = CString(pItem->pszText).Trim();
// 		wc.rmrepcnt = _tcstol(str, NULL, 10);
// 		str.Format(_T("%d"), wc.rmrepcnt);
// 		m_list.SetItemText(pItem->iItem, pItem->iSubItem, str);
// 		*pResult = TRUE;
// 	}

	if(*pResult)
		SetModified();
}


void CPPageAccelTbl::OnTimer(UINT nIDEvent)
{
	UpdateData();

// 	if(m_fWinLirc)
// 	{
// 		CString addr;
// 		m_WinLircEdit.GetWindowText(addr);
// 		AfxGetAppSettings().WinLircClient.Connect(addr);
// 	}
// 
// 	m_WinLircEdit.Invalidate();
// 
// 	if(m_fUIce)
// 	{
// 		CString addr;
// 		m_UIceEdit.GetWindowText(addr);
// 		AfxGetAppSettings().UIceClient.Connect(addr);
// 	}
// 
// 	m_UIceEdit.Invalidate();

	m_counter++;

	__super::OnTimer(nIDEvent);
}

HBRUSH CPPageAccelTbl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);

	int status = -1;

// 	if(*pWnd == m_WinLircEdit)
// 		status = AfxGetAppSettings().WinLircClient.GetStatus();
// 	else if(*pWnd == m_UIceEdit)
// 		status = AfxGetAppSettings().UIceClient.GetStatus();

	if(status == 0 || status == 2 && (m_counter&1))
		pDC->SetTextColor(0x0000ff);
	else if(status == 1) 
		pDC->SetTextColor(0x008000);

	return hbr;
}

BOOL CPPageAccelTbl::OnSetActive()
{
	SetTimer(1, 1000, NULL);

	return CPPageBase::OnSetActive();
}

BOOL CPPageAccelTbl::OnKillActive()
{
	KillTimer(1);

	return CPPageBase::OnKillActive();
}

