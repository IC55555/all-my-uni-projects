
// Перемножение матрицы 4.0.h: главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CПеремножениематрицы40App:
// Сведения о реализации этого класса: Перемножение матрицы 4.0.cpp
//

class CПеремножениематрицы40App : public CWinApp
{
public:
	CПеремножениематрицы40App();

// Переопределение
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CПеремножениематрицы40App theApp;
