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

// OpenCapDeviceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "mplayerc.h"
#include "OpenCapDeviceDlg.h"
#include "..\..\DSUtil\DSUtil.h"

static struct cc_t {long code; AnalogVideoStandard standard; const TCHAR* str;} s_countrycodes[] = 
{
	{1, AnalogVideo_NTSC_M, _T("USA")},
/*	{1, AnalogVideo_NTSC_M, _T("Anguilla")},
	{1, AnalogVideo_NTSC_M, _T("Antigua")},
	{1, AnalogVideo_NTSC_M, _T("Bahamas")},
	{1, AnalogVideo_NTSC_M, _T("Barbados")},
	{1, AnalogVideo_NTSC_M, _T("Bermuda")},
	{1, AnalogVideo_NTSC_M, _T("British Virgin Islands")},
	{1, AnalogVideo_NTSC_M, _T("Canada")},
	{1, AnalogVideo_NTSC_M, _T("Cayman Islands")},
	{1, AnalogVideo_NTSC_M, _T("Dominica")},
	{1, AnalogVideo_NTSC_M, _T("Dominican Republic")},
	{1, AnalogVideo_NTSC_M, _T("Grenada")},
	{1, AnalogVideo_NTSC_M, _T("Jamaica")},
	{1, AnalogVideo_NTSC_M, _T("Montserrat")},
	{1, AnalogVideo_NTSC_M, _T("Nevis")},
	{1, AnalogVideo_NTSC_M, _T("St. Kitts")},
	{1, AnalogVideo_NTSC_M, _T("St. Vincent and the Grenadines")},
	{1, AnalogVideo_NTSC_M, _T("Trinidad and Tobago")},
	{1, AnalogVideo_NTSC_M, _T("Turks and Caicos Islands")},
	{1, AnalogVideo_NTSC_M, _T("Barbuda")},
	{1, AnalogVideo_NTSC_M, _T("Puerto Rico")},
	{1, AnalogVideo_NTSC_M, _T("Saint Lucia")},
	{1, AnalogVideo_NTSC_M, _T("United States Virgin Islands")},
*/	{2, AnalogVideo_NTSC_M, _T("Canada")},
	{7, AnalogVideo_SECAM_D, _T("Russia")},
/*	{7, AnalogVideo_SECAM_D, _T("Kazakhstan")},
	{7, AnalogVideo_SECAM_D, _T("Kyrgyzstan")},
	{7, AnalogVideo_SECAM_D, _T("Tajikistan")},
	{7, AnalogVideo_SECAM_D, _T("Turkmenistan")},
	{7, AnalogVideo_SECAM_D, _T("Uzbekistan")},
*/	{20, AnalogVideo_SECAM_B, _T("Egypt")},
	{27, AnalogVideo_PAL_I, _T("South Africa")},
	{30, AnalogVideo_SECAM_B, _T("Greece")},
	{31, AnalogVideo_PAL_B, _T("Netherlands")},
	{32, AnalogVideo_PAL_B, _T("Belgium")},
	{33, AnalogVideo_SECAM_L, _T("France")},
	{34, AnalogVideo_PAL_B, _T("Spain")},
	{36, AnalogVideo_SECAM_D, _T("Hungary")},
	{39, AnalogVideo_PAL_B, _T("Italy")},
	{39, AnalogVideo_PAL_B, _T("Vatican City")},
	{40, AnalogVideo_PAL_D, _T("Romania")},
	{41, AnalogVideo_PAL_B, _T("Switzerland")},
	{41, AnalogVideo_PAL_B, _T("Liechtenstein")},
	{43, AnalogVideo_PAL_B, _T("Austria")},
	{44, AnalogVideo_PAL_I, _T("United Kingdom")},
	{45, AnalogVideo_PAL_B, _T("Denmark")},
	{46, AnalogVideo_PAL_B, _T("Sweden")},
	{47, AnalogVideo_PAL_B, _T("Norway")},
	{48, AnalogVideo_PAL_B, _T("Poland")},
	{49, AnalogVideo_PAL_B, _T("Germany")},
	{51, AnalogVideo_NTSC_M, _T("Peru")},
	{52, AnalogVideo_NTSC_M, _T("Mexico")},
	{53, AnalogVideo_NTSC_M, _T("Cuba")},
	{53, AnalogVideo_NTSC_M, _T("Guantanamo Bay")},
	{54, AnalogVideo_PAL_N, _T("Argentina")},
	{55, AnalogVideo_PAL_M, _T("Brazil")},
	{56, AnalogVideo_NTSC_M, _T("Chile")},
	{57, AnalogVideo_NTSC_M, _T("Colombia")},
	{58, AnalogVideo_NTSC_M, _T("Bolivarian Republic of Venezuela")},
	{60, AnalogVideo_PAL_B, _T("Malaysia")},
	{61, AnalogVideo_PAL_B, _T("Australia")},
	// {61, AnalogVideo_NTSC_M, _T("Cocos-Keeling Islands")},
	{62, AnalogVideo_PAL_B, _T("Indonesia")},
	{63, AnalogVideo_NTSC_M, _T("Philippines")},
	{64, AnalogVideo_PAL_B, _T("New Zealand")},
	{65, AnalogVideo_PAL_B, _T("Singapore")},
	{66, AnalogVideo_PAL_B, _T("Thailand")},
	{81, AnalogVideo_NTSC_M_J, _T("Japan")},
	{82, AnalogVideo_NTSC_M, _T("Korea (South)")},
	{84, AnalogVideo_NTSC_M, _T("Vietnam")},
	{86, AnalogVideo_PAL_D, _T("China")},
	{90, AnalogVideo_PAL_B, _T("Turkey")},
	{91, AnalogVideo_PAL_B, _T("India")},
	{92, AnalogVideo_PAL_B, _T("Pakistan")},
	{93, AnalogVideo_PAL_B, _T("Afghanistan")},
	{94, AnalogVideo_PAL_B, _T("Sri Lanka")},
	{95, AnalogVideo_NTSC_M, _T("Myanmar")},
	{98, AnalogVideo_SECAM_B, _T("Iran")},
	{212, AnalogVideo_SECAM_B, _T("Morocco")},
	{213, AnalogVideo_PAL_B, _T("Algeria")},
	{216, AnalogVideo_SECAM_B, _T("Tunisia")},
	{218, AnalogVideo_SECAM_B, _T("Libya")},
	{220, AnalogVideo_SECAM_K, _T("Gambia")},
	{221, AnalogVideo_SECAM_K, _T("Senegal Republic")},
	{222, AnalogVideo_SECAM_B, _T("Mauritania")},
	{223, AnalogVideo_SECAM_K, _T("Mali")},
	{224, AnalogVideo_SECAM_K, _T("Guinea")},
	{225, AnalogVideo_SECAM_K, _T("Cote D'Ivoire")},
	{226, AnalogVideo_SECAM_K, _T("Burkina Faso")},
	{227, AnalogVideo_SECAM_K, _T("Niger")},
	{228, AnalogVideo_SECAM_K, _T("Togo")},
	{229, AnalogVideo_SECAM_K, _T("Benin")},
	{230, AnalogVideo_SECAM_B, _T("Mauritius")},
	{231, AnalogVideo_PAL_B, _T("Liberia")},
	{232, AnalogVideo_PAL_B, _T("Sierra Leone")},
	{233, AnalogVideo_PAL_B, _T("Ghana")},
	{234, AnalogVideo_PAL_B, _T("Nigeria")},
	{235, AnalogVideo_PAL_B, _T("Chad")},
	{236, AnalogVideo_PAL_B, _T("Central African Republic")},
	{237, AnalogVideo_PAL_B, _T("Cameroon")},
	{238, AnalogVideo_NTSC_M, _T("Cape Verde Islands")},
	{239, AnalogVideo_PAL_B, _T("Sao Tome and Principe")},
	{240, AnalogVideo_SECAM_B, _T("Equatorial Guinea")},
	{241, AnalogVideo_SECAM_K, _T("Gabon")},
	{242, AnalogVideo_SECAM_D, _T("Congo")},
	{243, AnalogVideo_SECAM_K, _T("Congo(DRC)")},
	{244, AnalogVideo_PAL_I, _T("Angola")},
	{245, AnalogVideo_NTSC_M, _T("Guinea-Bissau")},
	{246, AnalogVideo_NTSC_M, _T("Diego Garcia")},
	{247, AnalogVideo_NTSC_M, _T("Ascension Island")},
	{248, AnalogVideo_PAL_B, _T("Seychelle Islands")},
	{249, AnalogVideo_PAL_B, _T("Sudan")},
	{250, AnalogVideo_PAL_B, _T("Rwanda")},
	{251, AnalogVideo_PAL_B, _T("Ethiopia")},
	{252, AnalogVideo_PAL_B, _T("Somalia")},
	{253, AnalogVideo_SECAM_K, _T("Djibouti")},
	{254, AnalogVideo_PAL_B, _T("Kenya")},
	{255, AnalogVideo_PAL_B, _T("Tanzania")},
	{256, AnalogVideo_PAL_B, _T("Uganda")},
	{257, AnalogVideo_SECAM_K, _T("Burundi")},
	{258, AnalogVideo_PAL_B, _T("Mozambique")},
	{260, AnalogVideo_PAL_B, _T("Zambia")},
	{261, AnalogVideo_SECAM_K, _T("Madagascar")},
	{262, AnalogVideo_SECAM_K, _T("Reunion Island")},
	{263, AnalogVideo_PAL_B, _T("Zimbabwe")},
	{264, AnalogVideo_PAL_I, _T("Namibia")},
	{265, AnalogVideo_NTSC_M, _T("Malawi")},
	{266, AnalogVideo_PAL_I, _T("Lesotho")},
	{267, AnalogVideo_SECAM_K, _T("Botswana")},
	{268, AnalogVideo_PAL_B, _T("Swaziland")},
	{269, AnalogVideo_SECAM_K, _T("Mayotte Island")},
//	{269, AnalogVideo_NTSC_M, _T("Comoros")},
	{290, AnalogVideo_NTSC_M, _T("St. Helena")},
	{291, AnalogVideo_NTSC_M, _T("Eritrea")},
	{297, AnalogVideo_NTSC_M, _T("Aruba")},
	{298, AnalogVideo_PAL_B, _T("Faroe Islands")},
	{299, AnalogVideo_NTSC_M, _T("Greenland")},
	{350, AnalogVideo_PAL_B, _T("Gibraltar")},
	{351, AnalogVideo_PAL_B, _T("Portugal")},
	{352, AnalogVideo_PAL_B, _T("Luxembourg")},
	{353, AnalogVideo_PAL_I, _T("Ireland")},
	{354, AnalogVideo_PAL_B, _T("Iceland")},
	{355, AnalogVideo_PAL_B, _T("Albania")},
	{356, AnalogVideo_PAL_B, _T("Malta")},
	{357, AnalogVideo_PAL_B, _T("Cyprus")},
	{358, AnalogVideo_PAL_B, _T("Finland")},
	{359, AnalogVideo_SECAM_D, _T("Bulgaria")},
	{370, AnalogVideo_PAL_B, _T("Lithuania")},
	{371, AnalogVideo_SECAM_D, _T("Latvia")},
	{372, AnalogVideo_PAL_B, _T("Estonia")},
	{373, AnalogVideo_SECAM_D, _T("Moldova")},
	{374, AnalogVideo_SECAM_D, _T("Armenia")},
	{375, AnalogVideo_SECAM_D, _T("Belarus")},
	{376, AnalogVideo_NTSC_M, _T("Andorra")},
	{377, AnalogVideo_SECAM_G, _T("Monaco")},
	{378, AnalogVideo_PAL_B, _T("San Marino")},
	{380, AnalogVideo_SECAM_D, _T("Ukraine")},
	{381, AnalogVideo_PAL_B, _T("Serbia and Montenegro")},
	{385, AnalogVideo_PAL_B, _T("Croatia")},
	{386, AnalogVideo_PAL_B, _T("Slovenia")},
	{387, AnalogVideo_PAL_B, _T("Bosnia and Herzegovina")},
	{389, AnalogVideo_PAL_B, _T("F.Y.R.O.M. (Former Yugoslav Republic of Macedonia)")},
	{420, AnalogVideo_PAL_D, _T("Czech Republic")},
	{421, AnalogVideo_PAL_B, _T("Slovak Republic")},
	{500, AnalogVideo_PAL_I, _T("Falkland Islands (Islas Malvinas)")},
	{501, AnalogVideo_NTSC_M, _T("Belize")},
	{502, AnalogVideo_NTSC_M, _T("Guatemala")},
	{503, AnalogVideo_NTSC_M, _T("El Salvador")},
	{504, AnalogVideo_NTSC_M, _T("Honduras")},
	{505, AnalogVideo_NTSC_M, _T("Nicaragua")},
	{506, AnalogVideo_NTSC_M, _T("Costa Rica")},
	{507, AnalogVideo_NTSC_M, _T("Panama")},
	{508, AnalogVideo_SECAM_K, _T("St. Pierre and Miquelon")},
	{509, AnalogVideo_NTSC_M, _T("Haiti")},
	{590, AnalogVideo_SECAM_K, _T("Guadeloupe")},
//	{590, AnalogVideo_NTSC_M, _T("French Antilles")},
	{591, AnalogVideo_PAL_N, _T("Bolivia")},
	{592, AnalogVideo_SECAM_K, _T("Guyana")},
	{593, AnalogVideo_NTSC_M, _T("Ecuador")},
	{594, AnalogVideo_SECAM_K, _T("French Guiana")},
	{595, AnalogVideo_PAL_N, _T("Paraguay")},
	{596, AnalogVideo_SECAM_K, _T("Martinique")},
	{597, AnalogVideo_NTSC_M, _T("Suriname")},
	{598, AnalogVideo_PAL_N, _T("Uruguay")},
	{599, AnalogVideo_NTSC_M, _T("Netherlands Antilles")},
	{670, AnalogVideo_NTSC_M, _T("Saipan Island")},
//	{670, AnalogVideo_NTSC_M, _T("Rota Island")},
//	{670, AnalogVideo_NTSC_M, _T("Tinian Island")},
	{671, AnalogVideo_NTSC_M, _T("Guam")},
	{672, AnalogVideo_NTSC_M, _T("Christmas Island")},
	{672, AnalogVideo_NTSC_M, _T("Australian Antarctic Territory")},
	//{672, AnalogVideo_PAL_B, _T("Norfolk Island")},
	{673, AnalogVideo_PAL_B, _T("Brunei")},
	{674, AnalogVideo_NTSC_M, _T("Nauru")},
	{675, AnalogVideo_PAL_B, _T("Papua New Guinea")},
	{676, AnalogVideo_NTSC_M, _T("Tonga")},
	{677, AnalogVideo_NTSC_M, _T("Solomon Islands")},
	{678, AnalogVideo_NTSC_M, _T("Vanuatu")},
	{679, AnalogVideo_NTSC_M, _T("Fiji Islands")},
	{680, AnalogVideo_NTSC_M, _T("Palau")},
	{681, AnalogVideo_SECAM_K, _T("Wallis and Futuna Islands")},
	{682, AnalogVideo_PAL_B, _T("Cook Islands")},
	{683, AnalogVideo_NTSC_M, _T("Niue")},
	{684, AnalogVideo_NTSC_M, _T("Territory of American Samoa")},
	{685, AnalogVideo_PAL_B, _T("Samoa")},
	{686, AnalogVideo_PAL_B, _T("Kiribati Republic")},
	{687, AnalogVideo_SECAM_K, _T("New Caledonia")},
	{688, AnalogVideo_NTSC_M, _T("Tuvalu")},
	{689, AnalogVideo_SECAM_K, _T("French Polynesia")},
	{690, AnalogVideo_NTSC_M, _T("Tokelau")},
	{691, AnalogVideo_NTSC_M, _T("Micronesia")},
	{692, AnalogVideo_NTSC_M, _T("Marshall Islands")},
	{850, AnalogVideo_SECAM_D, _T("Korea (North)")},
	{852, AnalogVideo_PAL_I, _T("Hong Kong SAR")},
	{853, AnalogVideo_PAL_I, _T("Macao SAR")},
	{855, AnalogVideo_PAL_B, _T("Cambodia")},
	{856, AnalogVideo_PAL_B, _T("Laos")},
	{871, AnalogVideo_NTSC_M, _T("INMARSAT (Atlantic-East)")},
	{872, AnalogVideo_NTSC_M, _T("INMARSAT (Pacific)")},
	{873, AnalogVideo_NTSC_M, _T("INMARSAT (Indian)")},
	{874, AnalogVideo_NTSC_M, _T("INMARSAT (Atlantic-West)")},
	{880, AnalogVideo_PAL_B, _T("Bangladesh")},
	{886, AnalogVideo_NTSC_M, _T("Taiwan")},
	{960, AnalogVideo_PAL_B, _T("Maldives")},
	{961, AnalogVideo_SECAM_B, _T("Lebanon")},
	{962, AnalogVideo_PAL_B, _T("Jordan")},
	{963, AnalogVideo_SECAM_B, _T("Syria")},
	{964, AnalogVideo_SECAM_B, _T("Iraq")},
	{965, AnalogVideo_PAL_B, _T("Kuwait")},
	{966, AnalogVideo_SECAM_B, _T("Saudi Arabia")},
	{967, AnalogVideo_PAL_B, _T("Yemen")},
	{968, AnalogVideo_PAL_B, _T("Oman")},
	{971, AnalogVideo_PAL_B, _T("United Arab Emirates")},
	{972, AnalogVideo_PAL_B, _T("Israel")},
	{973, AnalogVideo_PAL_B, _T("Bahrain")},
	{974, AnalogVideo_PAL_B, _T("Qatar")},
	{975, AnalogVideo_NTSC_M, _T("Bhutan")},
	{976, AnalogVideo_SECAM_D, _T("Mongolia")},
	{977, AnalogVideo_PAL_B, _T("Nepal")},
	{994, AnalogVideo_SECAM_D, _T("Azerbaijan")},
	{995, AnalogVideo_SECAM_D,_T("Georgia")},
};

