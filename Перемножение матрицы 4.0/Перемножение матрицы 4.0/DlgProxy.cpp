
// DlgProxy.cpp: файл реализации
//

#include "pch.h"
#include "framework.h"
#include "Перемножение матрицы 4.0.h"
#include "DlgProxy.h"
#include "Перемножение матрицы 4.0Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CПеремножениематрицы40DlgAutoProxy

IMPLEMENT_DYNCREATE(CПеремножениематрицы40DlgAutoProxy, CCmdTarget)

CПеремножениематрицы40DlgAutoProxy::CПеремножениематрицы40DlgAutoProxy()
{
	EnableAutomation();

	// Чтобы приложение работало, пока объект
	//	автоматизации активен, конструктор вызывает AfxOleLockApp.
	AfxOleLockApp();

	// Получает доступ к диалоговому окну через указатель
	//  главного окна приложения.  Задает внутренний указатель прокси-сервера для
	//  доступа к диалогу, а также задает обратный указатель диалога на этот
	//  прокси-сервер.
	ASSERT_VALID(AfxGetApp()->m_pMainWnd);
	if (AfxGetApp()->m_pMainWnd)
	{
		ASSERT_KINDOF(CПеремножениематрицы40Dlg, AfxGetApp()->m_pMainWnd);
		if (AfxGetApp()->m_pMainWnd->IsKindOf(RUNTIME_CLASS(CПеремножениематрицы40Dlg)))
		{
			m_pDialog = reinterpret_cast<CПеремножениематрицы40Dlg*>(AfxGetApp()->m_pMainWnd);
			m_pDialog->m_pAutoProxy = this;
		}
	}
}

CПеремножениематрицы40DlgAutoProxy::~CПеремножениематрицы40DlgAutoProxy()
{
	// Чтобы прервать работу приложения, когда все объекты создаются
	// 	автоматически, деструктор вызывает AfxOleUnlockApp.
	//  Среди прочего будет уничтожено главное диалоговое окно
	if (m_pDialog != nullptr)
		m_pDialog->m_pAutoProxy = nullptr;
	AfxOleUnlockApp();
}

void CПеремножениематрицы40DlgAutoProxy::OnFinalRelease()
{
	// Когда будет освобождена последняя ссылка на объект автоматизации,
	// Будет вызван OnFinalRelease.  Базовый класс автоматически
	// удалит объект.  Перед вызовом базового класса требуется
	// дополнительная очистка объекта.

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CПеремножениематрицы40DlgAutoProxy, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CПеремножениематрицы40DlgAutoProxy, CCmdTarget)
END_DISPATCH_MAP()

// Примечание. Мы добавили поддержку для IID_IПеремножениематрицы40, чтобы обеспечить безопасную с точки зрения типов привязку
//  из VBA.  Этот IID должен соответствовать GUID, связанному с
//  disp-интерфейс в файле .IDL.

// {682ddcdc-26fa-4ab4-86d9-c946423995c1}
static const IID IID_IПеремножениематрицы40 =
{0x682ddcdc,0x26fa,0x4ab4,{0x86,0xd9,0xc9,0x46,0x42,0x39,0x95,0xc1}};

BEGIN_INTERFACE_MAP(CПеремножениематрицы40DlgAutoProxy, CCmdTarget)
	INTERFACE_PART(CПеремножениематрицы40DlgAutoProxy, IID_IПеремножениематрицы40, Dispatch)
END_INTERFACE_MAP()

// Макрос IMPLEMENT_OLECREATE2 определен в pch.h этого проекта
// {7ebe73ec-1c3b-4cc1-9904-3fa0b76e6dd9}
IMPLEMENT_OLECREATE2(CПеремножениематрицы40DlgAutoProxy, "Перемножениематрицы40.Application", 0x7ebe73ec,0x1c3b,0x4cc1,0x99,0x04,0x3f,0xa0,0xb7,0x6e,0x6d,0xd9)


// Обработчики сообщений CПеремножениематрицы40DlgAutoProxy
