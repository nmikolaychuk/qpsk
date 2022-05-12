#pragma once
#define _USE_MATH_DEFINES
#include <vector>
#include <math.h>
#include <string>
#include <fstream>

using namespace std;

enum class GrapType
{
	Bits = 0,
	Graphic = 1
};

// Диалоговое окно DemodulatorQPSK

class DemodulatorQPSK : public CDialogEx
{
	DECLARE_DYNAMIC(DemodulatorQPSK)

public:
	DemodulatorQPSK(CWnd* pParent = nullptr);   // стандартный конструктор
	virtual ~DemodulatorQPSK();

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	
	// Коэффициенты масштабирования.
	double xminget, xmaxget,
		yminget, ymaxget,
		xpget, ypget;

	// Ручки.
	std::vector<CPen*> graphPens;
	CPen axesPen;
	CPen gridPen;
	CPen graphPen;

	// Параметры.
	double ampl = 1.;
	double startPhase = M_PI;
	double sampleRate = 512.;
	double freq = 5.5;
	double duration = 1.;
	int countOfBits = 16;

	// Области рисования.
	CWnd* PicWndInputQPSK;
	CDC* PicDcInputQPSK;
	CRect PicInputQPSK;

	CWnd* PicWndIsin;
	CDC* PicDcIsin;
	CRect PicIsin;

	CWnd* PicWndQcos;
	CDC* PicDcQcos;
	CRect PicQcos;

	CWnd* PicWndSinc;
	CDC* PicDcSinc;
	CRect PicSinc;

	CWnd* PicWndIfiltr;
	CDC* PicDcIfiltr;
	CRect PicIfiltr;

	CWnd* PicWndQfiltr;
	CDC* PicDcQfiltr;
	CRect PicQfiltr;

	CWnd* PicWndQbit;
	CDC* PicDcQbit;
	CRect PicQbit;

	CWnd* PicWndIbit;
	CDC* PicDcIbit;
	CRect PicIbit;

	CWnd* PicWndOut;
	CDC* PicDcOut;
	CRect PicOut;

	// Длительность символа.
	int period = 32;

	// Функции.
	void DrawGraph(std::vector<std::vector<double>>& Mass,
		double MinX, double MaxX, std::vector<CPen*> GraphPen,
		CDC* WinDc, CRect WinPic, GrapType type);
	void ReadDataFromFile(vector<double>& koef, char* way);

	// Обработчики.
	afx_msg void OnBnClickedGetQpsk();

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedGetIQ();
	afx_msg void OnBnClickedGetSinc();
	afx_msg void OnBnClickedGetfiltriq();
	afx_msg void OnBnClickedGetbitIQ();
};
