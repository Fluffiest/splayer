#pragma once
#include "stdafx.h"
#include "mplayerc.h"
#include "SUIButton.h"


//�����ʾ���
//mouseoverʱ��ʾ��͸�������������رհ�ť�ͱ�ɫ��ť��out�����ʾ����
//Ĭ������Ļ�ײ�������ʾ������ͨ����ק�ı�λ��
//�رպ��Ϊ�������ڽ�������ʾ

class SVPLycShowBox :
	public CFrameWnd
{
private:

	//����Ӧ��Ϊ͸��
	//ChangeBackgroundColor( COLORREF rgb_color );
	int ChangeFontColor( COLORREF rgb_color );


	/*
	��ק���ı䱳����ɫ , �������ɫ�ʱ仯
	ÿ60��һ������
	����+R ��+G ����+B
	����+R ��-G ����-B
	like a invisible color picker
	*/
	int ChangeFontColorByMouse(CPoint movement);

public:
	SVPLycShowBox(void);
	~SVPLycShowBox(void);

	/* 
	��ʾ������ֺ������ʱ��
	*/

	int ShowLycLine(CString szLycLine, int iLastingTime);

//Standard Message Loop Shits
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMove(int x, int y);

	void DoUpdateWindow();

	CBitmap m_bmpWnd;
	SIZE m_sizeBmpWnd;
	int m_nBmpWndPadding;
	CFont m_f;

	long m_nCurrentMouseAction;
	BOOL m_bMouseInAction;
	POINT m_ptMouse;

	//���� POPUP �� Layered Window
	HWND m_wndNewOSD;
};
