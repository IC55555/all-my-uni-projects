
// DlgProxy.h: файл заголовка
//

#pragma once

class CПеремножениематрицы40Dlg;


// Целевой объект команды CПеремножениематрицы40DlgAutoProxy

class CПеремножениематрицы40DlgAutoProxy : public CCmdTarget
{
	DECLARE_DYNCREATE(CПеремножениематрицы40DlgAutoProxy)

	CПеремножениематрицы40DlgAutoProxy();           // защищенный конструктор, используемый при динамическом создании

// Атрибуты
public:
	CПеремножениематрицы40Dlg* m_pDialog;

// Операции
public:

// Переопределение
	public:
	virtual void OnFinalRelease();

// Реализация
protected:
	virtual ~CПеремножениематрицы40DlgAutoProxy();

	// Созданные функции схемы сообщений

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CПеремножениематрицы40DlgAutoProxy)

	// Автоматически созданные функции диспетчерской карты OLE

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

