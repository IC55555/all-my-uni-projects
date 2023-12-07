
// Перемножение матрицы 4.0Dlg.h: файл заголовка
//

#pragma once

class CПеремножениематрицы40DlgAutoProxy;


// Диалоговое окно CПеремножениематрицы40Dlg
class CПеремножениематрицы40Dlg : public CDialogEx
{
	DECLARE_DYNAMIC(CПеремножениематрицы40Dlg);
	friend class CПеремножениематрицы40DlgAutoProxy;

// Создание
public:
	CПеремножениематрицы40Dlg(CWnd* pParent = nullptr);	// стандартный конструктор
	virtual ~CПеремножениематрицы40Dlg();

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ПЕРЕМНОЖЕНИЕМАТРИЦЫ40_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	CПеремножениематрицы40DlgAutoProxy* m_pAutoProxy;
	HICON m_hIcon;

	BOOL CanExit();

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
};
