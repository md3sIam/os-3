#ifndef INC_3_TASK11_H
#define INC_3_TASK11_H

#include <Windows.h>
#include <vector>
#include <string>

class Task11
{
public:
	Task11() = delete;
	static void diskList();
	static void diskInfo();
	static void createRemoveDir();
	static void createFile();
	static void copyMoveFile();
	static void changeFileAttr();

private:
	static std::vector<std::wstring> getParsedNames(const WCHAR* buffer, const DWORD length);
	static std::wstring driveTypeCodeToString(const uint32_t code);
	static void moveFileDialog();
	static void fileAttrsDialog();
	static void setFileAttrsDialog();
	static void printDecodedAttributes(const DWORD attrs);
	static void printFileInfo(const BY_HANDLE_FILE_INFORMATION& info);
	static std::string getReadableFileTime(const FILETIME& time);
	static const DWORD fillFileAttrsDialog();
	static const std::vector<FILETIME> fillFileTimeDialog();
	template <class T> static void printBitMask(const T& var)
	{
	 	auto buffer = new char[sizeof(T)];
		memcpy(buffer, &var, sizeof(T));
		for(int32_t i = 8 * sizeof(T) - 1; i >= 0; --i)
		{
			const uint8_t shift = i % 8;
			const uint32_t index = i / 8;
			std::cout << ((buffer[index] >> shift) & static_cast<char>(1));
		}
		delete[] buffer;
	};
};

#endif //INC_3_TASK11_H
