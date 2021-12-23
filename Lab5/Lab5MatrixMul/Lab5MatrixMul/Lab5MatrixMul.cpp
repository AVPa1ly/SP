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

void Multiply(int** matrix1, int** matrix2, int** res_matrix, int size)
{
    for (int r = 0; r < size; r++) {
        for (int c = 0; c < size; c++) {
            for (int in = 0; in < size; in++) {
                res_matrix[r][c] += matrix1[r][in] * matrix2[in][c];
            }
        }
    }
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

    const int size = 10;

    int** matrix1 = new int*[size];
    int** matrix2 = new int* [size];
    int** res_matrix = new int* [size];
    for (int i = 0;i < size;i++)
    {
        matrix1[i] = new int[size];
        matrix2[i] = new int[size];
        res_matrix[i] = new int[size];
    }

    std::string result;
    for (int i = 0, j;i < size;i++)
    {
        for (j = 0;j < size;j++)
        {
            matrix1[i][j] = rand() % 100;
            matrix2[i][j] = rand() % 100;
            res_matrix[i][j] = 0;
        }
    }

    Multiply(matrix1, matrix2, res_matrix, size);

    for (int i = 0, j;i < size;i++)
    {
        for (j = 0;j < size;j++)
        {
            result.append(std::to_string(res_matrix[i][j]));
            result.append(" ");
        }
        result.append("\r\n");
    }

    std::basic_string<TCHAR> converted(result.begin(), result.end());


    WriteSlot(hFile, converted.c_str());

    CloseHandle(hFile);

    return TRUE;
}
