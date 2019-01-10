// teah_win32_important message.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "teah_win32_important message.h"

#define MAX_LOADSTRING 100

// ȫ�ֱ���: 
HINSTANCE hInst;                                // ��ǰʵ��
WCHAR szTitle[MAX_LOADSTRING];                  // �������ı�
WCHAR szWindowClass[MAX_LOADSTRING];            // ����������

// �˴���ģ���а����ĺ�����ǰ������: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: �ڴ˷��ô��롣

    // ��ʼ��ȫ���ַ���
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TEAH_WIN32_IMPORTANTMESSAGE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ִ��Ӧ�ó����ʼ��: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TEAH_WIN32_IMPORTANTMESSAGE));

    MSG msg;

    // ����Ϣѭ��: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))//������Ϣ
        {
            TranslateMessage(&msg);//������Ϣ ��Ҫ��Լ�����Ϣ ��ϼ�
            DispatchMessage(&msg);//Ͷ����Ϣ
        }
    }

    return (int) msg.wParam;//���յ�WM_EXIT�����Ϣʱ ��Ӧ��msg.wParamΪ0 �����൱�ڷ���0 ��ʾ��������
}



//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW|CS_DBLCLKS;//��Ҫ����CS_DBCLKS������Ӧ˫���Ĳ���
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TEAH_WIN32_IMPORTANTMESSAGE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TEAH_WIN32_IMPORTANTMESSAGE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
HWND g_hWnd;//����һ��ȫ�ֵı���
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   g_hWnd = hWnd;
   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND  - ����Ӧ�ó���˵�
//  WM_PAINT    - ����������
//  WM_DESTROY  - �����˳���Ϣ������
//
//

