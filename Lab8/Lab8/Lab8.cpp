// Lab8.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Lab8.h"
#include <string>

#define BUFSIZE 1024
#define MAX_LOADSTRING 100
#define IDM_STARTBUTTON 10001

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING] = L"Lab8";                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
static HWND hInfoEdit;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void                AppendText(HWND hEditWnd, LPTSTR Text);
void                AppendText(HWND hEditWnd, std::wstring str);
void                AppendText(HWND hEditWnd, DWORD Text);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDC_LAB8, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB8));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB8));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB8);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass,  // имя класса
       szTitle, // заголовок окошка
       WS_OVERLAPPEDWINDOW | WS_VSCROLL, // режимы отображения окошка
       CW_USEDEFAULT, // позиция окошка по оси х
       NULL, // позиция окошка по оси у (у нас х по умолчанию и этот параметр писать не нужно)
       CW_USEDEFAULT, // ширина окошка
       NULL, // высота окошка (раз ширина по умолчанию, то писать не нужно)
       (HWND)NULL, // дескриптор родительского окна
       NULL, // дескриптор меню
       HINSTANCE(hInst), // дескриптор экземпляра приложения
       NULL); // ничего не передаём из WndProc

   if (!hWnd)
   {
       // в случае некорректного создания окошка (неверные параметры и т.п.):
       MessageBox(NULL, L"Не получилось создать окно!", L"Ошибка", MB_OK);
       return FALSE;
   }

   HWND hStartButtonAddWnd = CreateWindow(_T("BUTTON"), _T("Start"), WS_CHILD | WS_VISIBLE,
       50, 50, 50, 20, hWnd, (HMENU)IDM_STARTBUTTON, hInst, NULL);

   hInfoEdit = CreateWindow(L"edit", L"",
       ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | WS_TABSTOP | WS_VSCROLL | WS_HSCROLL, 150, 50, 300, 200,
       hWnd, 0, hInst, NULL);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);

            LPTSTR lpszSystemInfo;      // указатель на строку, в которой будет информация о системе.
            DWORD cchBuff = 256;        // длина имени компьютера или пользователя.
            TCHAR tchBuffer[BUFSIZE];   // буфер для строки.
            DWORD dwResult;             // возвращаемое значение функции.

            SYSTEM_INFO siSysInfo;

            lpszSystemInfo = tchBuffer;
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_STARTBUTTON:
                
                // Получаем и отображаем имя компьютера.

                if (GetComputerName(lpszSystemInfo, &cchBuff)) {
                    AppendText(hInfoEdit,L"Computer name : ");
                    AppendText(hInfoEdit, lpszSystemInfo);
                    AppendText(hInfoEdit,L"\r\n");
                }
                // Получаем и отображаем имя пользователя.

                if (GetUserName(lpszSystemInfo, &cchBuff)) {
                    AppendText(hInfoEdit, L"User name: ");
                    AppendText(hInfoEdit, lpszSystemInfo);
                    AppendText(hInfoEdit, L"\r\n");
                }
                // Получаем и отображаем системную директорию.

                if (GetSystemDirectory(lpszSystemInfo, MAX_PATH + 1)) {
                    AppendText(hInfoEdit, L"System directory: ");
                    AppendText(hInfoEdit, lpszSystemInfo);
                    AppendText(hInfoEdit, L"\r\n");
                }

                GetSystemInfo(&siSysInfo);

                AppendText(hInfoEdit, L"Hardware information: \r\n");

                AppendText(hInfoEdit, L"OEM ID:");
                AppendText(hInfoEdit, siSysInfo.dwOemId);
                AppendText(hInfoEdit, L"\r\n");

                AppendText(hInfoEdit, L"Number of processors:");
                AppendText(hInfoEdit, siSysInfo.dwNumberOfProcessors);
                AppendText(hInfoEdit, L"\r\n");

                AppendText(hInfoEdit, L"Page size:");
                AppendText(hInfoEdit, siSysInfo.dwPageSize);
                AppendText(hInfoEdit, L"\r\n");

                AppendText(hInfoEdit, L"Processor type:");
                AppendText(hInfoEdit, siSysInfo.dwProcessorType);
                AppendText(hInfoEdit, L"\r\n");

                AppendText(hInfoEdit, L"Active processor mask:");
                AppendText(hInfoEdit, siSysInfo.dwActiveProcessorMask);
                AppendText(hInfoEdit, L"\r\n");

                AppendText(hInfoEdit, L"Processor level:");
                AppendText(hInfoEdit, siSysInfo.wProcessorLevel);
                AppendText(hInfoEdit, L"\r\n");

                AppendText(hInfoEdit, L"Processor revision:");
                AppendText(hInfoEdit, siSysInfo.wProcessorRevision);
                AppendText(hInfoEdit, L"\r\n");

                break;
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
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
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


void AppendText(HWND hEditWnd, std::wstring str)//вывести текст в окно
{
    std::basic_string<TCHAR> converted(str.begin(), str.end());
    LPCTSTR Text = converted.c_str();
    int idx = GetWindowTextLength(hEditWnd);
    SendMessage(hEditWnd, EM_SETSEL, (WPARAM)idx, (LPARAM)idx);
    SendMessage(hEditWnd, EM_REPLACESEL, 0, (LPARAM)Text);
}

void AppendText(HWND hEditWnd, LPTSTR Text)//вывести текст в окно
{
    int idx = GetWindowTextLength(hEditWnd);
    SendMessage(hEditWnd, EM_SETSEL, (WPARAM)idx, (LPARAM)idx);
    SendMessage(hEditWnd, EM_REPLACESEL, 0, (LPARAM)Text);
}

void AppendText(HWND hEditWnd, DWORD Text)//вывести текст в окно
{
    TCHAR buf[32];
    _itot_s(Text, buf, 10, 10);
    int idx = GetWindowTextLength(hEditWnd);
    SendMessage(hEditWnd, EM_SETSEL, (WPARAM)idx, (LPARAM)idx);
    SendMessage(hEditWnd, EM_REPLACESEL, 0, (LPARAM)buf);
}