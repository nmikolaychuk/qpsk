#pragma once
#define _USE_MATH_DEFINES
#include <vector>
#include <complex>
#include <fstream>
#include <math.h>
#include "DemodulatorQPSK.h"
using namespace std;

enum class GraphType
{
	Bits = 0,
	Graphic = 1
};

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
protected:
	HICON m_hIcon;
	
	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// Коэффициенты масштабирования.
	double xminget, xmaxget,
		yminget, ymaxget,
		xpget, ypget;

	// Параметры.
	double ampl = 1.;
	double startPhase = 0.;
	double sampleRate = 512.;
	double freq = 20e3;
	double duration = 1.;
	int countOfBits = 16;
	double syncTime;
	vector<double> inputBits;
	vector<double> iComp;
	vector<double> qComp;
	vector<double> qpskSignal;

	// Области рисования.
	CWnd* PicWndBits;
	CDC* PicDcBits;
	CRect PicBits;

	CWnd* PicWndGeneral;
	CDC* PicDcGeneral;
	CRect PicGeneral;

	CWnd* PicWndIComp;
	CDC* PicDcIComp;
	CRect PicIComp;

	CWnd* PicWndQComp;
	CDC* PicDcQComp;
	CRect PicQComp;

	CWnd* PicWndQPSK;
	CDC* PicDcQPSK;
	CRect PicQPSK;

	CWnd* PicWndQPSKSpec;
	CDC* PicDcQPSKSpec;
	CRect PicQPSKSpec;
	
	// Ручки.
	std::vector<CPen*> graphPens;
	CPen axesPen;
	CPen gridPen;
	CPen graphPen;

	// Функции.
	void DrawGraph(std::vector<std::vector<double>>& Mass,
		double MinX, double MaxX, std::vector<CPen*> GraphPen,
		CDC* WinDc, CRect WinPic, GraphType type);
	vector<double> GetGeneralSignal(double ampl, double start_phase,
		double sampling_freq, int duration);
	void ConvolutionHS(vector<double> signal, vector<double>& convolution);

	// Обработчики.
	afx_msg void OnBnClickedGetInputBits();
	afx_msg void OnBnClickedGetGeneralSign();
	afx_msg void OnBnClickedGetIq();
	afx_msg void OnBnClickedGetQpsk();
	afx_msg void OnBnClickedIQfromQpsk();
	afx_msg void OnBnClickedGetWindow();


	DemodulatorQPSK* pGraphDialog = nullptr;
};
