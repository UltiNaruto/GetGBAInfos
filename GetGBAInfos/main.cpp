#include <stdio.h>
#include <conio.h>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <Windows.h>
#define GlobalSleep(x) Sleep(x)
#else
#include <unistd.h>
#define GlobalSleep(x) usleep(x*1000)
#endif


struct GBA_HEADER {
	unsigned int ROM_EP;
	char NINTENDO_LOGO[156];
	char GAME_NAME[12];
	char GAME_CODE[4];
	char VENDOR_CODE[2];
};

void StringCopy(char* dst, int len, char * src)
{
	for (int i = 0; i < len; i++)
	{
		dst[i] = src[i];
	}
}

const char * GetRegion(char type)
{
	if (type == 'P')
		return "Europe";
	else if (type == 'J')
		return "Japan";
	else if (type == 'E')
		return "USA/English";
	else if (type == 'F')
		return "French";
	else if (type == 'S')
		return "Spanish";
	else if (type == 'D')
		return "German";
	else if (type == 'I')
		return "Italian";
	else
		return NULL;
}

// credits to spacy51 from vba-m.com forum
const char * getSaveType(unsigned char * rom, int romSize)
{
	const int address_max = romSize - 10;
	char temp[11]; temp[10] = '\0';
	char * answer = "Undefined";

	const unsigned int EEPR = 'E' | ('E' << 8) | ('P' << 16) | ('R' << 24);
	const unsigned int SRAM = 'S' | ('R' << 8) | ('A' << 16) | ('M' << 24);
	const unsigned int FLAS = 'F' | ('L' << 8) | ('A' << 16) | ('S' << 24);

	for (int address = 0; address < address_max; address += 4) {
		const unsigned int check = *((unsigned int*)&rom[address]);

		if (EEPR == check) {
			memcpy(temp, &rom[address], 10);
			if (0 == strncmp(temp, "EEPROM_V", 8)) {
				answer = "EEPROM";
				break;
			}
		}

		if (SRAM == check) {
			memcpy(temp, &rom[address], 10);
			if ((0 == strncmp(temp, "SRAM_V", 6)) || (0 == strncmp(temp, "SRAM_F_V", 8))) {
				answer = "SRAM";
				break;
			}
		}

		if (FLAS == check) {
			memcpy(temp, &rom[address], 10);
			if ((0 == strncmp(temp, "FLASH_V", 7)) || (0 == strncmp(temp, "FLASH512_V", 10))) {
				answer = "FLASH (64 Kib)";
				break;
			}
			if (0 == strncmp(temp, "FLASH1M_V", 9)) {
				answer = "FLASH (128 KiB)";
				break;
			}
		}
	}
	return answer;
}

int main(int argc, char ** argv)
{
	if (argc == 2)
	{
		FILE * iFile;
		fopen_s(&iFile, argv[1], "rb");
		unsigned char datas[190] = { 0 };
		if (iFile != NULL)
		{
			fseek(iFile, 0L, SEEK_END);
			long fileSize = ftell(iFile);
			fseek(iFile, 0L, SEEK_SET);
			fread(datas, 190, 1, iFile);
			unsigned char * datas_copy = new unsigned char[fileSize];
			fseek(iFile, 0L, SEEK_SET);
			fread(datas_copy, fileSize, 1, iFile);
			GBA_HEADER * gbaHeader = (GBA_HEADER*)datas;
			char gameName[13] = { 0 };
			char gameCode[5] = { 0 };
			char vendorCode[3] = { 0 };
			StringCopy(gameName, 12, (char*)(&gbaHeader->GAME_NAME));
			StringCopy(gameCode, 4, (char*)(&gbaHeader->GAME_CODE));
			StringCopy(vendorCode, 2, (char*)(&gbaHeader->VENDOR_CODE));
			const char * region = GetRegion(gameCode[3]);
			printf("Game Name : %s\n", gameName);
			printf("Game Code : %s\n", gameCode);
			printf("Vendor Code : %s\n", vendorCode);
			printf("Region : %s\n", region);
			printf("Short Title : %c%c\n", gameCode[1], gameCode[2]);
			char * saveType = (char*)getSaveType(datas_copy, (int)fileSize);
			printf("Save Type : %s\n", saveType);
			while (!_kbhit())
				GlobalSleep(60);
			return 0;
		}
		else
		{
			printf("File not found\n");
			while (!_kbhit())
				GlobalSleep(60);
			return 1;
		}
	}
	else
	{
		printf("GetGBAInfos.exe <path_to_gba_file>");
		return 0;
	}
}