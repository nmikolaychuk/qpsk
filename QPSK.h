
// QPSK.h: главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CQPSKApp:
// Сведения о реализации этого класса: QPSK.cpp
//

class CQPSKApp : public CWinApp
{
public:
	CQPSKApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CQPSKApp theApp;
