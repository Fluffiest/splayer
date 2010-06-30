// ShaderCombineDlg.cpp : implementation file
//

#include "stdafx.h"
#include "mplayerc.h"
#include "ShaderCombineDlg.h"

// CShaderCombineDlg dialog

CShaderCombineDlg::CShaderCombineDlg(CAtlList<CString>& labels, CWnd* pParent /*=NULL*/)
	: CResizableDialog(CShaderCombineDlg::IDD, pParent)
	, m_labels(labels)
{
}

CShaderCombineDlg::~CShaderCombineDlg()
{
}

void CShaderCombineDlg::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_COMBO1, m_combo);
}

BEGIN_MESSAGE_MAP(CShaderCombineDlg, CResizableDialog)
	ON_BN_CLICKED(IDC_BUTTON2, &CShaderCombineDlg::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON3, &CShaderCombineDlg::OnBnClickedButton13)
	ON_BN_CLICKED(IDC_BUTTON1, &CShaderCombineDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON4, &CShaderCombineDlg::OnBnClickedButton11)
END_MESSAGE_MAP()

// CShaderCombineDlg message handlers

BOOL CShaderCombineDlg::OnInitDialog()
{
	__super::OnInitDialog();

	AddAnchor(IDC_LIST1, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_COMBO1, BOTTOM_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_STATIC1, BOTTOM_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_BUTTON2, BOTTOM_RIGHT);
	AddAnchor(IDC_BUTTON3, BOTTOM_RIGHT);
	AddAnchor(IDC_BUTTON1, TOP_RIGHT);
	AddAnchor(IDC_BUTTON4, TOP_RIGHT);
	AddAnchor(IDOK, TOP_RIGHT);
	AddAnchor(IDCANCEL, TOP_RIGHT);

	AppSettings& s = AfxGetAppSettings();

	CString str = s.m_shadercombine.Trim();

	CAtlList<CString> sl;
	if(!str.IsEmpty()) Explode(str, sl, '|');

	POSITION pos = sl.GetHeadPosition();
	while(pos) m_list.AddString(sl.GetNext(pos));

	pos = s.m_shaders.GetHeadPosition();
	while(pos) m_combo.AddString(s.m_shaders.GetNext(pos).label);
	if(m_combo.GetCount()) m_combo.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CShaderCombineDlg::OnOK()
{
	m_labels.RemoveAll();

	CAtlList<CString> sl;

	for(int i = 0, j = m_list.GetCount(); i < j; i++)
	{
		CString label;
		m_list.GetText(i, label);
		sl.AddTail(label);
		m_labels.AddTail(label);
	}

	AfxGetAppSettings().m_shadercombine = Implode(sl, '|');

	__super::OnOK();
}

void CShaderCombineDlg::OnBnClickedButton12()
{
	int i = m_combo.GetCurSel();
	if(i < 0) return;

	CString label; 
	m_combo.GetLBText(i, label);
	m_list.SetCurSel(m_list.AddString(label));
}

void CShaderCombineDlg::OnBnClickedButton13()
{
	int i = m_list.GetCurSel();
	if(i < 0) return;

	m_list.DeleteString(i);
	if(i == m_list.GetCount()) i--;
	if(i >= 0) m_list.SetCurSel(i);
}

void CShaderCombineDlg::OnBnClickedButton1()
{
	int i = m_list.GetCurSel();
	if(i < 1) return;

	CString label;
	m_list.GetText(i, label);
	m_list.DeleteString(i);
	i--;
	m_list.InsertString(i, label);
	m_list.SetCurSel(i);
}

void CShaderCombineDlg::OnBnClickedButton11()
{
	int i = m_list.GetCurSel();
	if(i < 0 || i >= m_list.GetCount()-1) return;

	CString label;
	m_list.GetText(i, label);
	m_list.DeleteString(i);
	if(++i == m_list.GetCount()) m_list.AddString(label);
	else m_list.InsertString(i, label);
	m_list.SetCurSel(i);
}
