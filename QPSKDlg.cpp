
// QPSKDlg.cpp: файл реализации
//

#include "pch.h"
#include "framework.h"
#include "QPSK.h"
#include "QPSKDlg.h"
#include "afxdialogex.h"
#include "AudioFile/AudioFile.h"
using namespace std;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define KOORDGET(x,y) (xpget*((x)-xminget)),(ypget*((y)-ymaxget)) 


CQPSKDlg::CQPSKDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_QPSK_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


void CQPSKDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CQPSKDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_GET_INPUT_BITS, &CQPSKDlg::OnBnClickedGetInputBits)
	ON_BN_CLICKED(IDC_GET_GENERAL_SIGN, &CQPSKDlg::OnBnClickedGetGeneralSign)
	ON_BN_CLICKED(IDC_GET_IQ, &CQPSKDlg::OnBnClickedGetIq)
	ON_BN_CLICKED(IDC_GET_QPSK, &CQPSKDlg::OnBnClickedGetQpsk)
END_MESSAGE_MAP()


BOOL CQPSKDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	PicWndBits = GetDlgItem(IDC_BITS);
	PicDcBits = PicWndBits->GetDC();
	PicWndBits->GetClientRect(&PicBits);

	PicWndGeneral = GetDlgItem(IDC_GENERAL_SIGN);
	PicDcGeneral = PicWndGeneral->GetDC();
	PicWndGeneral->GetClientRect(&PicGeneral);

	PicWndIComp = GetDlgItem(IDC_I_COMPONENT);
	PicDcIComp = PicWndIComp->GetDC();
	PicWndIComp->GetClientRect(&PicIComp);

	PicWndQComp = GetDlgItem(IDC_Q_COMPONENT);
	PicDcQComp = PicWndQComp->GetDC();
	PicWndQComp->GetClientRect(&PicQComp);

	PicWndQPSK = GetDlgItem(IDC_QPSK_SIGNAL);
	PicDcQPSK = PicWndQPSK->GetDC();
	PicWndQPSK->GetClientRect(&PicQPSK);

	PicWndQPSKSpec = GetDlgItem(IDC_QPSK_SPECTRUM);
	PicDcQPSKSpec = PicWndQPSKSpec->GetDC();
	PicWndQPSKSpec->GetClientRect(&PicQPSKSpec);

	graphPen.CreatePen(PS_SOLID, 2, RGB(20, 220, 0));
	UpdateData(false);
	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}


void CQPSKDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}


