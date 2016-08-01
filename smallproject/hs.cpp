// hs.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#define BUFFERSIZE 10240

unsigned char delta[16] = { 20, 20, 16, 16, 20, 20, 16, 16, 4, 4, 0, 0, 4, 4, 0, 0};
unsigned char buffer[BUFFERSIZE];

bool translate(char *filename) {
	FILE *fp;
	int readed, havereaded = 0;
	fopen_s(&fp, filename, "rb+");
	if (!fp)
		return false;
	do {
		fseek(fp, havereaded, SEEK_SET);
		readed = fread_s(buffer, BUFFERSIZE, 1, BUFFERSIZE, fp);
		for (int i = 0; i < readed; i++) {
			if (buffer[i] > 127) {
				buffer[i] = buffer[i] - 138 + delta[buffer[i] % 16];
			}
			else {
				buffer[i] = buffer[i] + 118 + delta[buffer[i] % 16];
			}
		}
		fseek(fp, havereaded, SEEK_SET);
		readed = fwrite(buffer, 1, readed, fp);
		havereaded += readed;

	} while (readed == BUFFERSIZE);
	fclose(fp);
	return true;
}

int main(int argc, char **argv) {
	WIN32_FIND_DATA findFileData;
	char path[MAX_PATH * 2];
	bool bRet;
	if (argc != 2) {
		printf("Translate .ec file\r\n    Such as : hs.exe *.ec\r\n!!! All files will changed, backup your files at first !!!\r\n");
		return 0;
	}
	HANDLE hFind = ::FindFirstFile(argv[1], &findFileData);
	strncpy(path, argv[1], MAX_PATH);
	char *pathend = path;
	for (int i = strlen(argv[1]); i > 0; i--) {
		if (path[i] == '\\') {
			pathend = &path[i+1];
			break;
		}
	}
	if (INVALID_HANDLE_VALUE == hFind) {
		return -1;
	}
	while (TRUE)
	{
		strncpy(pathend, findFileData.cFileName, MAX_PATH);
		bRet = translate(path);
		if (bRet) {
			printf("Translate %s\r\n", path);
		}
		bRet = ::FindNextFile(hFind, &findFileData);
		if (!bRet) {
			break;
		}
	}
	::FindClose(hFind);
}
