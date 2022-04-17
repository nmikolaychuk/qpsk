
// QPSKDlg.cpp: файл реализации
//

#include "pch.h"
#include "framework.h"
#include "QPSK.h"
#include "QPSKDlg.h"
#include "afxdialogex.h"
#include "AudioFile.h"
#include <vector>
#include <chrono>
//#include "../GenerateSignal/GenerateSignalDlg.h"
//#include "../GenerateSignal/GenerateSignal/GenerateSignalDlg.h"
using namespace std::chrono;	
//#include "../qpsk/decoder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define KOORDGET(x,y) (xpget*((x)-xminget)),(ypget*((y)-ymaxget)) 

// Диалоговое окно CQPSKDlg



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
	ON_BN_CLICKED(IDC_Read, &CQPSKDlg::OnBnClickedRead)
END_MESSAGE_MAP()


// Обработчики сообщений CQPSKDlg

BOOL CQPSKDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	PicWnd_Signal = GetDlgItem(IDC_QPSK);
	PicDc_Signal = PicWnd_Signal->GetDC();
	PicWnd_Signal->GetClientRect(&Pic_Signal);
	graf_pen.CreatePen(PS_SOLID, 2, RGB(20, 220, 0));
	graf_pen2.CreatePen(PS_SOLID, 2, RGB(220, 20, 0));

	PicWnd_Spectr = GetDlgItem(IDC_spectr);
	PicDc_Spectr = PicWnd_Spectr->GetDC();
	PicWnd_Spectr->GetClientRect(&Pic_Spectr);
	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

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

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CQPSKDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CQPSKDlg::DrawGraph(std::vector<std::vector<double>>& Mass, double MinX, double MaxX, std::vector<CPen*> GraphPen, CDC* WinDc, CRect WinPic)
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

	MemDc->SelectObject(&setka_pen);
	double shagX = (xmaxget - xminget) / 10;
	//отрисовка сетки по y
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

	MemDc->SelectObject(&osi_pen);
	//создаём Ось Y
	//MemDc->MoveTo(KOORDGET(0, ymaxget));
	//MemDc->LineTo(KOORDGET(0, yminget));
	//создаём Ось Х
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

bool CQPSKDlg::RandomBit(double low_chance)
{
	return (rand() < RAND_MAX * low_chance);
}

void CQPSKDlg::OnBnClickedRead()
{
	/*AudioFile<double> audioFile;
	audioFile.load("../GenerateSignal/audioFile.wav");
	int channel = 0;
	int numSamples = audioFile.getNumSamplesPerChannel();
	std::vector<double> Inputsignal;
	for (int i = 0; i < numSamples; i++)
	{
		Inputsignal.push_back(audioFile.samples[channel][i]);
	}*/

	double kolOtsh = 0;
	double freq = 128;
	double sampling_period = 1 / freq;
	double N = 1;
	vector<double> SQRT;
	vector<double> meandr;
	for (double i = kolOtsh; i < kolOtsh + N; i += sampling_period)
	{
		//CGenerateSignalDlg l;
		double separated_bit = RandomBit(0.5);
		meandr.push_back(separated_bit);
		/*double I = 5. * cos(0.5 * i);
		double Q = 5. * sin(0.5 * i);
		SQRT.push_back(I* 5. * cos(0.5 * i + 6.28)-Q* 5. * sin(0.5 * i + 6.28));*/
	}

	vector<double> I;
	vector<double> Q;
	I.resize(meandr.size());
	Q.resize(meandr.size());
	Q[0] = meandr[0];
	for (int i = 0; i < meandr.size()-2; i +=2)
	{
		I[i]= meandr[i];
		I[i + 1] = meandr[i];
		Q[i + 1] = meandr[i + 1];
		Q[i + 2] = meandr[i + 1];
	}
	int iter = 0;
	vector<double> II;
	vector<double> QQ;
	for (double i = kolOtsh; i < kolOtsh + N; i += sampling_period)
	{
		SQRT.push_back(I[iter]* 5. * cos(4 * i + 6.28)-Q[iter]* 5. * sin(4 * i + 6.28));
		II.push_back(I[iter] * 5. * cos(4 * i + 6.28));
		QQ.push_back(Q[iter] * 5. * sin(4 * i + 6.28));
		iter++;
	}
	vector<vector<double>>spectr2;
	/*spectr2.resize(3);
	spectr2[0] = meandr;
	spectr2[1] = I;
	spectr2[2] = Q;*/
	spectr2.resize(3);
	spectr2[0] = SQRT;
	spectr2[1] = II;
	spectr2[2] = QQ;
	GraphPen.clear();
	GraphPen.push_back(new CPen(PS_SOLID, 2, RGB(255, 0, 0)));
	GraphPen.push_back(new CPen(PS_SOLID, 2, RGB(50, 125, 50)));
	GraphPen.push_back(new CPen(PS_SOLID, 2, RGB(0, 0, 255)));
	DrawGraph(spectr2, kolOtsh, kolOtsh + N, GraphPen, PicDc_Signal, Pic_Signal);


	AudioFile<double> audioFile;
	AudioFile<double>::AudioBuffer buffer;
	buffer.resize(2);
	buffer[0].resize(N / sampling_period);
	buffer[1].resize(N / sampling_period);
	int numChannels = 2;
	int numSamplesPerChannel = 100000;
	float sampleRate = 44100.f;
	float frequency = 440.f;
	int iterr = 0;
	for (double i = kolOtsh; i < kolOtsh + N; i += sampling_period)
	{
		float sample = (float)spectr2[0][iterr];


		for (int channel = 0; channel < numChannels; channel++)
			buffer[channel][iterr] = sample * 0.5;
		iterr++;
	}

	bool ok = audioFile.setAudioBuffer(buffer);
	audioFile.save("audioFile.wav", AudioFileFormat::Wave);


	vector<complex<double>>* sp = new vector<complex<double>>;

	(*sp).resize(meandr.size());
	for (int i = 0; i < meandr.size(); i++)
	{
		(*sp)[i] = SQRT[i];
	}

	newFFT((*sp), -1);

	vector<double>spectr_long_signal;
	for (int i = 0; i < (*sp).size(); i++)
	{
		spectr_long_signal.push_back(abs((*sp)[i]));
	}
	vector<double>buf;
	for (int i = spectr_long_signal.size()/2; i < spectr_long_signal.size(); i++)
	{
		buf.push_back(spectr_long_signal[i]);
	}
	for (int i = 0; i < spectr_long_signal.size() / 2; i++)
	{
		buf.push_back(spectr_long_signal[i]);
	}

	vector<vector<double>>spectr;
	spectr.resize(1);
	spectr[0] = buf;
	GraphPen.clear();
	GraphPen.push_back(new CPen(PS_SOLID, 2, RGB(255, 0, 0)));
	GraphPen.push_back(new CPen(PS_SOLID, 2, RGB(50, 125, 50)));
	GraphPen.push_back(new CPen(PS_SOLID, 2, RGB(0, 0, 255)));
	DrawGraph(spectr, 0, (*sp).size(), GraphPen, PicDc_Spectr, Pic_Spectr);
}