// COpenCapDeviceDlg dialog

//IMPLEMENT_DYNAMIC(COpenCapDeviceDlg, CResizableDialog)
COpenCapDeviceDlg::COpenCapDeviceDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(COpenCapDeviceDlg::IDD, pParent)
	, m_vidstr(_T(""))
	, m_audstr(_T(""))
	, m_country(1)
{
}

COpenCapDeviceDlg::~COpenCapDeviceDlg()
{
}

void COpenCapDeviceDlg::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_vidctrl);
	DDX_Control(pDX, IDC_COMBO2, m_audctrl);
	DDX_Control(pDX, IDC_COMBO9, m_countryctrl);
}

BEGIN_MESSAGE_MAP(COpenCapDeviceDlg, CResizableDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// COpenCapDeviceDlg message handlers

BOOL COpenCapDeviceDlg::OnInitDialog()
{
	__super::OnInitDialog();

	AddAnchor(m_vidctrl, TOP_LEFT, TOP_RIGHT);
	AddAnchor(m_audctrl, TOP_LEFT, TOP_RIGHT);
	AddAnchor(m_countryctrl, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDOK, TOP_CENTER);
	AddAnchor(IDCANCEL, TOP_CENTER);

	CRect r;
	GetWindowRect(r);
	CSize s = r.Size();
	SetMinTrackSize(s);
	s.cx = 1000;
	SetMaxTrackSize(s);

	CString dummy = _T("dummy");
	CString vidstr = AfxGetMyApp()->GetProfileString(_T("Capture"), _T("VidDispName"), dummy);
	CString audstr = AfxGetMyApp()->GetProfileString(_T("Capture"), _T("AudDispName"), dummy);
	long country = AfxGetMyApp()->GetProfileInt(_T("Capture"), _T("Country"), 1);

	int iSel = vidstr == dummy ? 0 : -1;

	BeginEnumSysDev(CLSID_VideoInputDeviceCategory, pMoniker)
	{
		CComPtr<IPropertyBag> pPB;
		pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&pPB);

		CComVariant var;
		pPB->Read(CComBSTR(_T("FriendlyName")), &var, NULL);
		int i = m_vidctrl.AddString(CString(var.bstrVal));

		LPOLESTR strName = NULL;
		if(SUCCEEDED(pMoniker->GetDisplayName(NULL, NULL, &strName)))
		{
			m_vidnames.Add(CString(strName));
			if(vidstr == CString(strName)) iSel = i;
			CoTaskMemFree(strName);
		}
	}
	EndEnumSysDev

	if(m_vidctrl.GetCount())
		m_vidctrl.SetCurSel(iSel);

	iSel = audstr == dummy ? 0 : -1;

	BeginEnumSysDev(CLSID_AudioInputDeviceCategory, pMoniker)
	{
		CComPtr<IPropertyBag> pPB;
		pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&pPB);

		CComVariant var;
		pPB->Read(CComBSTR(_T("FriendlyName")), &var, NULL);
		int i = m_audctrl.AddString(CString(var.bstrVal));

		LPOLESTR strName = NULL;
		if(SUCCEEDED(pMoniker->GetDisplayName(NULL, NULL, &strName)))
		{
			m_audnames.Add(CString(strName));
			if(audstr == CString(strName)) iSel = i;
			CoTaskMemFree(strName);
		}
	}
	EndEnumSysDev

	{
		int i = m_audctrl.AddString(_T("<Video Capture Device>"));
		m_audnames.Add(_T(""));
		if(audstr.IsEmpty()) iSel = i;
	}

	if(m_audctrl.GetCount())
		m_audctrl.SetCurSel(iSel);

	iSel = 0;

	for(int j = 0; j < countof(s_countrycodes); j++)
	{
		CString standard;
		switch(s_countrycodes[j].standard)
		{
		case AnalogVideo_NTSC_M: standard = _T("NTSC M"); break;
		case AnalogVideo_NTSC_M_J: standard = _T("NTSC M J"); break;
		case AnalogVideo_NTSC_433: standard = _T("NTSC 433"); break;
		case AnalogVideo_PAL_B: standard = _T("PAL B"); break;
		case AnalogVideo_PAL_D: standard = _T("PAL D"); break;
		case AnalogVideo_PAL_G: standard = _T("PAL G"); break;
		case AnalogVideo_PAL_H: standard = _T("PAL H"); break;
		case AnalogVideo_PAL_I: standard = _T("PAL I"); break;
		case AnalogVideo_PAL_M: standard = _T("PAL M"); break;
		case AnalogVideo_PAL_N: standard = _T("PAL N"); break;
		case AnalogVideo_PAL_60: standard = _T("PAL 60"); break;
		case AnalogVideo_SECAM_B: standard = _T("SECAM B"); break;
		case AnalogVideo_SECAM_D: standard = _T("SECAM D"); break;
		case AnalogVideo_SECAM_G: standard = _T("SECAM G"); break;
		case AnalogVideo_SECAM_H: standard = _T("SECAM H"); break;
		case AnalogVideo_SECAM_K: standard = _T("SECAM K"); break;
		case AnalogVideo_SECAM_K1: standard = _T("SECAM K1"); break;
		case AnalogVideo_SECAM_L: standard = _T("SECAM L"); break;
		case AnalogVideo_SECAM_L1: standard = _T("SECAM L1"); break;
		case AnalogVideo_PAL_N_COMBO: standard = _T("PAL N COMBO"); break;
		}

		CString str;
		str.Format(_T("%d - %s - %s"), s_countrycodes[j].code, s_countrycodes[j].str, standard);

		int i = m_countryctrl.AddString(str);
		m_countryctrl.SetItemDataPtr(i, &s_countrycodes[j]);
		if(country == s_countrycodes[j].code) iSel = i;
	}

	if(m_countryctrl.GetCount())
		m_countryctrl.SetCurSel(iSel);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COpenCapDeviceDlg::OnBnClickedOk()
{
	UpdateData();

	if(m_vidctrl.GetCurSel() >= 0) 
	{
		m_vidstr = m_vidnames[m_vidctrl.GetCurSel()];
		AfxGetMyApp()->WriteProfileString(_T("Capture"), _T("VidDispName"), m_vidstr);
	}

	if(m_audctrl.GetCurSel() >= 0)
	{
		m_audstr = m_audnames[m_audctrl.GetCurSel()];
		AfxGetMyApp()->WriteProfileString(_T("Capture"), _T("AudDispName"), m_audstr);
	}

	if(m_countryctrl.GetCurSel() >= 0)
	{
		m_country = ((cc_t*)m_countryctrl.GetItemDataPtr(m_countryctrl.GetCurSel()))->code;
		AfxGetMyApp()->WriteProfileInt(_T("Capture"), _T("Country"), m_country);
	}

	OnOK();
}
