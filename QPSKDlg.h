#pragma once
#include <vector>
#include <complex>
using namespace std;

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
		CDC* WinDc, CRect WinPic);

	// Обработчики.
	afx_msg void OnBnClickedGetInputBits();
	afx_msg void OnBnClickedGetGeneralSign();
	afx_msg void OnBnClickedGetIq();
	afx_msg void OnBnClickedGetQpsk();
};