void CallBackFun()
{
	static int j = 0;
	HDC hdc = GetDC(g_hWnd);//��Ҫ����һ��ȫ�ֱ���������
	TextOut(hdc, 400, j, _T("time_3"), 6);
	ReleaseDC(g_hWnd, hdc);
	j += 20;
}
bool isDown = false;//Ϊ���γ���קЧ��
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
	{
		SetTimer(hWnd,
			//���ü�ʱ��
			1001,//��ʱ��ID��,ID��������������ʱ��
			1000,//��1s��һ��
			nullptr);//��ʱ���ص�����
		SetTimer(hWnd, 1002, 1000, nullptr);//���ü�ʱ��2
		SetTimer(hWnd, 1003, 2000, (TIMERPROC)CallBackFun);//���ü�ʱ�����������ûص�����(TIMERPROC)CallBackFun

		/*
		SetTimer(
    _In_opt_ HWND hWnd,
    _In_ UINT_PTR nIDEvent,
    _In_ UINT uElapse,
    _In_opt_ TIMERPROC lpTimerFunc);
		*/
	}
	break;
		//1.ϵͳ������Ϣ ��󻯡���С�����ر�
	case WM_SYSCOMMAND:
		//����ϵͳ��Ϣ,��Ϊ���û��case WM_SYSCOMMAND ϵͳ��������Ϣ�ͻ��DefWindowProc(hWnd, message, wParam, lParam);��
		//��������ϵͳ��Ϣ�Ժ� ��ק���ƶ����رմ��ڶ�ʧЧ�� ֻ��ͨ�����������ȥ��������
	{
		switch (wParam)
		{
		case SC_CLOSE://���ڹر�
			MessageBox(0, 0, 0, 0);
			break;
		}
	}
	DefWindowProc(hWnd, message, wParam, lParam);//�����Ϳ�����ȷ������ϵͳ������Ϣ
	break;
	//5.�������Ϣ
	case WM_LBUTTONDOWN://�������
	{
		//MessageBox(0, _T("�������"), 0, 0);
		int i = wParam;//�����������lParam 1  ��¼��ʱ��갴��shift��ctrl ��״̬
		int j = lParam;//�����������lParam 12976506 �������� lparam��һ��long����32λ������4�ֽ�
		/*
		��ˣ�ʹ��2���ֽ�������x���꣬�����ֽ�������y����
		*/
		int x = LOWORD(lParam);//#define LOWORD(l) ((WORD)(((DWORD_PTR)(l)) & 0xffff)) ���ȡ��λ���ֽ�
		int y = HIWORD(lParam);//#define HIWORD(l) ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff)) ��ʾȡ��λ���ֽ�

		isDown = true;//��קЧ��
		break;
	}
	
	case WM_LBUTTONUP://���̧�� �ܶఴť������Ӧ��ʱ����̧�����������رմ���
	{
		isDown = false;
		KillTimer(hWnd, 1001);
	}
		break;
	case WM_RBUTTONDBLCLK://�Ҽ�˫��  ��ע�ᴰ���� ��Ҫ����CS_DBCLKS������Ӧ˫���Ĳ���
		MessageBox(0, _T("�Ҽ�˫��"), 0, 0);
		break;
		//6.����ƶ���Ϣ
	case WM_MOUSEMOVE:
	{
		//Ч��Ϊ�����������ƶ����ڴ����ڴ�(0,0)�㵽��ǰ���λ�û���
		if (isDown == true)
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			HDC hdc = GetDC(hWnd);
			LineTo(hdc, x, y);
			ReleaseDC(hWnd, hdc);
		}
	}
	break;
	//7.��������Ϣ
	case WM_MOUSEHWHEEL://�����ֹ���
	{
		//��lparam�����¼x��y ��ֵ����wparam�����¼���ֳ�ǰ�����ǳ����
		//int x = LOWORD(wParam);
		int y = HIWORD(wParam);//��wparam�����¼���ֳ�ǰ�����ǳ���� �����ڸ����ֽ���
		//1�Ļ����� 0000 0001 -1�Ļ����� 1111 1111 �����������Ϊ0  ���Ի��������ǲ������
	}
	break;
	//8.��ʱ����Ϣ
	/*
	a.��Ҫ�ڹ�����Ϣ��WM_CREAT���ü�ʱ��SetTimer
	b.ͨ��WM_TIMER�õ���ʱ����Ϣ Ҳ������SetTimer(hWnd, 1003, 2000, (TIMERPROC)CallBackFun);���ûص����������е���
	c.�����ʱ��������Ҳ����ͨ��killtimer���ͷ�
	*/
	
	case WM_TIMER:
	{
		switch (wParam)//wParam�����ʱ����ID��
		{
		case 1001:
		{
			static int j = 0;
			HDC hdc = GetDC(hWnd);
			TextOut(hdc, 0, j, _T("time_1"), 6);
			ReleaseDC(hWnd, hdc);
			j += 20;
		}
		break;
		case 1002:
		{
			static int j = 0;
			HDC hdc = GetDC(hWnd);
			TextOut(hdc, 200, j, _T("time_2"), 6);
			ReleaseDC(hWnd, hdc);
			j += 20;
		}
		break;

		}
	}
	break;

	//4.ϵͳ���� A.alt/f10 B.alt��ϼ�
	case WM_SYSKEYDOWN:
		switch (wParam)
		{
		case VK_F10:
			MessageBox(0, _T("F10"), 0, 0);
			break;
		case 'X'://��Ҫʹ��ALT+X��ϼ� ����case VK_MENU ���ܴ���
			MessageBox(0, _T("ALT+X"), 0, 0);
			break;
		//case VK_MENU://ALT��
		//	MessageBox(0, _T("ALT"), 0, 0);
		//	break;
		}
	//3.�ַ���Ϣ���������д�ַ� Сд�ַ�
	case WM_CHAR://��TranslateMessage()���밴����Ϣ���õ��ַ���Ϣ 
	{
		switch (wParam)
		{
		case'!'://WM_CHAR֧����ϼ�����  ����˵ȭ�ʵĳ�����Ҫ��ϼ�(����һ����ʱ�� �ڹ涨ʱ����������������γ���ϼ� ����map/hash��ȥƥ����б�)
			MessageBox(0, _T("!"), 0, 0);
			break;
		case 'b':
		{
			HDC hdc = GetDC(hWnd);//��ʾ���ڴ��н��л���
			TextOut(hdc, 200, 0, TEXT("WM_CHAR b"), _tcslen(TEXT("WM_CHAR b")));
			ReleaseDC(hWnd, hdc);//����ڴ����
		}
		break;
		case'B'://1.B�����  2.��ϼ�shitf+bҲ�����
		{
			HDC hdc = GetDC(hWnd);//��ʾ���ڴ��н��л���
			TextOut(hdc, 400, 0, TEXT("WM_CHAR B"), 9);
			ReleaseDC(hWnd, hdc);//����ڴ����
		}
		break;
		}
	}
	break;
	//2.������Ϣ  ��ASCII���ʹ��ASCII�룬 û�еļ�VK_ ʹ�������ֵ ����VK_TAB
	case WM_KEYDOWN://���̰���
	{
		switch (wParam)//������Ѱ�����ֵ�ķ��� 1.��wParam���¶ϵ� 2.����һ��������VK_UPȻ��鿴wParam��ֵ3.ת����ʮ������0x..4.���ֵ��
		{
		case 'A'://WM_KEYDOWN����ʶ��Сд��ֻ��ʶ���д ������WM_KEYDOWN��Ϣ��Ӧ����ֵ����Ϊ��д
			MessageBox(0, _T("A"), 0, 0);
			break;
		case 'B'://��B���µ�ʱ��WM_CHAR WM_KEYDOWN����ƥ��
		{
			HDC hdc = GetDC(hWnd);//��ʾ���ڴ��н��л���
			TextOut(hdc, 0, 0, TEXT("WM_KEYDOWN B"), 12);
			ReleaseDC(hWnd, hdc);//����ڴ����
		}
			break;
		case VK_TAB:
			MessageBox(0, _T("Tab"), 0, 0);
			break;
		}
	}
	break;
	case WM_KEYUP://����̧��
	{
		switch (wParam)
		{
		case 'C':
			MessageBox(0, _T("A"), 0, 0);
			break;
		}
	}
	break;
    case WM_COMMAND://������Ϣ/�˵���Ϣ
        {
            int wmId = LOWORD(wParam);
            // �����˵�ѡ��: 
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: �ڴ˴����ʹ�� hdc ���κλ�ͼ����...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);//���ڵ�ϵͳ��Ϣ������ ������� ��С�� �ƶ����� �رմ���
    }
    return 0;
}

// �����ڡ������Ϣ�������
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
