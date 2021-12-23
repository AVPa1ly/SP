// Lab2.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "framework.h"
#include "Lab2.h"
#include <shobjidl.h> 
#include "ProcessLIst.cpp"
#include <stdio.h> 

#define MAX_LOADSTRING 100
#define IDM_FILEBUTTON 10001
#define IDM_CLOSEBUTTON 10002

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING] = L"Lab2";                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HRESULT BasicFileOpen();
ProcessList<MY_PROCESS_INFO> processList;//список процессов
static HWND hDeleteEdit;//поле ввода
int TIMER_ID = 1;


// Forward declarations of functions included in this code module:
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

    // TODO: Place code here.

    // Initialize global string
    LoadStringW(hInstance, IDC_LAB2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB2));

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
    WNDCLASSEXW wcex;// создаём экземпляр для обращения к членам класса WNDCLASSEX

    wcex.cbSize = sizeof(WNDCLASSEX);// размер структуры (в байтах)

    wcex.style = CS_HREDRAW | CS_VREDRAW;// стиль класса окошка
    wcex.lpfnWndProc = WndProc;// указатель на функцию обработки сообщений
    wcex.cbClsExtra = 0; // число дополнительных байтов при создании экземпляра приложения
    wcex.cbWndExtra = 0;// число дополнительных байтов в конце структуры
    wcex.hInstance = hInstance;// указатель на строку, содержащую имя меню, применяемого для класса
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB2));// декриптор пиктограммы
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);// дескриптор курсора
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);// дескриптор кисти для закраски фона окна
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB2);// указатель на имя меню (у нас его нет)
    wcex.lpszClassName = szWindowClass;// указатель на имя класса
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));// дескриптор маленькой пиктограммы

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

   HWND hWnd = CreateWindowW(
       szWindowClass,  // имя класса
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

   //определение кнопок и строки ввода

   HWND hFileButtonAddWnd = CreateWindow(_T("BUTTON"), _T("File"), WS_CHILD | WS_VISIBLE,
       50, 50, 100, 30, hWnd, (HMENU)IDM_FILEBUTTON, hInst, NULL);

   HWND hCloseButtonAddWnd = CreateWindow(_T("BUTTON"), _T("Close"), WS_CHILD | WS_VISIBLE,
       50, 100, 100, 30, hWnd, (HMENU)IDM_CLOSEBUTTON, hInst, NULL);

   hDeleteEdit = CreateWindow(L"edit", L"0",
       WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT, 50, 150, 50, 20,
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
    PAINTSTRUCT ps;
    HDC hdc;
    TCHAR StrA[20];
    int buffer;
    PROCESS_INFORMATION infoBuffer;
    switch (message)
    {
    case WM_CREATE:
    {
        SetTimer(hWnd, TIMER_ID, 5000, NULL);
        break;
    }
    case WM_TIMER:
    {
        processList.Check();
        InvalidateRect(hWnd, NULL, TRUE);
    }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_FILEBUTTON:
                BasicFileOpen();
                InvalidateRect(hWnd, NULL, TRUE);
                break;
            case IDM_CLOSEBUTTON:
                buffer = GetWindowText(hDeleteEdit, StrA, 20);
                if (!processList.GetInfo(&infoBuffer, _tstoi(StrA)))
                {
                    MessageBoxW(NULL, L"GetInfo failed", L"Error", MB_OK);
                    break;
                }
                if (!PostThreadMessage(infoBuffer.dwThreadId, WM_QUIT, (WPARAM)0, (LPARAM)0))
                    MessageBoxW(NULL, L"PostThreadMessage failed", L"Error", MB_OK);
                else
                {
                    CloseHandle(infoBuffer.hProcess);
                    CloseHandle(infoBuffer.hThread);
                }
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
            hdc = BeginPaint(hWnd, &ps);            

            processList.Draw(hdc);

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_CLOSE://нажав кнопку Закрыть или воспользовавшись сочетанием клавиш, например ALT + F4
        if (MessageBox(hWnd, L"Really quit?", L"My application", MB_OKCANCEL) == IDOK)
        {
            DestroyWindow(hWnd);//шлёт на WM_DESTROY
        }
        break;
    case WM_DESTROY://срабатывает когда окно закрывают
        PostQuitMessage(0);//помещает в очередь сообщений сообщение WM _ Quit , что приводит к завершению цикла обработки сообщений.   
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


HRESULT BasicFileOpen()//диалог выбора файла
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
        COINIT_DISABLE_OLE1DDE);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (SUCCEEDED(hr))
    {
        IFileOpenDialog* pFileOpen;

        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
            IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr))
        {
            // Show the Open dialog box.
            hr = pFileOpen->Show(NULL);

            // Get the file name from the dialog box.
            if (SUCCEEDED(hr))
            {
                IShellItem* pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    if (SUCCEEDED(hr))
                    {
                        // Set the default extension to be ".doc" file.
                        hr = pFileOpen->SetDefaultExtension(L"exe");
                        // Display the file name to the user.
                        if (SUCCEEDED(hr))
                        {
                            if(!CreateProcess(NULL,   // No module name (use command line)
                                pszFilePath,        // Command line
                                NULL,           // Process handle not inheritable
                                NULL,           // Thread handle not inheritable
                                FALSE,          // Set handle inheritance to FALSE
                                0,              // No creation flags
                                NULL,           // Use parent's environment block
                                NULL,           // Use parent's starting directory 
                                &si,            // Pointer to STARTUPINFO structure
                                &pi)           // Pointer to PROCESS_INFORMATION structure)
                                )
                            {
                                MessageBoxW(NULL, L"CreateProcess failed", L"Error", MB_OK);
                            }
                            
                            std::wstring ws(pszFilePath);
                            std::string str(ws.begin(), ws.end());


                            MY_PROCESS_INFO newInfo = { pi, TRUE, str };
                            processList.AddNode(newInfo);
                            CoTaskMemFree(pszFilePath);
                        }
                        pItem->Release();
                    }                    
                }
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }

    return 0;
}



