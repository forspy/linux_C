// teah_win32_important message.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "teah_win32_important message.h"

#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明: 
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

    // TODO: 在此放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TEAH_WIN32_IMPORTANTMESSAGE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TEAH_WIN32_IMPORTANTMESSAGE));

    MSG msg;

    // 主消息循环: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))//接收消息
        {
            TranslateMessage(&msg);//翻译消息 主要针对键盘消息 组合键
            DispatchMessage(&msg);//投递消息
        }
    }

    return (int) msg.wParam;//当收到WM_EXIT这个消息时 对应的msg.wParam为0 所以相当于返回0 表示正常结束
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW|CS_DBLCLKS;//需要加上CS_DBCLKS才能响应双击的操作
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
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
HWND g_hWnd;//定义一个全局的变量
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

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
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//

void CallBackFun()
{
	static int j = 0;
	HDC hdc = GetDC(g_hWnd);//需要建立一个全局变量来接受
	TextOut(hdc, 400, j, _T("time_3"), 6);
	ReleaseDC(g_hWnd, hdc);
	j += 20;
}
bool isDown = false;//为了形成拖拽效果
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
	{
		SetTimer(hWnd,
			//设置计时器
			1001,//计时器ID号,ID号用来区别多个计时器
			1000,//隔1s放一次
			nullptr);//计时器回调函数
		SetTimer(hWnd, 1002, 1000, nullptr);//设置计时器2
		SetTimer(hWnd, 1003, 2000, (TIMERPROC)CallBackFun);//设置计时器三，并设置回调函数(TIMERPROC)CallBackFun

		/*
		SetTimer(
    _In_opt_ HWND hWnd,
    _In_ UINT_PTR nIDEvent,
    _In_ UINT uElapse,
    _In_opt_ TIMERPROC lpTimerFunc);
		*/
	}
	break;
		//1.系统命令消息 最大化、最小化、关闭
	case WM_SYSCOMMAND:
		//拦截系统消息,因为如果没有case WM_SYSCOMMAND 系统的命令消息就会从DefWindowProc(hWnd, message, wParam, lParam);走
		//而拦截了系统消息以后 拖拽、移动、关闭窗口都失效了 只能通过任务管理器去结束窗口
	{
		switch (wParam)
		{
		case SC_CLOSE://窗口关闭
			MessageBox(0, 0, 0, 0);
			break;
		}
	}
	DefWindowProc(hWnd, message, wParam, lParam);//这样就可以正确的引流系统命令消息
	break;
	//5.鼠标点击消息
	case WM_LBUTTONDOWN://左键单击
	{
		//MessageBox(0, _T("左键单击"), 0, 0);
		int i = wParam;//比如左键单击lParam 1  记录的时鼠标按键shift和ctrl 的状态
		int j = lParam;//比如左键单击lParam 12976506 鼠标的坐标 lparam是一个long类型32位里面是4字节
		/*
		因此，使用2个字节来描述x坐标，两个字节来描述y坐标
		*/
		int x = LOWORD(lParam);//#define LOWORD(l) ((WORD)(((DWORD_PTR)(l)) & 0xffff)) 因此取低位两字节
		int y = HIWORD(lParam);//#define HIWORD(l) ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff)) 表示取高位两字节

		isDown = true;//拖拽效果
		break;
	}
	
	case WM_LBUTTONUP://左键抬起 很多按钮真正响应的时鼠标的抬起操作，比如关闭窗口
	{
		isDown = false;
		KillTimer(hWnd, 1001);
	}
		break;
	case WM_RBUTTONDBLCLK://右键双击  在注册窗口类 需要加上CS_DBCLKS才能响应双击的操作
		MessageBox(0, _T("右键双击"), 0, 0);
		break;
		//6.鼠标移动消息
	case WM_MOUSEMOVE:
	{
		//效果为：随着鼠标的移动，在窗口内从(0,0)点到当前鼠标位置画线
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
	//7.鼠标滚轮消息
	case WM_MOUSEHWHEEL://鼠标滚轮滚动
	{
		//在lparam里面记录x，y 的值，在wparam里面记录滚轮朝前滚还是朝后滚
		//int x = LOWORD(wParam);
		int y = HIWORD(wParam);//在wparam里面记录滚轮朝前滚还是朝后滚 保存在高两字节中
		//1的机器码 0000 0001 -1的机器码 1111 1111 所以两个相加为0  所以机器里面是补码相加
	}
	break;
	//8.计时器消息
	/*
	a.需要在构造消息中WM_CREAT设置计时器SetTimer
	b.通过WM_TIMER得到计时器消息 也可以在SetTimer(hWnd, 1003, 2000, (TIMERPROC)CallBackFun);设置回调函数来进行调用
	c.如果计时器不用了也可以通过killtimer来释放
	*/
	
	case WM_TIMER:
	{
		switch (wParam)//wParam保存计时器的ID号
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

	//4.系统按键 A.alt/f10 B.alt组合键
	case WM_SYSKEYDOWN:
		switch (wParam)
		{
		case VK_F10:
			MessageBox(0, _T("F10"), 0, 0);
			break;
		case 'X'://需要使用ALT+X组合键 并且case VK_MENU 不能存在
			MessageBox(0, _T("ALT+X"), 0, 0);
			break;
		//case VK_MENU://ALT键
		//	MessageBox(0, _T("ALT"), 0, 0);
		//	break;
		}
	//3.字符消息，能区别大写字符 小写字符
	case WM_CHAR://当TranslateMessage()翻译按键消息后会得到字符消息 
	{
		switch (wParam)
		{
		case'!'://WM_CHAR支持组合键输入  比如说拳皇的出招需要组合键(配置一个计时器 在规定时间内连续输入才能形成组合键 利用map/hash表去匹配出招表)
			MessageBox(0, _T("!"), 0, 0);
			break;
		case 'b':
		{
			HDC hdc = GetDC(hWnd);//表示在内存中进行绘制
			TextOut(hdc, 200, 0, TEXT("WM_CHAR b"), _tcslen(TEXT("WM_CHAR b")));
			ReleaseDC(hWnd, hdc);//清除内存绘制
		}
		break;
		case'B'://1.B能输出  2.组合键shitf+b也能输出
		{
			HDC hdc = GetDC(hWnd);//表示在内存中进行绘制
			TextOut(hdc, 400, 0, TEXT("WM_CHAR B"), 9);
			ReleaseDC(hWnd, hdc);//清除内存绘制
		}
		break;
		}
	}
	break;
	//2.键盘消息  有ASCII码的使用ASCII码， 没有的加VK_ 使用虚拟键值 比如VK_TAB
	case WM_KEYDOWN://键盘按下
	{
		switch (wParam)//快速找寻虚拟键值的方法 1.在wParam处下断点 2.输入一个键比如VK_UP然后查看wParam的值3.转换成十六进制0x..4.查键值表
		{
		case 'A'://WM_KEYDOWN不能识别小写，只能识别大写 所以在WM_KEYDOWN消息下应将键值设置为大写
			MessageBox(0, _T("A"), 0, 0);
			break;
		case 'B'://在B按下的时候WM_CHAR WM_KEYDOWN都能匹配
		{
			HDC hdc = GetDC(hWnd);//表示在内存中进行绘制
			TextOut(hdc, 0, 0, TEXT("WM_KEYDOWN B"), 12);
			ReleaseDC(hWnd, hdc);//清除内存绘制
		}
			break;
		case VK_TAB:
			MessageBox(0, _T("Tab"), 0, 0);
			break;
		}
	}
	break;
	case WM_KEYUP://键盘抬起
	{
		switch (wParam)
		{
		case 'C':
			MessageBox(0, _T("A"), 0, 0);
			break;
		}
	}
	break;
    case WM_COMMAND://命令消息/菜单消息
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择: 
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
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);//窗口的系统消息处理函数 比如最大化 最小化 移动窗口 关闭窗口
    }
    return 0;
}

// “关于”框的消息处理程序。
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
