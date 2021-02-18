#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

/*
Copyright <2021> <AnoXic>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma pack(push, 1)

typedef struct
{
  uint32_t D[7];
  uint32_t A[6];
} SWSRegs;

typedef struct
{
	uint32_t offsett;
	uint16_t matches;
	uint16_t wins;
	uint16_t draws;
	uint16_t lost;
	uint16_t goals_for;
	uint16_t goals_again;
	uint16_t points;
} SWSTableInfo;

typedef struct
{
	uint8_t teamfileno;
	uint8_t number;
	uint16_t genswosno;
	uint8_t playersel;
	char name[19];
} SWSTeamInfo;

typedef struct
{
  uint16_t type;
  uint16_t res0;
  char name[24];
} SWSDIYInfo;

#pragma pack(pop)

uintptr_t swshwnd;

const uintptr_t curlgeptr = 0x13980D;
const uintptr_t savlgeptr = 0x1398E4;

const uint8_t callopc = 0xe8;
const uint8_t nopopc = 0x90;
const uintptr_t compbuf = 0x47B5A82;
const uintptr_t teambuf = 0x4787277;
const uintptr_t curlbuf = 0x4785E30;
const uintptr_t curtbuf = 0x4787277;

const uintptr_t seasyear = 0x19FA14;
const uintptr_t curseasonno = 0x477D48A;


static bool currentcomp;
SWSRegs* regs;

uint32_t SaveCurrent(void)
{
  char filename[32];
  SWSDIYInfo* comp = (SWSDIYInfo*)(swshwnd + curlbuf);
  uint16_t* teamcount = (uint16_t*)(swshwnd + curlbuf + 0x31);
  SWSTableInfo* table = (SWSTableInfo*)(swshwnd + curlbuf + 0x2b3);
  uint32_t* yearstart = (uint32_t*)(swshwnd + seasyear);
  uint16_t* actseason = (uint16_t*)(swshwnd + curseasonno);
  uint32_t season = (*actseason + *yearstart);
  CreateDirectory("careerlogs", NULL);
  sprintf(filename, "careerlogs\\%d_%s.txt", season, comp->name);
  FILE* f = fopen(filename, "w");
  fprintf(f, "CompName: %s\n", comp->name);
  fprintf(f, "Teams: %d\n", *teamcount);
  fprintf(f, "team;p;w;d;l;gf;ga;pts\n");
  for (int i = 0; i < *teamcount; i++)
  {
    SWSTeamInfo* team = (SWSTeamInfo*)(swshwnd + teambuf + table[i].offsett);
    //fprintf(f, "File: %02x, No: %02x, GenNo: %04x, %s\n", team->teamfileno, team->number, team->genswosno, team->name);
    fprintf(f, "%s;%d;%d;%d;%d;%d;%d;%d\n", team->name, table[i].matches, 
    table[i].wins, table[i].draws, table[i].lost, table[i].goals_for, table[i].goals_again, table[i].points);
  }
  //MessageBoxA(NULL, "Stop To Test", "SWSCar", MB_OK | MB_ICONINFORMATION);
  fclose(f);
  return regs->D[7];
}

uint16_t SaveLge(void)
{
  char filename[32];
  SWSDIYInfo* comp = (SWSDIYInfo*)(swshwnd + compbuf);
  uint16_t* teamcount = (uint16_t*)(swshwnd + compbuf + 0x31);
  SWSTableInfo* table = (SWSTableInfo*)(swshwnd + compbuf + 0x2b3);
  uint32_t* yearstart = (uint32_t*)(swshwnd + seasyear);
  uint16_t* actseason = (uint16_t*)(swshwnd + curseasonno);
  uint32_t season = (*actseason + *yearstart);
  CreateDirectory("careerlogs", NULL);
  sprintf(filename, "careerlogs\\%d_%s.txt", season, comp->name);
  FILE* f = fopen(filename, "w");
  fprintf(f, "CompName: %s\n", comp->name);
  fprintf(f, "Teams: %d\n", *teamcount);
  fprintf(f, "team;p;w;d;l;gf;ga;pts\n");
  for (int i = 0; i < *teamcount; i++)
  {
    SWSTeamInfo* team = (SWSTeamInfo*)(swshwnd + teambuf + table[i].offsett);
    //fprintf(f, "File: %02x, No: %02x, GenNo: %04x, %s\n", team->teamfileno, team->number, team->genswosno, team->name);
    fprintf(f, "%s;%d;%d;%d;%d;%d;%d;%d\n", team->name, table[i].matches, 
    table[i].wins, table[i].draws, table[i].lost, table[i].goals_for, table[i].goals_again, table[i].points);
  }
  //MessageBoxA(NULL, "Stop To Test", "SWSCar", MB_OK | MB_ICONINFORMATION);
  fclose(f);
  return regs->D[7];
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
    swshwnd = (uintptr_t)GetModuleHandle(NULL);
    regs = (SWSRegs*)(0x4797DA7 + swshwnd);
    DWORD dwOrginalProtect;
		DWORD dwNewProtect = PAGE_EXECUTE_READWRITE;
    // curcar
    uintptr_t adr = swshwnd + curlgeptr;
    VirtualProtect((LPVOID)adr, 6, dwNewProtect, &dwOrginalProtect);
    memcpy((void*)adr, &callopc, 1);
    uint32_t rel = ((uintptr_t)&SaveLge) - (adr + 5);
		memcpy((void*)(adr+1), &rel,  4);
		memcpy((void*)(adr+5), &nopopc, 1);
    VirtualProtect((LPVOID)adr, 6, dwOrginalProtect, &dwNewProtect);
    // savcar
    adr = swshwnd + savlgeptr;
		VirtualProtect((LPVOID)adr, 6, dwNewProtect, &dwOrginalProtect);
    memcpy((void*)adr, &callopc, 1);
    rel = ((uintptr_t)&SaveLge) - (adr + 5);
		memcpy((void*)(adr+1), &rel,  4);
		memcpy((void*)(adr+5), &nopopc, 1);
    VirtualProtect((LPVOID)adr, 6, dwOrginalProtect, &dwNewProtect);
	}
	return TRUE;
}