#include "Task11.h"
#include "ConsoleMenu.h"
#include <iostream>
#include <sstream>

void Task11::diskList()
{
	std::cout << "Drives: " << GetLogicalDrives() << " Bit mask: "<< std::endl;
	printBitMask<DWORD>(GetLogicalDrives());
	std::cout << std::endl;

	std::cout << "Drives' names: " << std::endl;
	const uint32_t bSize = 64;
	auto buffer = new WCHAR[bSize];
	DWORD written = GetLogicalDriveStringsW(bSize, buffer);
	for(const std::wstring& name : getParsedNames(buffer, written))
		std::wcout << name << std::endl;
	delete[] buffer;
}

void Task11::diskInfo()
{
	std::wstring driveName;
	std::cout << "Drive's name: ";
	std::wcin >> driveName;

	std::cout << std::endl;
	const DWORD driveType = GetDriveTypeW(driveName.c_str());
	std::wcout << "Drive type: " << driveTypeCodeToString(driveType) << std::endl;
	if(driveType == 0) return;

	const DWORD volNameSize = 64, fileSystemNameSize = 64;
	auto volNameBuffer = new WCHAR[volNameSize],
			 fileSystemNameBuffer = new WCHAR[fileSystemNameSize];
	DWORD volSerialNumber,
				maxComponentLength,
				fileSystemFlags;

	if(!GetVolumeInformationW(driveName.c_str(), volNameBuffer, volNameSize,
			                     &volSerialNumber, &maxComponentLength, &fileSystemFlags,
			                     fileSystemNameBuffer, fileSystemNameSize))
	{
		std::cout << "Not all requested fields are retrieved!" << std::endl;
		return;
	}

	std::cout << std::endl;
	std::wcout << L"Volume name: " << volNameBuffer << std::endl
						 << L"Volume serial number: " << volSerialNumber << std::endl
						 << L"Max file name length: " << maxComponentLength << std::endl
						 << L"File system flags: " << std::hex << fileSystemFlags << std::dec << std::endl
						 << L"File system name: " << fileSystemNameBuffer << std::endl;

	DWORD sectorsPerCluster,
				bytesPerSector,
				nOfFreeClusters,
				totalNOfClusters;

	if(!GetDiskFreeSpaceW(driveName.c_str(), &sectorsPerCluster, &bytesPerSector,
			                  &nOfFreeClusters, &totalNOfClusters))
	{
		std::cout << "Not all requested info received!" << std::endl;
		return;
	}

	std::cout << std::endl;
	std::cout << "Sectors per cluster: " << sectorsPerCluster << std::endl
						<< "Bytes per cluster: " << bytesPerSector << std::endl
						<< "Number of free clusters: " << nOfFreeClusters << std::endl
						<< "Total number of clusters: " << totalNOfClusters << std::endl;

	delete[] volNameBuffer;
	delete[] fileSystemNameBuffer;

}

void Task11::createRemoveDir()
{
	ConsoleMenu<std::string> menu;
	menu.addOption("Create directory", [](){
		std::wstring dirName;
		std::cout << "Directory name to create: ";
		std::wcin >> dirName;
		if(!CreateDirectoryW(dirName.c_str(), NULL))
			std::wcout << "Directory '" << dirName << "' was not created!" << std::endl;
	});
	menu.addOption("Remove directory", [](){
		std::wstring dirName;
		std::cout << "Directory name to remove: ";
		std::wcin >> dirName;
		if(!RemoveDirectoryW(dirName.c_str()))
			std::wcout << "Directory '" << dirName << "' was not created!" << std::endl;
	});

	menu.exec();
}

