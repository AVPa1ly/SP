#include <windows.h>
#include <stdio.h>
#include <cstring>
#include <string>

using namespace std;

HANDLE hSlot;
LPCTSTR SlotName = TEXT("\\\\.\\mailslot\\my_mailslot");

BOOL WriteSlot(HANDLE hSlot, LPCTSTR lpszMessage)
{
    BOOL fResult;
    DWORD cbWritten;

    fResult = WriteFile(hSlot,
        lpszMessage,
        (DWORD)(lstrlen(lpszMessage) + 1) * sizeof(TCHAR),
        &cbWritten,
        (LPOVERLAPPED)NULL);

    if (!fResult)
    {
        printf("WriteFile failed with %d.\n", GetLastError());
        return FALSE;
    }

    printf("Slot written to successfully.\n");

    return TRUE;
}

int main()
{
    HANDLE hFile;

    hFile = CreateFile(SlotName,
        GENERIC_WRITE,
        FILE_SHARE_READ,
        (LPSECURITY_ATTRIBUTES)NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        (HANDLE)NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        printf("CreateFile failed with %d.\n", GetLastError());
        return FALSE;
    }

    const int size = 100;
    int data[size];
    std::string result;
    for (int i = 0;i < size;i++)
    {
        data[i] = rand();
    }
    int buffer;
    for (int i = 0, j;i < size - 1;i++)
    {
        for (j = 0;j < size - 1 - i;j++)
        {
            if (data[j] > data[j+1])
            {
                buffer = data[j];
                data[j] = data[j + 1];
                data[j + 1] = buffer;
            }
        }
    }
    for (int i = 0;i < size;i++)
    {
        result.append(std::to_string(data[i]));
        result.append(" ");
    }

    result.append("\r\n");

    std::basic_string<TCHAR> converted(result.begin(), result.end());
    

    WriteSlot(hFile, converted.c_str());

    CloseHandle(hFile);

    return TRUE;
}