HCURSOR CQPSKDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CQPSKDlg::DrawGraph(vector<vector<double>>& Mass, double MinX, double MaxX, vector<CPen*> GraphPen, CDC* WinDc, CRect WinPic)
{
	//----- поиск максимального и минимального значения -----------------------------

	vector<double> MaxY;
	MaxY.resize(Mass.size());

	vector<double> MinY;
	MinY.resize(Mass.size());

	for (int i = 0; i < Mass.size(); i++)
	{
		for (int j = 0; j < Mass[i].size(); j++)
		{
			if (MaxY[i] < Mass[i][j]) MaxY[i] = Mass[i][j];
			if (MinY[i] > Mass[i][j]) MinY[i] = Mass[i][j];
		}
	}


	xminget = MinX;			//минимальное значение х
	xmaxget = MaxX;			//максимальное значение х
	yminget = -1;			//минимальное значение y
	ymaxget = 0;			//максимальное значение y

	for (int i = 0; i < MaxY.size(); i++)
	{
		if (MaxY[i] > ymaxget) ymaxget = MaxY[i];
		if (MinY[i] < yminget) yminget = MinY[i];
	}

	yminget = -ymaxget;

	xpget = ((double)(WinPic.Width()) / (xmaxget - xminget));			//Коэффициенты пересчёта координат по Х
	ypget = -((double)(WinPic.Height()) / (ymaxget - yminget));

	CBitmap bmp;
	CDC* MemDc;
	MemDc = new CDC;
	MemDc->CreateCompatibleDC(WinDc);
	bmp.CreateCompatibleBitmap(
		WinDc,
		WinPic.Width(),
		WinPic.Height());
	CBitmap* pBmp = (CBitmap*)MemDc->SelectObject(&bmp);
	MemDc->FillSolidRect(WinPic, RGB(255, 255, 255));

	MemDc->SelectObject(&gridPen);
	double shagX = (xmaxget - xminget) / 10;
	for (float x = xminget; x <= xmaxget; x += shagX)
	{
		MemDc->MoveTo(KOORDGET(x, ymaxget));
		MemDc->LineTo(KOORDGET(x, yminget));
	}
	double shagY = (ymaxget - yminget) / 10;
	//отрисовка сетки по x
	for (float y = yminget; y <= ymaxget; y += shagY)
	{
		MemDc->MoveTo(KOORDGET(xminget, y));
		MemDc->LineTo(KOORDGET(xmaxget, y));
	}

	MemDc->SelectObject(&axesPen);
	MemDc->MoveTo(KOORDGET(xminget, 0));
	MemDc->LineTo(KOORDGET(xmaxget, 0));

	//подпись осей
	MemDc->TextOutW(KOORDGET(0, ymaxget - 0.2), _T("")); //Y
	MemDc->TextOutW(KOORDGET(xmaxget - 0.3, 0 - 0.2), _T("A")); //X

	CFont font3;
	font3.CreateFontW(15, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS || CLIP_LH_ANGLES, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Times New Roman"));
	MemDc->SelectObject(font3);
	//по Y с шагом 5
	for (double i = yminget; i <= ymaxget; i += shagY)
	{
		CString str;
		str.Format(_T("%.1f"), i);
		MemDc->TextOutW(KOORDGET(0.01, i), str);
	}

	for (double j = xminget; j <= xmaxget; j += shagX)
	{
		CString str;
		str.Format(_T("%.1f"), j);
		MemDc->TextOutW(KOORDGET(j, -0.00005), str);
	}
	double shag = (MaxX - MinX) / Mass[0].size();
	//MemDc->SelectObject(&graf_pen2);
	for (int i = 0; i < Mass.size(); i++)
	{
		double x = MinX;
		MemDc->SelectObject(GraphPen[i]);
		MemDc->MoveTo(KOORDGET(MinX, Mass[i][0]));
		for (int j = 0; j < Mass[i].size(); j++)
		{
			MemDc->LineTo(KOORDGET(x, Mass[i][j]));
			x += shag;
		}
	}

	//----- вывод на экран -------------------------------------------
	WinDc->BitBlt(0, 0, WinPic.Width(), WinPic.Height(), MemDc, 0, 0, SRCCOPY);
	delete MemDc;
}


static void newFFT(vector<complex<double>>& in, int direction)
{
	//out = in;
	unsigned int pts = 2;
	while (in.size() > pts)
	{
		pts *= 2;
	}

	int pts_to_add = pts - in.size();

	for (int ttt = 0; ttt < pts_to_add; ttt++)
	{
		in.push_back(complex<double>(0, 0));
	}
	int n = in.size();

	int i, j, istep;
	int m, mmax;
	double r, r1, theta, w_r, w_i, temp_r, temp_i;

	r = 3.14159 * direction;
	j = 0;

	for (i = 0; i < n; i++)
	{
		if (i < j)
		{
			temp_r = in[j].real();
			temp_i = in[j].imag();
			in[j] = in[i];
			in[i] = complex<double>(temp_r, temp_i);
		}
		m = n >> 1;
		while (j >= m)
		{
			j -= m;
			m = (m + 1) / 2;
		}
		j += m;
	}
	mmax = 1;
	while (mmax < n)
	{
		istep = mmax << 1;
		r1 = r / (double)mmax;
		for (m = 0; m < mmax; m++)
		{
			theta = r1 * m;
			w_r = (double)cos((double)theta);
			w_i = (double)sin((double)theta);
			for (i = m; i < n; i += istep)
			{
				j = i + mmax;
				temp_r = w_r * in[j].real() - w_i * in[j].imag();
				temp_i = w_r * in[j].imag() + w_i * in[j].real();
				in[j] = complex<double>((in[i].real() - temp_r), (in[i].imag() - temp_i));
				in[i] += complex<double>(temp_r, temp_i);
			}
		}
		mmax = istep;
	}
	if (direction > 0)
	{
		double sqn = sqrt(n);
		for (i = 0; i < n; i++)
		{
			in[i] /= sqn;
		}
	}

}


void CQPSKDlg::OnBnClickedGetInputBits()
{
}


void CQPSKDlg::OnBnClickedGetGeneralSign()
{
}


void CQPSKDlg::OnBnClickedGetIq()
{
}


void CQPSKDlg::OnBnClickedGetQpsk()
{
}
