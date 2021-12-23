#pragma comment (lib, "Msimg32.lib")
#include "framework.h"
#include "WindowsProject3.h"
#include <vector>
#include <commdlg.h>
#include <commctrl.h>
#include <wingdi.h>

#define MAX_LOADSTRING 100
// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
int size = 1;
BOOL LMouseIsClicked = false;
POINT LastP;
POINT NewP;
std::vector<POINT> points;
int CurDrawMode = 1;
std::vector<HBITMAP> hbmMemory;
std::vector<HDC> hdcMemory;
std::vector<POINT> figMem;
int index = -1;
COLORREF lastColor = RGB(255, 0, 255);
HPEN pen = CreatePen(PS_SOLID, 2 * size, lastColor);
HBRUSH brush = (HBRUSH)GetStockObject(NULL_BRUSH);
HPEN erasePen = CreatePen(PS_SOLID, 2 * size, RGB(255, 255, 255));
BOOL saveFlag = false;
BOOL loadFlag = true;
BOOL missFlag = false;
BOOL createFlag = false;
BOOL isRight = true;
BOOL isVert = false;
HDC hdcMem;
HBITMAP hbmMem;
POINT WindSize;
HBITMAP hbmScreen;
char num[2];
HWND hPenc, hLine, hBack, hForward, hCircle, hRect, hPen, hEraser, hWidth, hFill, hGradFill, hClear, hUp, hDown, hRight, hLeft;


// Отправить объявления функций, включенных в этот модуль кода:
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

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WIN32LAB, szWindowClass, MAX_LOADSTRING);

    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32LAB));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32LAB));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = 0;
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WIN32LAB);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_WIN32LAB));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
bool MouseInClientZone(HWND hWnd)
{
    RECT rt;
    POINT cur;
    GetCursorPos(&cur);
    GetClientRect(hWnd, &rt);
    POINT zeroP = { 0,0 };
    ClientToScreen(hWnd, &zeroP);
    rt.left += zeroP.x;
    rt.right += zeroP.x;
    rt.top += zeroP.y;
    rt.bottom += zeroP.y;
    return (cur.x >= rt.left) && (cur.x <= rt.right) && (cur.y >= rt.top) && (cur.y <= rt.bottom);
}

CHOOSECOLOR GetColorD(HWND hWndc)
{
    CHOOSECOLOR cc;
    static COLORREF acrCustClr[16];
    static DWORD rgbCurrent;

    ZeroMemory(&cc, sizeof(cc));
    cc.lStructSize = sizeof(cc);
    cc.hwndOwner = hWndc;
    cc.lpCustColors = (LPDWORD)acrCustClr;
    cc.rgbResult = rgbCurrent;
    cc.Flags = CC_FULLOPEN | CC_RGBINIT;
    return cc;
}

void DrawLine(HDC hdc, POINT temp1, POINT temp2)
{
    MoveToEx(hdc, temp2.x, temp2.y, NULL);
    LineTo(hdc, temp1.x, temp1.y);
}
void DrawCircle(HDC hdc, POINT temp1, POINT temp2)
{
    Ellipse(hdc, temp1.x, temp1.y, temp2.x, temp2.y);
}
void DrawRect(HDC hdc, POINT temp1, POINT temp2)
{
    Rectangle(hdc, temp1.x, temp1.y, temp2.x, temp2.y);
}
void SimpleFill(HDC hdc, POINT temp1)
{
    HBRUSH tempbrush = (HBRUSH)CreateSolidBrush(lastColor);
    SelectObject(hdc, tempbrush);
    ExtFloodFill(hdc, temp1.x, temp1.y, GetPixel(hdc, temp1.x, temp1.y), FLOODFILLSURFACE);
    DeleteObject(tempbrush);
}