void Task11::createFile()
{
	std::wstring filename;
	std::cout << "File name: ";
	std::wcin >> filename;
	const HANDLE hFile = CreateFileW(filename.c_str(), GENERIC_READ | GENERIC_WRITE,
			                0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		std::cout << "File has not been created!" << std::endl;
	// CloseHandle(hFile);
}

void Task11::copyMoveFile()
{
	ConsoleMenu<std::string> menu;
	menu.addOption("Copy file", [](){
		std::wstring existingFilename, newFilename;
		std::cout << "File to copy: ";
		std::wcin >> existingFilename;
		std::cout << "New file: ";
		std::wcin >> newFilename;

		if(!CopyFileW(existingFilename.c_str(), newFilename.c_str(), TRUE))
		{
			if(GetLastError() == ERROR_FILE_NOT_FOUND)
				std::cout << "File is not found!" << std::endl;
			else
				std::cout << "File with that name already exists" << std::endl;
		}

	});
	menu.addOption("Move file", [](){
		 moveFileDialog();
	});

	menu.exec();
}

void Task11::changeFileAttr()
{
	ConsoleMenu<std::string> menu;
	menu.addOption("Show file attributes: ", fileAttrsDialog);
	menu.addOption("Change file attributes: ", setFileAttrsDialog);

	menu.exec();
}

std::vector<std::wstring> Task11::getParsedNames(const WCHAR *buffer, const DWORD length)
{
	if(!length) return {};

	std::vector<std::wstring> result;
	size_t curLenght = 0;
	while(curLenght < length)
	{
		result.emplace_back(buffer);
		curLenght += wcslen(buffer) + 1;
		buffer += wcslen(buffer) + 1;
	}

	return result;
}

std::wstring Task11::driveTypeCodeToString(const uint32_t code)
{
	switch(code)
	{
		case 0:
			return L"Unknown drive type";
		case 1:
			return L"Root path is invalid";
		case 2:
			return L"Removable drive";
		case 3:
			return L"Fixed drive";
		case 4:
			return L"Remote drive";
		case 5:
			return L"CD-ROM drive";
		case 6:
			return L"RAM disk";
		default:
			return {};
	}
}

void Task11::moveFileDialog()
{
	std::wstring existingFilename, newFilename;
	std::cout << "File to move: ";
	std::wcin >> existingFilename;
	std::cout << "New filename: ";
	std::wcin >> newFilename;

	if(!MoveFileW(existingFilename.c_str(), newFilename.c_str()))
	{
		if(GetLastError() == ERROR_FILE_NOT_FOUND)
			std::cout << "File is not found!" << std::endl;
		else
		{
			bool prompt = false;
			std::cout << "File already exists. Force move? (1/0): ";
			std::cin >> prompt;
			if(prompt)
				MoveFileExW(existingFilename.c_str(), newFilename.c_str(), MOVEFILE_REPLACE_EXISTING);
		}
	}
}

void Task11::fileAttrsDialog()
{
	std::wstring filename;
	std::cout << "File: ";
	std::wcin >> filename;
	std::cout << std::endl;

	const HANDLE hFile = CreateFileW(filename.c_str(), NULL, FILE_SHARE_READ, NULL,
			                             OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	BY_HANDLE_FILE_INFORMATION info;
	if(!GetFileInformationByHandle(hFile, &info))
	{
		std::cout << "Cannot get info by handle!";
		return;
	}

	printFileInfo(info);
	// CloseHandle(hFile);
}

void Task11::setFileAttrsDialog()
{
	std::wstring filename;
	std::cout << "File: ";
	std::wcin >> filename;
	std::cout << std::endl;

	const HANDLE hFile = CreateFileW(filename.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL,
																	 OPEN_EXISTING, FILE_ATTRIBUTE_READONLY | FILE_WRITE_ATTRIBUTES, NULL);
	const DWORD attrs = fillFileAttrsDialog();
	const std::vector<FILETIME> filetimes = fillFileTimeDialog();

	if(!SetFileAttributesW(filename.c_str(), attrs) ||
	   !SetFileTime(hFile, &filetimes[0], &filetimes[1], &filetimes[2]))
		std::cout << "Something went wrong upon setting attributes" << std::endl;
	// CloseHandle(hFile);
}

void Task11::printDecodedAttributes(const DWORD attrs)
{
	std::cout << "Archive: " << static_cast<bool>(attrs & FILE_ATTRIBUTE_ARCHIVE) << std::endl;
	std::cout << "Compressed: " << static_cast<bool>(attrs & FILE_ATTRIBUTE_COMPRESSED) << std::endl;
	std::cout << "Directory: " << static_cast<bool>(attrs & FILE_ATTRIBUTE_DIRECTORY) << std::endl;
	std::cout << "Encrypted: " << static_cast<bool>(attrs & FILE_ATTRIBUTE_ENCRYPTED) << std::endl;
	std::cout << "Hidden: " << static_cast<bool>(attrs & FILE_ATTRIBUTE_HIDDEN) << std::endl;
	std::cout << "Read only: " << static_cast<bool>(attrs & FILE_ATTRIBUTE_READONLY) << std::endl;
	std::cout << "Not fully present locally: " << static_cast<bool>(attrs & FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS) << std::endl;
	std::cout << "Reparse point: " << static_cast<bool>(attrs & FILE_ATTRIBUTE_REPARSE_POINT) << std::endl;
	std::cout << "Sparse: " << static_cast<bool>(attrs & FILE_ATTRIBUTE_SPARSE_FILE) << std::endl;
	std::cout << "Temporary: " << static_cast<bool>(attrs & FILE_ATTRIBUTE_TEMPORARY) << std::endl;
}

void Task11::printFileInfo(const BY_HANDLE_FILE_INFORMATION& info)
{
	printDecodedAttributes(info.dwFileAttributes);
	std::cout << "Volume serial number: " << info.dwVolumeSerialNumber << std::endl;
	std::cout << "Number of links: " << info.nNumberOfLinks << std::endl;
	std::cout << "High order index: " << info.nFileIndexHigh << std::endl;
	std::cout << "Low order index: " << info.nFileIndexLow << std::endl;
	std::cout << "Time created: " << getReadableFileTime(info.ftCreationTime).c_str() << std::endl;
	std::cout << "Time written: " << getReadableFileTime(info.ftLastWriteTime).c_str() << std::endl;
	std::cout << "Time accessed: " << getReadableFileTime(info.ftLastAccessTime).c_str() << std::endl;
}

std::string Task11::getReadableFileTime(const FILETIME& time)
{
	SYSTEMTIME sysTime;
	FileTimeToSystemTime(&time, &sysTime);

	std::stringstream readableTime;
	readableTime << sysTime.wDay << "." << sysTime.wMonth << "." << sysTime.wYear << " T "
							 << sysTime.wHour << ":" << sysTime.wMinute << ":" << sysTime.wSecond << "." << sysTime.wMilliseconds;
	return readableTime.str();
}

const DWORD Task11::fillFileAttrsDialog()
{
	DWORD result = 0;
	bool answer = false;

	std::cout << "Make file hidden? (1/0): ";
	std::cin >> answer;
	if(answer)
		result |= FILE_ATTRIBUTE_HIDDEN;

	std::cout << "Make file read only? (1/0): ";
	std::cin >> answer;
	if(answer)
		result |= FILE_ATTRIBUTE_READONLY;

	return result;
}

const std::vector<FILETIME> Task11::fillFileTimeDialog()
{
	// \todo dates input will be written someday!
	return {{0, 0}, {0, 1}, {0, 2}};
}
