#include "windows.h"
#include "iostream"
#include "Shlwapi.h"
#pragma comment(lib, "Shlwapi.lib")

namespace ConsoleForeground {
	enum {
		BLACK = 0,
		DARKBLUE = FOREGROUND_BLUE,
		DARKGREEN = FOREGROUND_GREEN,
		DARKCYAN = FOREGROUND_GREEN | FOREGROUND_BLUE,
		DARKRED = FOREGROUND_RED,
		DARKMAGENTA = FOREGROUND_RED | FOREGROUND_BLUE,
		DARKYELLOW = FOREGROUND_RED | FOREGROUND_GREEN,
		DARKGRAY = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
		GRAY = FOREGROUND_INTENSITY,
		BLUE = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
		GREEN = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
		CYAN = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
		RED = FOREGROUND_INTENSITY | FOREGROUND_RED,
		MAGENTA = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
		YELLOW = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
		WHITE = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	};
}


bool truncate(const char* PathName, unsigned long long Size) {
	HANDLE hFile = CreateFileA(PathName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		//printf("\n can't open %s (%d)", PathName, GetLastError());
		return false;
	}
	bool Success = false;
	LARGE_INTEGER NewSize = { 0 }; // Negative values move the pointer backward in the file
	NewSize.QuadPart = Size;
	Success = (SetFilePointerEx(hFile, NewSize, NULL, FILE_BEGIN) && SetEndOfFile(hFile));
	
	CloseHandle(hFile);
	return Success;
}

int main(int argc, char* argv[])
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	GetConsoleScreenBufferInfo(hStdout, &csbiInfo);
	WORD wOldColorAttrs = csbiInfo.wAttributes;;

	std::string path;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAA   FindFileData;
	
	if (argc < 2) {
		char name[MAX_PATH];
		GetModuleFileNameA(NULL, name, MAX_PATH);
		SetConsoleTextAttribute(hStdout, ConsoleForeground::GREEN);
		std::cout << "BURST plots checker (version 1.0)\nChecking and validation plots for BURST";
		SetConsoleTextAttribute(hStdout, ConsoleForeground::BLUE);
		std::cout << "\t [Programming: Blago]";
		SetConsoleTextAttribute(hStdout, ConsoleForeground::DARKGREEN);
		std::cout << "\nUsage:   " << PathFindFileNameA(name) << " [PATH]";
		std::cout << "\nExample:   " << PathFindFileNameA(name) << " d:\\plots";
		SetConsoleTextAttribute(hStdout, wOldColorAttrs);
		exit(0);
	}
	else{
		path = std::string(argv[1]);
		if (path.rfind("\\") < path.length() - 1) path += "\\";

		hFile = FindFirstFileA(LPCSTR((path + "*").c_str()), &FindFileData);
		if (INVALID_HANDLE_VALUE != hFile)	{
			do	{
				if (FILE_ATTRIBUTE_DIRECTORY & FindFileData.dwFileAttributes) continue; //Skip directories
				char* ekey = strstr(FindFileData.cFileName, "_");
				if (ekey != nullptr)
				{
					char* estart = strstr(ekey + 1, "_");
					if (estart != nullptr)
					{
						char* enonces = strstr(estart + 1, "_");
						if (enonces != nullptr)
						{
							unsigned long long key, nonce, nonces, stagger, file_size;;
							if (sscanf_s(FindFileData.cFileName, "%llu_%llu_%llu_%llu", &key, &nonce, &nonces, &stagger) == 4)
							{
								file_size = (((static_cast<ULONGLONG>(FindFileData.nFileSizeHigh) << (sizeof(FindFileData.nFileSizeLow) * 8)) | FindFileData.nFileSizeLow));
								if (file_size == nonces * 4096 * 64) {
									std::cout << "\nfile: " << FindFileData.cFileName;
									SetConsoleTextAttribute(hStdout, ConsoleForeground::GREEN);
									std::cout << "\tchecked - OK";
									SetConsoleTextAttribute(hStdout, wOldColorAttrs);
								}
								else {
									if (nonces == stagger) {
										std::cout << "\nfile: " << FindFileData.cFileName;
										SetConsoleTextAttribute(hStdout, ConsoleForeground::RED);
										std::cout << "\tneed to delete and replot"; // incomplete optimized plot
										SetConsoleTextAttribute(hStdout, wOldColorAttrs);
									}
									else{
										// rename file
										unsigned long long new_nonces = file_size / 4096 / 64;
										new_nonces = (new_nonces / stagger) * stagger; // ajusting by stagger
										char new_name[MAX_PATH];
										sprintf_s(new_name, MAX_PATH, "%llu_%llu_%llu_%llu", key, nonce, new_nonces, stagger);

										if (rename((path + FindFileData.cFileName).c_str(), (path + new_name).c_str()) == 0) {
											std::cout << "\nfile: " << FindFileData.cFileName;
											SetConsoleTextAttribute(hStdout, ConsoleForeground::YELLOW);
											std::cout << "\trenamed to " << new_name;
											SetConsoleTextAttribute(hStdout, wOldColorAttrs);
											// truncate file
											if (file_size != new_nonces * 4096 * 64) {
												if (truncate((path + new_name).c_str(), new_nonces * 4096 * 64)){
													std::cout << "\nfile: " << new_name;
													SetConsoleTextAttribute(hStdout, ConsoleForeground::BLUE);
													std::cout << "\ttruncated to " << new_nonces * 4096 * 64 << " bytes";
													SetConsoleTextAttribute(hStdout, wOldColorAttrs);
												}
												else { //error
													SetConsoleTextAttribute(hStdout, ConsoleForeground::RED);
													std::cout << "\nERROR for truncate file: " << new_name << " to " << new_nonces * 4096 * 64 << " bytes";
													SetConsoleTextAttribute(hStdout, wOldColorAttrs);
												}
											}
										}
										else { //error
											SetConsoleTextAttribute(hStdout, ConsoleForeground::RED);
											std::cout << "\nERROR for renaming file: " << FindFileData.cFileName << " to " << new_name;
											SetConsoleTextAttribute(hStdout, wOldColorAttrs);
										}
									}
								}
							}
						}
					}
				}
			} while (FindNextFileA(hFile, &FindFileData));
			FindClose(hFile);
		}
	}
	return 0;
}

