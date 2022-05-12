// DemodulatorQPSK.cpp: файл реализации
//

#include "pch.h"
#include "QPSK.h"
#include "DemodulatorQPSK.h"
#include "afxdialogex.h"

#define KOORDGET(x,y) (xpget*((x)-xminget)),(ypget*((y)-ymaxget)) 


// Диалоговое окно DemodulatorQPSK

IMPLEMENT_DYNAMIC(DemodulatorQPSK, CDialogEx)

DemodulatorQPSK::DemodulatorQPSK(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

DemodulatorQPSK::~DemodulatorQPSK()
{
}

void DemodulatorQPSK::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DemodulatorQPSK, CDialogEx)
	ON_BN_CLICKED(IDC_GET_QPSK, &DemodulatorQPSK::OnBnClickedGetQpsk)
	ON_BN_CLICKED(IDC_GET_I_Q, &DemodulatorQPSK::OnBnClickedGetIQ)
	ON_BN_CLICKED(IDC_GET_Sinc, &DemodulatorQPSK::OnBnClickedGetSinc)
	ON_BN_CLICKED(IDC_GET_filtr_i_q, &DemodulatorQPSK::OnBnClickedGetfiltriq)
	ON_BN_CLICKED(IDC_GET_bit_I_Q, &DemodulatorQPSK::OnBnClickedGetbitIQ)
END_MESSAGE_MAP()

void DemodulatorQPSK::DrawGraph(vector<vector<double>>& Mass, double MinX,
	double MaxX, vector<CPen*> GraphPen,
	CDC* WinDc, CRect WinPic, GrapType gType)
{
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

	xminget = MinX;
	xmaxget = MaxX;
	yminget = -1;
	ymaxget = 0;

	for (int i = 0; i < MaxY.size(); i++)
	{
		if (MaxY[i] > ymaxget) ymaxget = MaxY[i];
		if (MinY[i] < yminget) yminget = MinY[i];
	}

	if (gType == GrapType::Bits) yminget = -ymaxget * 0.1;
	else if (gType == GrapType::Graphic) yminget = -ymaxget;

	xpget = ((double)(WinPic.Width()) / (xmaxget - xminget));
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
	MemDc->FillSolidRect(WinPic, RGB(240, 240, 240));

	MemDc->SelectObject(&axesPen);
	MemDc->MoveTo(KOORDGET(xminget, 0));
	MemDc->LineTo(KOORDGET(xmaxget, 0));

	CFont font3;
	font3.CreateFontW(15, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS || CLIP_LH_ANGLES, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Times New Roman"));
	MemDc->SelectObject(font3);

	double step = (MaxX - MinX) / Mass[0].size();

	for (double j = xminget; j < xmaxget; j += step)
	{
		CString str;
		str.Format(_T("%.1f"), j);
		MemDc->TextOutW(KOORDGET(j, 0.01 * ymaxget), str);
	}

	for (int i = 0; i < Mass.size(); i++)
	{
		MemDc->SelectObject(GraphPen[i]);
		double x = MinX;
		MemDc->MoveTo(KOORDGET(MinX, Mass[i][0]));
		for (int j = 1; j < Mass[i].size(); j++)
		{
			if (gType == GrapType::Graphic) {
				MemDc->LineTo(KOORDGET(x, Mass[i][j]));
				x += step;
			}
			else if (gType == GrapType::Bits) {
				x += step;
				MemDc->LineTo(KOORDGET(x, Mass[i][j - 1]));
				// Вертикальные линии.
				if (Mass[i][j - 1] != Mass[i][j]) {
					MemDc->LineTo(KOORDGET(x, Mass[i][j]));
				}
				if (j == Mass[i].size() - 1) {
					x += step;
					MemDc->LineTo(KOORDGET(x, Mass[i][j]));
				}
			}
		}
	}
	WinDc->BitBlt(0, 0, WinPic.Width(), WinPic.Height(), MemDc, 0, 0, SRCCOPY);
	delete MemDc;
}

void DemodulatorQPSK::ReadDataFromFile(vector<double>& koef, char* way)
{
	string line;
	ifstream in(way);
	if (in.is_open())
	{
		while (getline(in, line))
		{
			koef.push_back(atof(line.c_str()));
		}
	}
}

BOOL DemodulatorQPSK::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	PicWndInputQPSK = GetDlgItem(IDC_Inputs_QPSK);
	PicDcInputQPSK = PicWndInputQPSK->GetDC();
	PicWndInputQPSK->GetClientRect(&PicInputQPSK);

	PicWndIsin = GetDlgItem(IDC_I_sin);
	PicDcIsin = PicWndIsin->GetDC();
	PicWndIsin->GetClientRect(&PicIsin);

	PicWndQcos = GetDlgItem(IDC_Q_cos);
	PicDcQcos = PicWndQcos->GetDC();
	PicWndQcos->GetClientRect(&PicQcos);

	PicWndSinc = GetDlgItem(IDC_SINC);
	PicDcSinc = PicWndSinc->GetDC();
	PicWndSinc->GetClientRect(&PicSinc);

	PicWndIfiltr = GetDlgItem(IDC_I_filtr);
	PicDcIfiltr = PicWndIfiltr->GetDC();
	PicWndIfiltr->GetClientRect(&PicIfiltr);

	PicWndQfiltr = GetDlgItem(IDC_Q_filtr);
	PicDcQfiltr = PicWndQfiltr->GetDC();
	PicWndQfiltr->GetClientRect(&PicQfiltr);

	PicWndQbit = GetDlgItem(IDC_Q_BITS);
	PicDcQbit = PicWndQbit->GetDC();
	PicWndQbit->GetClientRect(&PicQbit);

	PicWndIbit = GetDlgItem(IDC_I_BITS);
	PicDcIbit = PicWndIbit->GetDC();
	PicWndIbit->GetClientRect(&PicIbit);

	PicWndOut = GetDlgItem(IDC_output_BITS);
	PicDcOut = PicWndOut->GetDC();
	PicWndOut->GetClientRect(&PicOut);


	graphPen.CreatePen(PS_SOLID, 2, RGB(20, 220, 0));
	graphPens.push_back(&graphPen);
	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // Исключение: страница свойств OCX должна возвращать значение FALSE
}

