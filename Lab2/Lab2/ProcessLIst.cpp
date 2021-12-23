#include <cstddef>
#include <shlwapi.h>
#include <string>
#include "pch.h"


struct MY_PROCESS_INFO//структура в которой будет храниться инфа о процессе
{
	PROCESS_INFORMATION procInfo;//хранит id и handle процесса и его потока
	bool state;//активен процесс или завершён
	std::string name;//путь выбранного файла
};


template <typename dataType>
class Node//элемент списка
{
public:
	dataType data;
	Node<dataType>* next;
	Node(dataType d) :data(d)
	{
		next = NULL;
	}
};

template <typename dataType>
class ProcessList//обычный односвязный список, хранящий процессы
{
public:
	Node<dataType>* first;
	ProcessList() :first(NULL) {}
	void AddNode(dataType newData)
	{
		Node<dataType>* newNode = new Node<dataType>(newData);
		if (first == NULL)
		{
			first = newNode;
		}
		else
		{
			Node<dataType>* currentNode = first;
			while (currentNode->next != NULL)
			{
				currentNode = currentNode->next;
			}
			currentNode->next = newNode;
		}
	}
	void Draw(HDC hdc)//отобразить инфу о процессах
	{
		Node<dataType>* currentNode = first;
		std::wstring widestr;
		std::wstring buffer;
		LPCWSTR text;
		for (int i = 0;currentNode != NULL;i++)
		{
			buffer = std::to_wstring(i + 1);
			buffer.append(std::wstring(currentNode->data.name.begin(), currentNode->data.name.end()));
			widestr.assign(buffer.begin(), buffer.end());
			if (currentNode->data.state)
			{
				widestr.append(L"Выполняется");
			}
			else
			{
				widestr.append(L"Завершился");
			}
			text = widestr.c_str();
			TextOut(hdc, 300, 100+i*25, text, wcslen(text));
			currentNode = currentNode->next;
		}
	}
	BOOL GetInfo(PROCESS_INFORMATION* bufferInfo,int needNubmer)//Найти нужный процесс
	{
		Node<dataType>* currentNode = first;
		for (int i = 0;currentNode != NULL;i++)
		{
			if (i+1 == needNubmer)
			{
				currentNode->data.state = FALSE;
				*bufferInfo = currentNode->data.procInfo;
				return TRUE;
			}
			currentNode = currentNode->next;
		}
		return FALSE;
	}
	void Check()//Проверка активности процессов
	{
		Node<dataType>* currentNode = first;
		DWORD dwWaitResult;
		while (currentNode != NULL)
		{
			if (currentNode->data.state)
			{
				dwWaitResult = WaitForSingleObject(
					currentNode->data.procInfo.hProcess,
					5);
				switch (dwWaitResult)
				{
				case WAIT_OBJECT_0://The state of the specified object is signaled. 
					currentNode->data.state = FALSE;
					break;
				case WAIT_TIMEOUT://The time-out interval elapsed, and the object's state is nonsignaled. 
					
					break;
				case WAIT_FAILED://The function has failed. To get extended error information, call GetLastError. 
					currentNode->data.state = FALSE;
					break;
				case WAIT_ABANDONED:
					/*The specified object is a mutex object that was not released by the thread that owned the mutex object
					before the owning thread terminated.Ownership of the mutex object is granted to the calling threadand the
					mutex state is set to nonsignaled.
					If the mutex was protecting persistent state information, you should check it for consistency.*/
					currentNode->data.state = FALSE;
					break;
				}
			}
			currentNode = currentNode->next;
		}
	}
private:
};