VOID GradientFill(HDC hdc, POINT temp1, BOOL isVert, BOOL isRight, COLORREF firstColor, COLORREF secondColor)
{
    TRIVERTEX        trivertex[2];
    GRADIENT_RECT    gRect;
    int i;
    if (isRight) {
        COLORREF temp = firstColor;
        firstColor = secondColor;
        secondColor = temp;
    }
    trivertex[0].x = 0;
    trivertex[0].y = 0;
    trivertex[0].Red = GetRValue(firstColor) << 8;
    trivertex[0].Green = GetGValue(firstColor) << 8; //приведение
    trivertex[0].Blue = GetBValue(firstColor) << 8;
    trivertex[0].Alpha = 0x0000;
    trivertex[1].x = WindSize.x;
    trivertex[1].y = WindSize.y;
    trivertex[1].Red = GetRValue(secondColor) << 8;
    trivertex[1].Green = GetGValue(secondColor) << 8;
    trivertex[1].Blue = GetBValue(secondColor) << 8;
    trivertex[1].Alpha = 0x0000;

    gRect.UpperLeft = 0;
    gRect.LowerRight = 1;
    HDC hdcMemT = CreateCompatibleDC(hdc);
    HBITMAP hbmMemT = CreateCompatibleBitmap(hdc, WindSize.x, WindSize.y);
    SelectObject(hdcMemT, hbmMemT);
    isVert ?
        GradientFill(hdcMemT, trivertex, 2, &gRect, 1, GRADIENT_FILL_RECT_V) :
        GradientFill(hdcMemT, trivertex, 2, &gRect, 1, GRADIENT_FILL_RECT_H);
    HBRUSH tempbrush = CreatePatternBrush(hbmMemT);
    SelectObject(hdc, tempbrush);
    ExtFloodFill(hdc, temp1.x, temp1.y, GetPixel(hdc, temp1.x, temp1.y), FLOODFILLSURFACE);
    DeleteObject(tempbrush);
}

