#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "cldiy.h"

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

typedef void(*stdproc)(void);

#pragma pack(push, 1)

typedef struct
{
  uint32_t D[8];
  uint32_t A[7];
} SWSRegs;

typedef struct 
{
  uint8_t unused : 3;
  uint8_t skin : 2;
  uint8_t position : 3;
} SWSMultiPos;

typedef struct
{
  uint8_t second : 4;
  uint8_t first: 4;
} SWSAttributte;

typedef struct
{
  uint8_t type;
  uint8_t shirt_colA;
  uint8_t shirt_colB;
  uint8_t short_col;
  uint8_t socks_col;
} SWSKit;

typedef struct
{
  uint8_t nationality;
  uint8_t res0;
  uint8_t number;
  char name[23];
  SWSMultiPos position;
  uint8_t res1;
  SWSAttributte attXP;
  SWSAttributte attVH;
  SWSAttributte attTC;
  SWSAttributte attSF;
  uint8_t value;
  uint8_t careerdta[5];
} SWSPlayer;

typedef struct
{
  uint8_t nation;
  uint8_t number;
  uint16_t swsgenno;
  uint8_t res0;
  char name[19];
  uint8_t tactic;
  uint8_t division;
  SWSKit homekit;
  SWSKit awaykit;
  char coachname[24];
  uint8_t playerpos[16];
  SWSPlayer players[16];
} SWSTeam;

typedef struct
{
	int32_t offsett;
	int16_t matches;
	int16_t wins;
	int16_t draws;
	int16_t lost;
	int16_t goals_for;
	int16_t goals_again;
	int16_t points;
} SWSTableInfo;


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

const uint32_t concareermenu = 0x48194B9;
const uintptr_t continuecareerproc = 0x0E17F3;
const uintptr_t endcareerptr = 0x134E5F;


const uint8_t callopc = 0xe8;
const uint8_t nopopc = 0x90;
const uintptr_t compbuf = 0x47B5A82;
const uintptr_t teambuf = 0x4787277;
const uintptr_t curlbuf = 0x4785E30;
const uintptr_t curtbuf = 0x4787277;

const uintptr_t seasyear = 0x19FA14;
const uintptr_t curseasonno = 0x477D48A;


static bool currentcomp;
uint8_t* cldiydata;
uint16_t clteamcount;
SWSRegs* regs;


int comppts(const void* t1, const void* t2)
{
  SWSTableInfo* tr1 = (SWSTableInfo*)t1;
  SWSTableInfo* tr2 = (SWSTableInfo*)t2;
  return tr2->points - tr1->points;
}

int compdiff(const void* t1, const void* t2)
{
  SWSTableInfo* tr1 = (SWSTableInfo*)t1;
  SWSTableInfo* tr2 = (SWSTableInfo*)t2;
  return (tr2->goals_for - tr2->goals_again) - (tr1->goals_for - tr1->goals_again);
}

void ContinueCareer(void)
{
  stdproc contcar = (stdproc)(continuecareerproc + swshwnd);
  MessageBoxA(NULL, "Continue Career", "SavCar.ASI", MB_OK);
  cldiydata = (uint8_t*)malloc(champlgediy_size + (sizeof(SWSTeam) * 32));
  memcpy(cldiydata, champlgediy_data, champlgediy_size);
  clteamcount = 0;
  contcar();
}

uint32_t EndCareer(void)
{
  char filename[16];
  uint32_t* yearstart = (uint32_t*)(swshwnd + seasyear);
  uint16_t* actseason = (uint16_t*)(swshwnd + curseasonno);
  uint32_t season = (*actseason + *yearstart);
  sprintf(filename, "%s_%d.DIY", "CLTST", season);
  FILE* f = fopen(filename, "wb");
  fwrite(cldiydata, champlgediy_size + (sizeof(SWSTeam) * 32), 1, f);
  fclose(f);
  MessageBoxA(NULL, "End Career", "SavCar.ASI", MB_OK);
  return regs->A[0];
}


