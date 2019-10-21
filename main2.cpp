#include <Windows.h>
#include <iostream>
#include <string>


constexpr int32_t BUFFER_SIZE = 4096;
int32_t g_bytesTransferred;

const std::string getFilePrompt(const std::string& msg)
{
	std::string str;
	std::cout << msg;
	std::cin >> str;
	return str;
}

void FileIOCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransferred, LPOVERLAPPED pol)
{
	g_bytesTransferred = dwNumberOfBytesTransferred;
}

int main()
{
	const std::string originFile = "forcopy.png"; //getFilePrompt("File to copy: ");
	const std::string destFile = "copied.png"; //getFilePrompt("Destination: ");

	const HANDLE originFileHandle = CreateFileA(originFile.c_str(),
			                       GENERIC_READ,
			                       FILE_SHARE_READ | FILE_SHARE_WRITE,
			                       NULL,
			                       OPEN_EXISTING,
			                       FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
			                       NULL);

	if(originFileHandle == INVALID_HANDLE_VALUE)
	{
		std::cout << "Origin file is not opened. Last error code: " << GetLastError() << std::endl;
		return 0;
	}

	const HANDLE destFileHandle = CreateFileA(destFile.c_str(),
			                                      GENERIC_READ | GENERIC_WRITE,
			                                      FILE_SHARE_READ | FILE_SHARE_WRITE,
			                                      NULL,
			                                      CREATE_ALWAYS,
			                                      FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
			                                      NULL);

	if(destFileHandle == INVALID_HANDLE_VALUE)
	{
		std::cout << "Destination file is not opened. Last error code: " << GetLastError() << std::endl;
		return 0;
	}

	OVERLAPPED overlapped = {};
	char buffer[BUFFER_SIZE] = {};
	int32_t shift = 0;
	int32_t readBytes = 0;
	do
	{
		overlapped.Offset = shift * BUFFER_SIZE;
		if(!ReadFileEx(originFileHandle, buffer, BUFFER_SIZE, &overlapped, FileIOCompletionRoutine))
		{
			std::cout << "Error upon reading! Last error: " << GetLastError();
			break;
		}
		SleepEx(1000, TRUE);

		readBytes = g_bytesTransferred;
		if(readBytes < BUFFER_SIZE)
			memset(buffer + readBytes, '\0', BUFFER_SIZE - readBytes);

		if(!WriteFileEx(destFileHandle, buffer, BUFFER_SIZE, &overlapped, FileIOCompletionRoutine))
		{
			std::cout << "Error upon writing! Last error: " << GetLastError();
			break;
		}
		SleepEx(1000, TRUE);

		++shift;
	}while(readBytes >= BUFFER_SIZE);

	SetFilePointer(destFileHandle, (shift - 1) * BUFFER_SIZE + readBytes, NULL, FILE_BEGIN);
	SetEndOfFile(destFileHandle);
	CloseHandle(originFileHandle);
	CloseHandle(destFileHandle);

	std::cout << std::endl << "Running file comparator..." << std::endl;
	system((std::string("FC ") + originFile + std::string(" ") + destFile).c_str());
	return 0;
}