BOOL SaveHBITMAPToFile(HBITMAP hBitmap, LPCTSTR lpszFileName)
{
    HDC hdc;
    int iBits;
    WORD wBitCount;
    DWORD dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
    BITMAP Bitmap0;
    BITMAPFILEHEADER bmfHdr;
    BITMAPINFOHEADER bi;
    LPBITMAPINFOHEADER lpbi;
    HANDLE fh, hDib, hPal, hOldPal2 = NULL;
    hdc = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
    iBits = GetDeviceCaps(hdc, BITSPIXEL) * GetDeviceCaps(hdc, PLANES);
    DeleteDC(hdc);
    if (iBits <= 1)
        wBitCount = 1;
    else if (iBits <= 4)
        wBitCount = 4;
    else if (iBits <= 8)
        wBitCount = 8;
    else
        wBitCount = 24;
    GetObject(hBitmap, sizeof(Bitmap0), (LPSTR)&Bitmap0);
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = Bitmap0.bmWidth;
    bi.biHeight = -Bitmap0.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = wBitCount;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrImportant = 0;
    bi.biClrUsed = 256;
    dwBmBitsSize = ((Bitmap0.bmWidth * wBitCount + 31) & ~31) / 8
        * Bitmap0.bmHeight;
    hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
    *lpbi = bi;

    hPal = GetStockObject(DEFAULT_PALETTE);
    if (hPal)
    {
        hdc = GetDC(NULL);
        hOldPal2 = SelectPalette(hdc, (HPALETTE)hPal, FALSE);
        RealizePalette(hdc);
    }


    GetDIBits(hdc, hBitmap, 0, (UINT)Bitmap0.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER)
        + dwPaletteSize, (BITMAPINFO*)lpbi, DIB_RGB_COLORS);

    if (hOldPal2)
    {
        SelectPalette(hdc, (HPALETTE)hOldPal2, TRUE);
        RealizePalette(hdc);
        ReleaseDC(NULL, hdc);
    }

    fh = CreateFile(lpszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

    if (fh == INVALID_HANDLE_VALUE)
        return FALSE;

    bmfHdr.bfType = 0x4D42; // "BM"
    dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
    bmfHdr.bfSize = dwDIBSize;
    bmfHdr.bfReserved1 = 0;
    bmfHdr.bfReserved2 = 0;
    bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

    WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);

    WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
    GlobalUnlock(hDib);
    GlobalFree(hDib);
    CloseHandle(fh);
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case(WM_CREATE):
        RECT rt;
        SendMessage(hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
        GetClientRect(hWnd, &rt);
        WindSize.x = rt.right;
        WindSize.y = rt.bottom;
        SetTimer(hWnd, 1, 100, NULL);
        hPenc = CreateWindow(L"button", L"Pencil",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            0, 0, 90, 30, hWnd, 0, hInst, NULL);
        hLine = CreateWindow(L"button", L"Line",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            0, 30, 90, 30, hWnd, 0, hInst, NULL);
        hBack = CreateWindow(L"button", L"Back",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            0, 270, 90, 30, hWnd, 0, hInst, NULL);
        hForward = CreateWindow(L"button", L"Forward",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            0, 300, 90, 30, hWnd, 0, hInst, NULL);
        hCircle = CreateWindow(L"button", L"Ellipse",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            0, 60, 90, 30, hWnd, 0, hInst, NULL);
        hRect = CreateWindow(L"button", L"Rectangle",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            0, 90, 90, 30, hWnd, 0, hInst, NULL);
        hPen = CreateWindow(L"button", L"Brush color",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            0, 120, 90, 30, hWnd, 0, hInst, NULL);
        hEraser = CreateWindow(L"button", L"Eraser",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            0, 150, 90, 30, hWnd, 0, hInst, NULL);
        hWidth = CreateWindow(_T("ComboBox"), _T("aa"), WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
            0, 420, 90, 400,
            hWnd, 0, hInst, NULL);
        hFill = CreateWindow(L"button", L"Fill",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            0, 180, 90, 30, hWnd, 0, hInst, NULL);
        hGradFill = CreateWindow(L"button", L"Gradient fill",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            0, 210, 90, 30, hWnd, 0, hInst, NULL);
        hClear = CreateWindow(L"button", L"Clear screen",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            0, 240, 90, 30, hWnd, 0, hInst, NULL);
        hUp = CreateWindow(L"button", L"↑",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            32, 348, 25, 25, hWnd, 0, hInst, NULL);
        hDown = CreateWindow(L"button", L"↓",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            32, 392, 25, 25, hWnd, 0, hInst, NULL);
        hRight = CreateWindow(L"button", L"→",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            62, 370, 25, 25, hWnd, 0, hInst, NULL);
        hLeft = CreateWindow(L"button", L"←",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            2, 370, 24, 24, hWnd, 0, hInst, NULL);
        CreateWindowEx(WS_EX_WINDOWEDGE,
            L"BUTTON",
            L"Grad Dir:",
            WS_VISIBLE | WS_CHILD | BS_GROUPBOX,  // Styles 
            0, 330, 90, 90,
            hWnd,
            NULL,
            hInst, NULL);
        for (int i = 1; i < 10; i++)
        {
            _itoa_s(i, num, 10);
            SendMessage(hWidth, CB_ADDSTRING, 1, (LPARAM)num);
        }
        SendMessage(hWidth, CB_SETCURSEL, 0, 0L);
        break;

    case(WM_SIZE):
    case(WM_MOVE):
        loadFlag = true;
        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case(WM_LBUTTONDOWN):
        LMouseIsClicked = true;
        missFlag = true;
        switch (CurDrawMode)
        {
        case 1:
        case 5:
        {
            LastP = { LOWORD(lParam), HIWORD(lParam) };
            NewP = LastP;
        }
        break;
        case 2:
        case 3:
        case 4:
        {
            LastP = { LOWORD(lParam), HIWORD(lParam) };
        }
        break;
        case 7:
        case 6:
            LastP = { LOWORD(lParam), HIWORD(lParam) };
            saveFlag = true;
            createFlag = true;
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        default:
            break;
        }

        break;
    case(WM_TIMER):
        if ((GetKeyState(VK_LBUTTON) & 0x8000) != 0 || LMouseIsClicked == false)
            break;
    case(WM_LBUTTONUP):
        LMouseIsClicked = false;
        switch (CurDrawMode)
        {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            if (!missFlag)
            {
                saveFlag = true;
                InvalidateRect(hWnd, NULL, TRUE);
                break;
            }
        }
        break;
    case WM_MOUSEMOVE:
    {
        if (LMouseIsClicked)
        {
            switch (CurDrawMode)
            {
            case 1:
            case 5:
                missFlag = false;
                LastP = NewP;
                NewP = { LOWORD(lParam), HIWORD(lParam) };
                InvalidateRect(hWnd, NULL, TRUE);
                break;
            case 2:
            case 3:
            case 4:
                missFlag = false;
                NewP = { LOWORD(lParam), HIWORD(lParam) };
                InvalidateRect(hWnd, NULL, TRUE);
                break;

            default:
                break;
            }
        }

    }
    break;
    case WM_COMMAND:
    {
        if (lParam == (LPARAM)hPenc)    
        {
            CurDrawMode = 1;
        }
        else if (lParam == (LPARAM)hLine)    
        {
            CurDrawMode = 2;
        }
        else if (lParam == (LPARAM)hBack)    
        {
            if (index > -1)
            {
                index--;
                loadFlag = true;
                InvalidateRect(hWnd, NULL, TRUE);
            }
        }
        else if (lParam == (LPARAM)hForward)    
        {
            if (index < (int)hbmMemory.size() - 1)
            {
                index++;
                loadFlag = true;
                InvalidateRect(hWnd, NULL, TRUE);
            }
        }
        else if (lParam == (LPARAM)hCircle)    
        {
            CurDrawMode = 3;
        }
        else if (lParam == (LPARAM)hRect)    
        {
            CurDrawMode = 4;
        }
        else if (lParam == (LPARAM)hPen)    
        {
            CHOOSECOLOR temp = GetColorD(hWnd);
            if (ChooseColor(&temp) == TRUE)
            {
                DeleteObject(pen);
                lastColor = temp.rgbResult;
                pen = CreatePen(PS_SOLID, 2 * size, temp.rgbResult);
            }
        }
        else if (lParam == (LPARAM)hEraser)    
        {
            CurDrawMode = 5;
        }
        else if (HIWORD(wParam) == CBN_SELCHANGE)
        {
            int ItemIndex = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL,
                (WPARAM)0, (LPARAM)0);
            TCHAR  ListItem[256];
            (TCHAR)SendMessage((HWND)lParam, (UINT)CB_GETLBTEXT,
                (WPARAM)ItemIndex, (LPARAM)ListItem);
            int newSize = ListItem[0] - '0';
            pen = CreatePen(PS_SOLID, 2 * newSize, lastColor);
            erasePen = CreatePen(PS_SOLID, 2 * newSize, RGB(255, 255, 255));
        }
        else if (lParam == (LPARAM)hFill)
        {
            CurDrawMode = 6;
        }
        else if (lParam == (LPARAM)hGradFill)
        {
            CurDrawMode = 7;
        }
        else if (lParam == (LPARAM)hClear)
        {
            hbmMemory.clear();
            hdcMemory.clear();
            index = -1;
            loadFlag = true;
            InvalidateRect(hWnd, NULL, TRUE);
        }
        else if (lParam == (LPARAM)hUp) 
        {
            isVert = true;
            isRight = true;
        }
        else if (lParam == (LPARAM)hDown)
        {
            isVert = true;
            isRight = false;
        }
        else if (lParam == (LPARAM)hRight)
        {
            isVert = false;
            isRight = false;
        }
        else if (lParam == (LPARAM)hLeft)
        {
            isVert = false;
            isRight = true;
        }
        int wmId = LOWORD(wParam);
        // Разобрать выбор в меню:

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
        if (loadFlag)
        {
            if (index == -1)
            {
                hdcMem = CreateCompatibleDC(hdc);
                hbmMem = CreateCompatibleBitmap(hdc, WindSize.x, WindSize.y);
                SelectObject(hdcMem, hbmMem);
                HBRUSH tempbrush = (HBRUSH)CreateSolidBrush(RGB(255,255,255));
                SelectObject(hdcMem, tempbrush);
                Rectangle(hdcMem, 0, 0, WindSize.x, WindSize.y);
            }
            else
            {
                hdcMem = hdcMemory[index];
                hbmMem = hbmMemory[index];
                SelectObject(hdcMem, hbmMem);
            }
            
            BitBlt(hdc, 0, 0, WindSize.x, WindSize.y, hdcMem, 0, 0, SRCCOPY);
            loadFlag = false;
            NewP.x = -1;
            NewP.y = -1;
            LastP.x = -1;
            LastP.y = -1;
        }
        else
        {
            if (saveFlag)
            {
                SelectObject(hdcMem, pen);

                HBITMAP temp = (HBITMAP)CopyImage(hbmMem, IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
                switch (CurDrawMode)
                {
                case 5:
                    SelectObject(hdcMem, erasePen);
                case 1:
                    while (figMem.size() != 0)
                    {
                        DrawLine(hdcMem, figMem.back(), figMem[figMem.size() - 2]);
                        figMem.pop_back();
                        figMem.pop_back();
                    }
                    break;
                case 2:
                    DrawLine(hdcMem, LastP, NewP);
                    break;
                case 3:
                    SelectObject(hdcMem, brush);
                    DrawCircle(hdcMem, LastP, NewP);
                    break;
                case 4:
                    SelectObject(hdcMem, brush);
                    DrawRect(hdcMem, LastP, NewP);
                    break;
                case 6:
                    SimpleFill(hdcMem, LastP);
                    SimpleFill(hdc, LastP);
                    break;
                case 7:
                    GradientFill(hdc, LastP, isVert, isRight, RGB(0, 0, 0), lastColor);
                    GradientFill(hdcMem, LastP, isVert, isRight, RGB(0, 0, 0), lastColor);
                    break;
                default:
                    return 0;
                }
                int n = hbmMemory.size() - index - 1;
                for (int i = 0; i < n; i++)
                {
                    hbmMemory.pop_back();
                    hdcMemory.pop_back();
                }
                hbmMemory.push_back((HBITMAP)CopyImage(hbmMem, IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE));
                hdcMemory.push_back(hdcMem);
                if (index > -1)
                    hbmMemory[index] = temp;
                index++;
                saveFlag = false;
                NewP.x = -1;
                NewP.y = -1;
                LastP.x = -1;
                LastP.y = -1;
            }
            SelectObject(hdc, pen);
            switch (CurDrawMode)
            {
            case 5:
                SelectObject(hdc, erasePen);

            case 1:
                DrawLine(hdc, LastP, NewP);

                figMem.push_back(LastP);
                figMem.push_back(NewP);
                break;
            case 2:

                BitBlt(hdc, 0, 0, WindSize.x, WindSize.y, hdcMem, 0, 0, SRCCOPY);

                DrawLine(hdc, LastP, NewP);
                break;
            case 3:

                BitBlt(hdc, 0, 0, WindSize.x, WindSize.y, hdcMem, 0, 0, SRCCOPY);
                SelectObject(hdc, brush);

                DrawCircle(hdc, LastP, NewP);
                break;
            case 4:

                BitBlt(hdc, 0, 0, WindSize.x, WindSize.y, hdcMem, 0, 0, SRCCOPY);
                SelectObject(hdc, brush);

                DrawRect(hdc, LastP, NewP);
                break;
            }
        }
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_KEYDOWN:
    {
        switch (wParam) {
        case VK_LEFT:
            if (index > -1)
            {
                index--;
                loadFlag = true;
                InvalidateRect(hWnd, NULL, TRUE);
            }
            break;
        case VK_RIGHT:
            if (index < (int)hbmMemory.size() - 1)
            {
                index++;
                loadFlag = true;
                InvalidateRect(hWnd, NULL, TRUE);
            }
            break;
        default:
            break;
        }
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

// Обработчик сообщений для окна "О программе".
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
