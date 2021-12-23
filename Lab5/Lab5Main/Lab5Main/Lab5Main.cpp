// Lab5Main.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Lab5Main.h"
#include "ProcessLIst.cpp"
#include <shobjidl.h> 
#include <strsafe.h>

#define MAX_LOADSTRING 100
#define IDM_STARTBUTTON 10001
#define IDM_TESTBUTTON 10002

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING] = L"Lab5";;                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
ProcessList<MY_PROCESS_INFO> processList;       //список процессов
static HWND hInfoEdit;
int TIMER_ID = 1;
HANDLE hSlot;
LPCTSTR SlotName = TEXT("\\\\.\\mailslot\\my_mailslot");


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
HRESULT             BasicFileOpen();
BOOL WINAPI         MakeSlot(LPCTSTR lpszSlotName);
BOOL                ReadSlot();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDC_LAB5MAIN, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB5MAIN));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB5MAIN));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB5MAIN);
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
    case WM_CREATE:
    {
        MakeSlot(SlotName);
        SetTimer(hWnd, TIMER_ID, 10000, NULL);
        break;
    }
    case WM_TIMER:
    {
        //processList.Check();
        //ReadSlot();
    }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_STARTBUTTON:
                BasicFileOpen();
                WaitForSingleObject(processList.GetInfo(processList.size - 1).procInfo.hProcess, INFINITE);
                ReadSlot();
                processList.PrintLast(hInfoEdit);
                CloseHandle(processList.GetInfo(processList.size - 1).procInfo.hProcess);
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
                        // Set the default extension to be ".exe" file.
                        hr = pFileOpen->SetDefaultExtension(L"exe");
                        // Display the file name to the user.
                        if (SUCCEEDED(hr))
                        {
                            if (!CreateProcess(NULL,   // No module name (use command line)
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


                            LPTSTR lpszDynamic = new TCHAR[1001];//примерно сколько символов

                            MY_PROCESS_INFO newInfo = { pi, TRUE, str, lpszDynamic};
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

BOOL WINAPI MakeSlot(LPCTSTR lpszSlotName)
{
    hSlot = CreateMailslot(lpszSlotName,
        0,                             // no maximum message size 
        MAILSLOT_WAIT_FOREVER,         // no time-out for operations 
        (LPSECURITY_ATTRIBUTES)NULL); // default security

    if (hSlot == INVALID_HANDLE_VALUE)
    {
        MessageBox(NULL, L"CreateMailslot failed", L"Fuck", MB_OK);
        return FALSE;
    }
    return TRUE;
}

BOOL ReadSlot()
{
    DWORD cbMessage, cMessage, cbRead;
    BOOL fResult;
    LPTSTR lpszBuffer;
    TCHAR achID[80];
    DWORD cAllMessages;
    HANDLE hEvent;
    OVERLAPPED ov;

    cbMessage = cMessage = cbRead = 0;

    hEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("ExampleSlot"));
    if (NULL == hEvent)
        return FALSE;
    ov.Offset = 0;
    ov.OffsetHigh = 0;
    ov.hEvent = hEvent;

    fResult = GetMailslotInfo(hSlot, // mailslot handle 
        (LPDWORD)NULL,               // no maximum message size 
        &cbMessage,                   // size of next message 
        &cMessage,                    // number of messages 
        (LPDWORD)NULL);              // no read time-out 

    if (!fResult)
    {
        MessageBox(NULL, L"GetMailslotInfo failed", L"Fuck", MB_OK);
        return FALSE;
    }

    if (cbMessage == MAILSLOT_NO_MESSAGE)
    {
        MessageBox(NULL, L"Waiting for a message...", L"Ok", MB_OK);
        return TRUE;
    }

    cAllMessages = cMessage;

    while (cMessage != 0)  // retrieve all messages
    {
        // Create a message-number string. 

        StringCchPrintf((LPTSTR)achID,
            80,
            TEXT("\nMessage #%d of %d\n"),
            cAllMessages - cMessage + 1,
            cAllMessages);

        // Allocate memory for the message. 

        lpszBuffer = (LPTSTR)GlobalAlloc(GPTR, lstrlen((LPTSTR)achID) * sizeof(TCHAR) + cbMessage);
        if (NULL == lpszBuffer)
            return FALSE;
        lpszBuffer[0] = '\0';

        fResult = ReadFile(hSlot,
            lpszBuffer,
            cbMessage,
            &cbRead,
            &ov);

        if (!fResult)
        {
            MessageBox(NULL, L"ReadFile failed", L"Fuck", MB_OK);
            GlobalFree((HGLOBAL)lpszBuffer);
            return FALSE;
        }

        // Concatenate the message and the message-number string. 

        /*StringCbCat(lpszBuffer,
            lstrlen((LPTSTR)achID) * sizeof(TCHAR) + cbMessage,
            (LPTSTR)achID);*/

        // Display the message. 

        //_tprintf(TEXT("Contents of the mailslot: %s\n"), lpszBuffer);
        //processList.AppendText(hInfoEdit, lpszBuffer);

        wcscpy_s(processList.GetInfo(processList.size - 1).result, wcslen(lpszBuffer) + 1, lpszBuffer);

        GlobalFree((HGLOBAL)lpszBuffer);

        fResult = GetMailslotInfo(hSlot,  // mailslot handle 
            (LPDWORD)NULL,               // no maximum message size 
            &cbMessage,                   // size of next message 
            &cMessage,                    // number of messages 
            (LPDWORD)NULL);              // no read time-out 

        if (!fResult)
        {
            MessageBox(NULL, L"GetMailslotInfo failed", L"Fuck", MB_OK);
            return FALSE;
        }
    }
    CloseHandle(hEvent);
    return TRUE;
}