uint32_t SaveCurrent(void)
{
  char filename[32];
  SWSDIYInfo* comp = (SWSDIYInfo*)(swshwnd + curlbuf);
  int16_t* teamcount = (uint16_t*)(swshwnd + curlbuf + 0x31);
  SWSTableInfo* tablex = (SWSTableInfo*)(swshwnd + curlbuf + 0x2b3);
  uint32_t* yearstart = (uint32_t*)(swshwnd + seasyear);
  uint16_t* actseason = (uint16_t*)(swshwnd + curseasonno);
  uint32_t season = (*actseason + *yearstart);
  // Sorting
  SWSTableInfo* table = (SWSTableInfo*)calloc(*teamcount, sizeof(SWSTableInfo));
  memcpy(table, tablex, sizeof(SWSTableInfo)*(*teamcount));
  qsort(table, *teamcount, sizeof(SWSTableInfo), compdiff);
  qsort(table, *teamcount, sizeof(SWSTableInfo), comppts); 
  //
  CreateDirectory("careerlogs", NULL);
  sprintf(filename, "careerlogs\\%d_%s.txt", season, comp->name);
  FILE* f = fopen(filename, "w");
  fprintf(f, "CompName: %s\n", comp->name);
  fprintf(f, "Teams: %d\n", *teamcount);
  fprintf(f, "team;p;w;d;l;gf;ga;pts\n");
  // Check Champions League
  int selectorid = -1;
  SWSTeam* tteam = (SWSTeam*)(swshwnd + teambuf + table[0].offsett);
  for (int i =0; i < 43; i++)
  {
    if (nationsdta[i].nationid == tteam->nation)
    {
      if (tteam->division == 0)
        selectorid = i;
    }
  }
  int cl = 0;
  int clend = 0;
  if (selectorid > 0)
  {
    cl = nationsdta[selectorid].startcount;
    clend = nationsdta[selectorid].startcount + nationsdta[selectorid].teamscount;
  }
  for (int i = 0; i < *teamcount; i++)
  {
    SWSTeam* team = (SWSTeam*)(swshwnd + teambuf + table[i].offsett);
    //fprintf(f, "File: %02x, No: %02x, GenNo: %04x, %s\n", team->teamfileno, team->number, team->genswosno, team->name);
    fprintf(f, "%s;%d;%d;%d;%d;%d;%d;%d\n", team->name, table[i].matches, 
    table[i].wins, table[i].draws, table[i].lost, table[i].goals_for, table[i].goals_again, table[i].points);
    if (cl < clend)
    {
      memcpy((void*)(cldiydata + 0x1447 + (clteamcount * sizeof(SWSTeam))), team, sizeof(SWSTeam));
      uint16_t ids = (team->number) << 8 | team->nation; 
      memcpy((void*)(cldiydata + 0x99 + (clteamcount*2)), (void*)&ids, 2);
      memcpy((void*)(cldiydata + 0x54c + (clteamcount*2)), (void*)&ids, 2);
      clteamcount++;
    }
    cl++;
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
  SWSTableInfo* tablex = (SWSTableInfo*)(swshwnd + compbuf + 0x2b3);
  uint32_t* yearstart = (uint32_t*)(swshwnd + seasyear);
  uint16_t* actseason = (uint16_t*)(swshwnd + curseasonno);
  uint32_t season = (*actseason + *yearstart);
  // Sorting
  SWSTableInfo* table = (SWSTableInfo*)calloc(*teamcount, sizeof(SWSTableInfo));
  memcpy(table, tablex, sizeof(SWSTableInfo)*(*teamcount));
  qsort(table, *teamcount, sizeof(SWSTableInfo), compdiff);
  qsort(table, *teamcount, sizeof(SWSTableInfo), comppts); 
  //
  CreateDirectory("careerlogs", NULL);
  sprintf(filename, "careerlogs\\%d_%s.txt", season, comp->name);
  FILE* f = fopen(filename, "w");
  fprintf(f, "CompName: %s\n", comp->name);
  fprintf(f, "Teams: %d\n", *teamcount);
  fprintf(f, "team;p;w;d;l;gf;ga;pts\n");
  // Check Champions League
  int selectorid = -1;
  SWSTeam* tteam = (SWSTeam*)(swshwnd + teambuf + table[0].offsett);
  /*
  if (tteam->nation == 8)
  {
    char text[64];
    sprintf(text, "Team to test: %s, Division: %d", tteam->name, tteam->division);
    MessageBoxA(NULL, text, "TEAMTEST", MB_OK);
  }
  */
  for (int i =0; i < 43; i++)
  {
    if (nationsdta[i].nationid == tteam->nation)
    {
      if (tteam->division == 0)
        selectorid = i;
    }
  }
  int cl = 0;
  int clend = 0;
  if (selectorid > 0)
  {
    cl = nationsdta[selectorid].startcount;
    clend = nationsdta[selectorid].startcount + nationsdta[selectorid].teamscount;
  }
  /*
  if (tteam->nation == 8)
  {
    char text[64];
    sprintf(text, "England-> SelectorID: %d, Start: %d, End: %d", selectorid, cl, clend);
    MessageBoxA(NULL, text, "SWSASI", MB_OK);
  }
  */
  for (int i = 0; i < *teamcount; i++)
  {
    SWSTeam* team = (SWSTeam*)(swshwnd + teambuf + table[i].offsett);
    //fprintf(f, "File: %02x, No: %02x, GenNo: %04x, %s\n", team->teamfileno, team->number, team->genswosno, team->name);
    fprintf(f, "%s;%d;%d;%d;%d;%d;%d;%d\n", team->name, table[i].matches, 
    table[i].wins, table[i].draws, table[i].lost, table[i].goals_for, table[i].goals_again, table[i].points);
    if (cl < clend)
    {
      memcpy((void*)(cldiydata + 0x1447 + (clteamcount * sizeof(SWSTeam))), team, sizeof(SWSTeam));
      uint16_t ids = (team->number) << 8 | team->nation;
      memcpy((void*)(cldiydata + 0x99 + (clteamcount*2)), (void*)&ids, 2);
      memcpy((void*)(cldiydata + 0x54c + (clteamcount*2)), (void*)&ids, 2);
      clteamcount++;
    }
    cl++;
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
      // contcareer
    adr = swshwnd + concareermenu;
		VirtualProtect((LPVOID)adr, 4, dwNewProtect, &dwOrginalProtect);
    uintptr_t ccptr = (uintptr_t)&ContinueCareer;
    memcpy((void*)adr, &ccptr, 4);
    VirtualProtect((LPVOID)adr, 4, dwOrginalProtect, &dwNewProtect);
    // endcareer
    adr = swshwnd + endcareerptr;
		VirtualProtect((LPVOID)adr, 5, dwNewProtect, &dwOrginalProtect);
    memcpy((void*)adr, &callopc, 1);
    rel = ((uintptr_t)&EndCareer) - (adr + 5);
		memcpy((void*)(adr+1), &rel,  4);
    VirtualProtect((LPVOID)adr, 5, dwOrginalProtect, &dwNewProtect);
	}
	return TRUE;
}