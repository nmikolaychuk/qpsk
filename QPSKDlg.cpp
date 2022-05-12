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
	ON_BN_CLICKED(IDC_I_Q_from_QPSK, &CQPSKDlg::OnBnClickedIQfromQpsk)
	ON_BN_CLICKED(IDC_GET_WINDOW, &CQPSKDlg::OnBnClickedGetWindow)
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
	graphPens.push_back(&graphPen);
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


void CQPSKDlg::DrawGraph(vector<vector<double>>& Mass, double MinX,
	double MaxX, vector<CPen*> GraphPen,
	CDC* WinDc, CRect WinPic, GraphType gType)
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

	if (gType == GraphType::Bits) yminget = -ymaxget * 0.1;
	else if (gType == GraphType::Graphic) yminget = -ymaxget;

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
			if (gType == GraphType::Graphic) {
				MemDc->LineTo(KOORDGET(x, Mass[i][j]));
				x += step;
			}
			else if (gType == GraphType::Bits) {
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


vector<double> GetInputBits(int length)
{
	vector<double> bits;
	if (length > 0) {
		for (int i = 0; i < length; i++) {
			bits.push_back(double(rand() < RAND_MAX * 0.5));
		}
	}
	return bits;
}


void CQPSKDlg::OnBnClickedGetInputBits()
{
	// Генерация входной последовательности.
	srand(time(NULL));
	inputBits = GetInputBits(countOfBits);
	vector<vector<double>> graphs;
	graphs.push_back(inputBits);

	GraphType type = GraphType::Bits;
	DrawGraph(graphs, 0, inputBits.size(), graphPens, PicDcBits, PicBits, type);
}


vector<double> CQPSKDlg::GetGeneralSignal(double ampl, double start_phase, double sampling_freq, int duration) {
	vector<double> signal;
	int length_of_signal = round(sampling_freq * (double)duration) + syncTime;
	// Частота сигнала, Гц.
	for (int t = syncTime; t < length_of_signal; ++t) {
		// Вычисление частоты.
		double w = 2. * M_PI * freq;
		signal.push_back(ampl * sin(w * t / sampling_freq + start_phase));
	}
	syncTime = length_of_signal - 1;
	return signal;
}


void CQPSKDlg::OnBnClickedGetGeneralSign()
{ 
	vector<double> generalSignal = GetGeneralSignal(ampl, startPhase, sampleRate, duration);
	vector<vector<double>> graphs;
	graphs.push_back(generalSignal);

	GraphType type = GraphType::Graphic;
	DrawGraph(graphs, 0, generalSignal.size(), graphPens, PicDcGeneral, PicGeneral, type);
}


void CQPSKDlg::OnBnClickedGetIq()
{
	if (!inputBits.empty()) {
		iComp.clear();
		qComp.clear();
		for (int i = 0; i < inputBits.size(); i++) {
			if (i % 2 == 0) {
				iComp.push_back(inputBits[i]);
			}
			else {
				qComp.push_back(inputBits[i]);
			}
		}

		//for (int i = 0; i <= inputBits.size() - 2; i += 2)
		//{
		//	iComp[i] = inputBits[i];
		//	iComp[i + 1] = inputBits[i];
		//	qComp[i] = inputBits[i + 1];
		//	qComp[i + 1] = inputBits[i + 1];
		//}

		vector<vector<double>> iGraphs;
		iGraphs.push_back(iComp);

		vector<vector<double>> qGraphs;
		qGraphs.push_back(qComp);

		GraphType type = GraphType::Bits;
		DrawGraph(iGraphs, 0, iComp.size(), graphPens, PicDcIComp, PicIComp, type);
		DrawGraph(qGraphs, 0, qComp.size(), graphPens, PicDcQComp, PicQComp, type);
	}
}

void CQPSKDlg::OnBnClickedGetQpsk()
{
	if (!iComp.empty() && !qComp.empty()) {
		double length = sampleRate * duration;
		double iq_step = length / ((double)countOfBits / 2.);
		qpskSignal.resize(length);
		for (int i = 0; i < length; i++) {
			int index = (int)((double)i / iq_step);
			double w = 2. * M_PI * freq;
			double bufI = 0, bufQ = 0;
			if (iComp[index] == 0) bufI = -1.;
			else bufI = 1.;
			if (qComp[index] == 0) bufQ = -1.;
			else bufQ = 1.;
			qpskSignal[i] = bufI * cos(w * i / sampleRate + startPhase) +
				bufQ * sin(w * i / sampleRate + startPhase);
		}

		vector<vector<double>> graphsSignal;
		graphsSignal.push_back(qpskSignal);

		GraphType type = GraphType::Graphic;
		DrawGraph(graphsSignal, 0, qpskSignal.size(), graphPens, PicDcQPSK, PicQPSK, type);

		// Спектр.
		vector<complex<double>> qpskSpectrum;
		for (int i = 0; i < length; i++) {
			qpskSpectrum.push_back(qpskSignal[i]);
		}
		
		newFFT(qpskSpectrum, -1);
		vector<vector<double>> graphsSpec;
		graphsSpec.resize(1);
		for (int i = 0; i < qpskSpectrum.size(); i++) {
			graphsSpec[0].push_back(abs(qpskSpectrum[i]));
		}

		DrawGraph(graphsSpec, 0, graphsSpec.size(), graphPens, PicDcQPSKSpec, PicQPSKSpec, type);
	}
}

vector<double> ffiltr_comp;
void CQPSKDlg::ConvolutionHS(vector<double> signal, vector<double>& convolution)
{
	for (int n = 0; n < signal.size(); n++)
	{
		double counter = 0.;
		for (int m = 0; m < ffiltr_comp.size(); m++)
		{
			if ((-m + n) >= 0)
				counter += signal[-m + n] * ffiltr_comp[m];
			else
				continue;
		}
		convolution.push_back(counter);
	}
}

void CQPSKDlg::OnBnClickedIQfromQpsk()
{
	if (!qpskSignal.empty())
	{
		double length = sampleRate * duration;
		vector<double> outI(length);
		vector<double> outQ(length);
		double w = 2. * M_PI * freq;
		ofstream foutI("I_sin.txt");
		ofstream foutQ("Q_sin.txt");
		ofstream fQPSK("QPSK.txt");
		for (int i = 0; i < length; i++)
		{
			outI[i] = cos(w * i / sampleRate + startPhase) * qpskSignal[i];
			foutI << outI[i] <<"\n";
			outQ[i]= sin(w * i / sampleRate + startPhase) * qpskSignal[i];
			foutQ<< outQ[i] << "\n";
			fQPSK << qpskSignal[i] << "\n";
		}
		foutI.close(); // закрываем файл
		foutQ.close(); // закрываем файл
		fQPSK.close(); // закрываем файл
		ffiltr_comp.resize(length);
		ofstream fSINC("SINC.txt");
		for (double i = 0; i < length; i++)
		{
			if (i == 0) ffiltr_comp[i] = 1;
			else ffiltr_comp[i] = sin(0.3 * i) / (0.3 * i);
			fSINC<< ffiltr_comp[i]<< "\n";
		}
		fSINC.close(); // закрываем файл
		// Свертка с фильтром нижних частот.
		vector<double> convI;
		vector<double> convQ;
		ConvolutionHS(outI, convI);
		ConvolutionHS(outQ, convQ);
		ofstream filtr_I("I_filtr.txt");
		ofstream filtr_Q("Q_filtr.txt");
		for (int i = 0; i < convI.size(); i++)
		{
			filtr_I << convI[i]<< "\n";
			filtr_Q << convQ[i] << "\n";
		}
		filtr_I.close(); // закрываем файл
		filtr_Q.close();
		vector<double> bitI(length);
		vector<double> bitQ(length);
		ofstream bit_I("I_bit.txt");
		ofstream bit_Q("Q_bit.txt");
		for (int i = 0; i < convI.size(); i++)
		{
			if (convI[i] < 0)bitI[i] = 0;
			else bitI[i] = 1;
			if (convQ[i] < 0)bitQ[i] = 0;
			else bitQ[i] = 1;
			bit_I<< bitI[i]<< "\n";
			bit_Q << bitQ[i] << "\n";
		}
		bit_I.close(); // закрываем файл
		bit_Q.close();
		int x = 0;
	}
}


void CQPSKDlg::OnBnClickedGetWindow()
{
	OnBnClickedIQfromQpsk();
	if (pGraphDialog == NULL)
	{
		pGraphDialog = new DemodulatorQPSK;
		pGraphDialog->Create(IDD_DIALOG1);
		pGraphDialog->ShowWindow(SW_SHOW);
	}
	pGraphDialog->ShowWindow(SW_SHOW);
}
