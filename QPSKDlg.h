
// QPSKDlg.h: файл заголовка
//

#pragma once
#include <vector>
#include <complex>
using namespace std;

// Диалоговое окно CQPSKDlg
class CQPSKDlg : public CDialogEx
{
// Создание
public:
	CQPSKDlg(CWnd* pParent = nullptr);	// стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_QPSK_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRead();
	typedef vector < complex < float > > iq_signal;
	void GenerateIQ(iq_signal& buffer);
	double Bitrate = 1e-3;
	double sampling_rate = 1024;
	bool RandomBit(double low_chance);
	struct SignalGenerationParameters
	{
		double start_timestamp;
		double start_phase;
		double duration;

		double n_samples;
		double sampling_frequency;
		double bitrate;
		double additional_parameter;
	};
	struct Signal
	{
		vector<complex<double>> signal;
		vector<double> keys;

		string name;
		string description;

		double sampling;
		double timestamp;
		double duration;
	};
	void GenerateQPSKSignal(SignalGenerationParameters params, Signal& in_signal, Signal& ret_signal);
	void DrawGraph(std::vector<std::vector<double>>& Mass, double MinX, double MaxX, std::vector<CPen*> GraphPen, CDC* WinDc, CRect WinPic);

	double xminget, xmaxget,
		yminget, ymaxget,
		xpget, ypget;

	CWnd* PicWnd_Signal;
	CDC* PicDc_Signal;
	CRect Pic_Signal;
	std::vector<CPen*> GraphPen;

	CPen osi_pen;		// для осей 
	CPen setka_pen;		// для сетки
	CPen graf_pen;		// для графика функции
	CPen graf_pen2;
	CPen graf_pen3;
	//vector<doube.
	CWnd* PicWnd_Spectr;
	CDC* PicDc_Spectr;
	CRect Pic_Spectr;
};
