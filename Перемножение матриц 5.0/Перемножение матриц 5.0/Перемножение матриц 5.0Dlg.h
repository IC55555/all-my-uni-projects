﻿
// Перемножение матриц 5.0Dlg.h: файл заголовка
//

#pragma once


// Диалоговое окно CПеремножениематриц50Dlg
class CПеремножениематриц50Dlg : public CDialogEx
{
// Создание
public:
	CПеремножениематриц50Dlg(CWnd* pParent = nullptr);	// стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ПЕРЕМНОЖЕНИЕМАТРИЦ50_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
