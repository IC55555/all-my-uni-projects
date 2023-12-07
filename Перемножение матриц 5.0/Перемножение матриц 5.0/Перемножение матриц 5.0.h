
// Перемножение матриц 5.0.h: главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CПеремножениематриц50App:
// Сведения о реализации этого класса: Перемножение матриц 5.0.cpp
//

class CПеремножениематриц50App : public CWinApp
{
public:
	CПеремножениематриц50App();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CПеремножениематриц50App theApp;
