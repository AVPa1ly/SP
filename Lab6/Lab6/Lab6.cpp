#include <iostream>
#include <conio.h>
#include <windows.h>
using namespace std;

class ThreadPar {
public:
    int* share;
    int num;
    ThreadPar(int* sh, int n) { share = sh; num = n; }
};

const int BUF_LEN = 256;

HANDLE CritMutex, CW;
HANDLE c_WriteEvent, // Разрешить запись
c_ReadEvent; // Разрешить чтение

int ActiveReaders = 0, ActiveWriters = 0;
int WaitingReaders = 0, WaitingWriters = 0;
//
void Cout(const char* s) {
    WaitForSingleObject(CritMutex, INFINITE);
    cout << s;
    ReleaseMutex(CritMutex);
}
//
void StartWrite(ThreadPar* p) {
    WaitForSingleObject(CritMutex, INFINITE);

    bool ShareBusy = (ActiveReaders > 0) || (ActiveWriters > 0);
    if (ShareBusy) {
        InterlockedIncrement((LONG*)&WaitingWriters);
    }
    else {
        InterlockedIncrement((LONG*)&ActiveWriters);
        (*p->share)++;
    }
    ReleaseMutex(CritMutex);

    if (ShareBusy) {
        WaitForSingleObject(c_WriteEvent, INFINITE); // Ожидание разрешения на запись
        (*p->share)++;
    }
}
//
void StopWrite(ThreadPar* p) {
    WaitForSingleObject(CritMutex, INFINITE);
    InterlockedDecrement((LONG*)&ActiveWriters);
    HANDLE hEvent = 0;
    if (WaitingReaders) {
        InterlockedExchangeAdd((LONG*)&ActiveReaders, WaitingReaders);
        WaitingReaders = 0;
        hEvent = c_ReadEvent;
    }
    else if (WaitingWriters) {
        InterlockedDecrement((LONG*)&WaitingWriters);
        InterlockedIncrement((LONG*)&ActiveWriters);
        hEvent = c_WriteEvent;
    }
    ReleaseMutex(CritMutex);
    if (hEvent) {
        SetEvent(hEvent);
    }
}
//
void StartRead(ThreadPar* p) {
    WaitForSingleObject(CritMutex, INFINITE);
    bool ShareBusy = (ActiveWriters > 0) || (WaitingWriters > 0);
    if (ShareBusy) {
        InterlockedIncrement((LONG*)&WaitingReaders);
        ResetEvent(c_ReadEvent);
    }
    else {
        InterlockedIncrement((LONG*)&ActiveReaders);
    }
    ReleaseMutex(CritMutex);

    if (ShareBusy) {
        WaitForSingleObject(c_ReadEvent, INFINITE); // Ожидать разрешения на чтение
    }
}
//
void StopRead(ThreadPar* p) {
    WaitForSingleObject(CritMutex, INFINITE);
    InterlockedDecrement((LONG*)&ActiveReaders);
    HANDLE hEvent = 0;
    if (!ActiveReaders && (WaitingWriters > 0)) { // Если нет больше читателей и ждут писатели
        InterlockedDecrement((LONG*)&WaitingWriters);
        InterlockedIncrement((LONG*)&ActiveWriters);
        hEvent = c_WriteEvent;
    }
    ReleaseMutex(CritMutex);
    if (hEvent) {
        SetEvent(hEvent); // Установить событие
    }
}
//
DWORD WINAPI Reader(PVOID pvParam) {
    ThreadPar* p = (ThreadPar*)pvParam;
    while (1) {
        StartRead(p);

        char buf[BUF_LEN];
        sprintf_s(buf, "%s %d %s %d\n", "Reader", p->num, "read", *p->share);
        Cout(buf);
        SetEvent(CW);
        //Sleep(100);
        StopRead(p);
    }
    return 0;
}
//
DWORD WINAPI Writer(PVOID pvParam) {
    ThreadPar* p = (ThreadPar*)pvParam;
    while (1) {
        StartWrite(p);

        char buf[BUF_LEN];
        sprintf_s(buf, "%s %d %s %d\n", "Writer", p->num, "write", *p->share);
        Cout(buf);
        //Sleep(100);
        StopWrite(p);
    }
    return 0;
}
//
int main()
{
    HANDLE Readers[10], Writers[10];

    int share = 0;

    int NumR, NumW;
    NumW = 3;
    NumR = 5;
    CritMutex = CreateMutex(NULL, FALSE, L"Crit");
    // с автосбросом
    c_WriteEvent = CreateEvent(NULL, FALSE, FALSE, L"C_WRITE");
    // сброс вручную
    c_ReadEvent = CreateEvent(NULL, TRUE, FALSE, L"C_READ");
    ThreadPar* p;
    for (int i = 0; i < NumW; i++) {
        p = new ThreadPar(&share, i);
        Writers[i] = CreateThread(NULL, NULL, Writer, (LPVOID)p, NULL, NULL);
    }
    for (int j = 0; j < NumR; j++) {
        p = new ThreadPar(&share, j);
        Readers[j] = CreateThread(NULL, NULL, Reader, (LPVOID)p, NULL, NULL);
    }
    //
    //getch();
    // здесь Sleep можно использовать для теста чтобы программа быстрее останавливалась
    Sleep(40);
    for (int i = 0; i < NumW; i++) {
        ResumeThread(Writers[i]);
        CloseHandle(Writers[i]);
    }
    for (int j = 0; j < NumR; j++) {
        ResumeThread(Readers[j]);
        CloseHandle(Readers[j]);
    }
    CloseHandle(CritMutex);
    CloseHandle(c_ReadEvent);
    CloseHandle(c_WriteEvent);
    return 0;
}