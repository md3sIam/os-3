#include <Windows.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <ctime>

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

void copyFile(const std::string& file, const std::string& dest, const int32_t bufferSize, const int32_t threadsAmount)
{
	std::vector<char*>buffers;
	for(size_t i = 0; i < threadsAmount; ++i)
		buffers.push_back(new char[bufferSize]);

	const HANDLE originFileHandle = CreateFileA(file.c_str(),
																							GENERIC_READ,
																							FILE_SHARE_READ | FILE_SHARE_WRITE,
																							NULL,
																							OPEN_EXISTING,
																							FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
																							NULL);

	if(originFileHandle == INVALID_HANDLE_VALUE)
	{
		std::cout << "Origin file is not opened. Last error code: " << GetLastError() << std::endl;
		return;
	}

	const HANDLE destFileHandle = CreateFileA(dest.c_str(),
																						GENERIC_READ | GENERIC_WRITE,
																						FILE_SHARE_READ | FILE_SHARE_WRITE,
																						NULL,
																						CREATE_ALWAYS,
																						FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
																						NULL);

	if(destFileHandle == INVALID_HANDLE_VALUE)
	{
		std::cout << "Destination file is not opened. Last error code: " << GetLastError() << std::endl;
		return;
	}

	int32_t shift = 0;
	int32_t readBytes = 0;
	int32_t lastBuffer = -1;
	int32_t readLast = -1;
	std::vector<OVERLAPPED> overlappeds(threadsAmount);

	auto t0 = clock();
	do
	{
		for(size_t i = 0; i < threadsAmount; ++i)
		{
			overlappeds[i].Offset = (shift + i) * bufferSize;
			if(!ReadFileEx(originFileHandle, buffers[i], bufferSize, &overlappeds[i], FileIOCompletionRoutine))
			{
				std::cout << "Error upon reading! Last error: " << GetLastError();
				break;
			}
			SleepEx(1000, TRUE);
			if(lastBuffer == -1 && g_bytesTransferred != bufferSize)
			{
				lastBuffer = i;
				readLast = g_bytesTransferred;
			}
		}

		for(size_t i = 0; i < threadsAmount; ++i)
		{
			if(lastBuffer == -1 || lastBuffer >= i)
			{
				overlappeds[i].Offset = (shift + i) * bufferSize;
				if(!WriteFileEx(destFileHandle, buffers[i], bufferSize, &overlappeds[i], FileIOCompletionRoutine))
				{
					std::cout << "Error upon writing! Last error: " << GetLastError();
					break;
				}
				SleepEx(1000, TRUE);
			}
		}

		shift += threadsAmount;
	}while(lastBuffer == -1);
	auto t1 = clock();

	const int32_t fileSize = (shift - threadsAmount + lastBuffer) * bufferSize + readLast;
	SetFilePointer(destFileHandle, fileSize, NULL, FILE_BEGIN);
	SetEndOfFile(destFileHandle);
	CloseHandle(originFileHandle);
	CloseHandle(destFileHandle);

	std::cout << "Buffer size: " << std::setw(5) << bufferSize << "B " << std::setw(0)
						<< "Threads: " << std::setw(2) << threadsAmount << std::setw(0)
						<< " Speed: "<< (double)fileSize / 1024 / ((double)(t1 - t0) / CLOCKS_PER_SEC) << "KB/s" << std::endl;

	for(char* buffer : buffers)
		delete[] buffer;

	std::cout << std::endl << "Running file comparator..." << std::endl;
	system((std::string("FC ") + file + std::string(" ") + dest).c_str());
}

int main()
{
	constexpr int32_t clusterSize = 512;

	std::vector<int32_t> bufferSizes{clusterSize};
	for(int i = 1; i <= 10; ++i)
		bufferSizes.push_back(bufferSizes[bufferSizes.size() - 1] * 2);

	std::vector<int32_t> threadsAmounts{1, 2, 4, 8, 12, 16};

	const std::string originFile = getFilePrompt("File to copy: ");
	const std::string destFile = getFilePrompt("Destination: ");

	// checking for the best block size
	for(const int32_t bufferSize : bufferSizes)
		copyFile(originFile, destFile, bufferSize, 1);

	// checking for the best threads amount
	for(const int32_t threadsAmount : threadsAmounts)
		copyFile(originFile, destFile, 4096, threadsAmount);

	system("pause");
	return 0;
}