void DemodulatorQPSK::OnBnClickedGetQpsk()
{
	vector<double> qpsk;
	ReadDataFromFile(qpsk, "QPSK.txt");

	vector<vector<double>> graphsSignal;
	graphsSignal.push_back(qpsk);

	GrapType type = GrapType::Graphic;
	DrawGraph(graphsSignal, 0, qpsk.size(), graphPens, PicDcInputQPSK, PicInputQPSK, type);
}


void DemodulatorQPSK::OnBnClickedGetIQ()
{
	vector<double> Isin;
	ReadDataFromFile(Isin, "I_sin.txt");

	vector<vector<double>> graphsIsin;
	graphsIsin.push_back(Isin);

	GrapType type = GrapType::Graphic;
	DrawGraph(graphsIsin, 0, Isin.size(), graphPens, PicDcIsin, PicIsin, type);

	vector<double> Qcos;
	ReadDataFromFile(Qcos, "Q_sin.txt");

	vector<vector<double>> graphsQcos;
	graphsQcos.push_back(Qcos);

	DrawGraph(graphsQcos, 0, Qcos.size(), graphPens, PicDcQcos, PicQcos, type);
}


void DemodulatorQPSK::OnBnClickedGetSinc()
{
	vector<double> sinc;
	ReadDataFromFile(sinc, "SINC.txt");

	vector<vector<double>> graphsSignal;
	graphsSignal.push_back(sinc);

	GrapType type = GrapType::Graphic;
	DrawGraph(graphsSignal, 0, sinc.size(), graphPens, PicDcSinc, PicSinc, type);
}


void DemodulatorQPSK::OnBnClickedGetfiltriq()
{
	vector<double> filtrI;
	ReadDataFromFile(filtrI, "I_filtr.txt");

	vector<vector<double>> graphsfiltrI;
	graphsfiltrI.push_back(filtrI);

	GrapType type = GrapType::Graphic;
	DrawGraph(graphsfiltrI, 0, filtrI.size(), graphPens, PicDcIfiltr, PicIfiltr, type);

	vector<double> filtrQ;
	ReadDataFromFile(filtrQ, "Q_filtr.txt");

	vector<vector<double>> graphsfiltrQ;
	graphsfiltrQ.push_back(filtrQ);

	DrawGraph(graphsfiltrQ, 0, filtrQ.size(), graphPens, PicDcQfiltr, PicQfiltr, type);
}


void DemodulatorQPSK::OnBnClickedGetbitIQ()
{
	vector<double> bitI;
	ReadDataFromFile(bitI, "I_bit.txt");

	vector<vector<double>> graphsbitI;
	graphsbitI.push_back(bitI);

	GrapType type = GrapType::Bits;
	DrawGraph(graphsbitI, 0, bitI.size(), graphPens, PicDcIbit, PicIbit, type);

	vector<double> bitQ;
	ReadDataFromFile(bitQ, "Q_bit.txt");

	vector<vector<double>> graphsbitQ;
	graphsbitQ.push_back(bitQ);

	DrawGraph(graphsbitQ, 0, bitQ.size(), graphPens, PicDcQbit, PicQbit, type);

	vector<double> out_bit_I;
	vector<double> out_bit_Q;

	for (int i = 0; i <= bitQ.size(); i += period * 2)
	{
		if (i != 0) {
			out_bit_I.push_back(bitI[i - period]);
			out_bit_Q.push_back(bitQ[i - period]);
		}
	}

	vector<vector<double>> out_graphsbitI;
	out_graphsbitI.push_back(out_bit_I);
	DrawGraph(out_graphsbitI, 0, out_bit_I.size(), graphPens, PicDcIbit, PicIbit, type);

	vector<vector<double>> out_graphsbitQ;
	out_graphsbitQ.push_back(out_bit_Q);
	DrawGraph(out_graphsbitQ, 0, out_bit_Q.size(), graphPens, PicDcQbit, PicQbit, type);

	vector<double> output;
	for (int i = 0; i < out_bit_I.size(); i++) {
		output.push_back(out_bit_I[i]);
		output.push_back(out_bit_Q[i]);
	}

	vector<vector<double>> out;
	out.push_back(output);
	DrawGraph(out, 0, output.size(), graphPens, PicDcOut, PicOut, type);
}
