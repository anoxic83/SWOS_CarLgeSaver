#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
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
  uint8_t playerspos[28];
  uint32_t unkn;
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
  SWSPlayer players[28];
} SWSCarTeam;

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

const uintptr_t carrerfilebuffer = 0x476FEC6;
const uintptr_t ownteamincareer = carrerfilebuffer + 0xDB60;

static bool currentcomp;
static bool careerstart;
uint8_t* cldiydata;
uint16_t clteamcount;
SWSTeam* clteams;
uint16_t clqualifycount;
SWSTeam* clqualify;
bool clownteam;
bool clownqualify;
SWSRegs* regs;


int comppts(const void* t1, const void* t2)
{
  SWSTableInfo* tr1 = (SWSTableInfo*)t1;
  SWSTableInfo* tr2 = (SWSTableInfo*)t2;
  if (tr2->points == tr1->points)
  {
    if ((tr2->goals_for - tr2->goals_again) == (tr1->goals_for - tr1->goals_again))
      return tr2->goals_for - tr1->goals_for;
    else
      return (tr2->goals_for - tr2->goals_again) - (tr1->goals_for - tr1->goals_again);
  }
  return tr2->points - tr1->points;
}

int compdiff(const void* t1, const void* t2)
{
  SWSTableInfo* tr1 = (SWSTableInfo*)t1;
  SWSTableInfo* tr2 = (SWSTableInfo*)t2;
  return (tr2->goals_for - tr2->goals_again) - (tr1->goals_for - tr1->goals_again);
}

int comprand(const void* t1, const void* t2)
{
  (void)t1; (void)t2; 
  return rand() % 2 ? +1 : -1;
}

void ContinueCareer(void)
{
  stdproc contcar = (stdproc)(continuecareerproc + swshwnd);
  //MessageBoxA(NULL, "Continue Career", "SavCar.ASI", MB_OK);
  cldiydata = (uint8_t*)malloc(champlgediy_size + (sizeof(SWSTeam) * 32));
  memset(cldiydata, 0 ,champlgediy_size + (sizeof(SWSTeam) * 32));
  memcpy(cldiydata, champlgediy_data, champlgediy_size);
  clteamcount = 0;
  clqualifycount = 0;
  clownteam = false;
  clownqualify = false;
  clteams = (SWSTeam*)calloc(32, sizeof(SWSTeam));
  clqualify = (SWSTeam*)calloc(32, sizeof(SWSTeam));
  careerstart = true;
  contcar();
}

uint32_t EndCareer(void)
{
  if (careerstart == true)
  {
    srand(time(NULL));
    char filename[16];
    uint32_t* yearstart = (uint32_t*)(swshwnd + seasyear);
    uint16_t* actseason = (uint16_t*)(swshwnd + curseasonno);
    uint32_t season = (*actseason + *yearstart);
    // Find own team
    for (int i = 0; i < clteamcount; i++)
    {
      if (clteams[i].res0 > 1)
      {
        SWSCarTeam* ownteam = (SWSCarTeam*)(ownteamincareer+swshwnd);
        FILE* q = fopen("!OWN!.DAT", "wb");
        fwrite((void*)(ownteam), sizeof(SWSCarTeam), 1, q);
        fclose(q);
        SWSTeam own;
        memcpy((void*)&own, (void*)(&ownteam->nation),sizeof(SWSTeam));
        for (int p = 0; p < 16; p++)
        {
          memcpy((void*)(&own.players[p]), (void*)(&ownteam->players[ownteam->playerspos[p]]), sizeof(SWSPlayer));
          own.playerpos[p] = p;
        }
        memcpy((void*)(clteams+i), (void*)(&own), sizeof(SWSTeam));
        
      }
    }
    // Qualifications
    int tqualcount = (clownqualify) ? 5 : 6;
    qsort(clqualify, clqualifycount, sizeof(SWSTeam), comprand);
    for (int i = 0; i < tqualcount; i++)
    {
      //MessageBoxA(NULL, clqualify[i].name, "Qualify", MB_OK);
      memcpy((void*)(clteams+i+clteamcount), (void*)(clqualify+i), sizeof(SWSTeam));
    }
    clteamcount += tqualcount;
    // Set new seed;
    int64_t s1 = rand();
    int64_t s2 = rand();
    uint64_t seed = (s1 << 32)+s2;
    memcpy((void*)(cldiydata + 0x141a), &seed, 6);
    // randomize teams
    qsort(clteams, 32, sizeof(SWSTeam), comprand);
    // save teams->{nation, id} to table
    for (int i = 0; i < 32; i++)
    {
      uint16_t ids = (clteams[i].number) << 8 | clteams[i].nation; 
      memcpy((void*)(cldiydata + 0x99 + (i*2)), (void*)&ids, 2);
      memcpy((void*)(cldiydata + 0x54c + (i*2)), (void*)&ids, 2);
    }
    sprintf(filename, "%s_%d.DIY", "CLTST", season);
    FILE* f = fopen(filename, "wb");
    fwrite(cldiydata, champlgediy_size, 1, f);
    fwrite(clteams, sizeof(SWSTeam), clteamcount, f);
    fclose(f);
    //MessageBoxA(NULL, "End Career", "SavCar.ASI", MB_OK);
    free(cldiydata);
    cldiydata = NULL;
    careerstart = false;
  }
  return regs->A[0];
}


void GetCLDirect(SWSTableInfo* table, int teamcount, bool ownleague)
{
  int selectorid = -1;
  SWSTeam* tteam = (SWSTeam*)(swshwnd + teambuf + table[0].offsett);
  for (int i =0; i < 43; i++)
  {
    if (natcldirect[i].nationid == tteam->nation)
    {
      if (tteam->division == 0)
        selectorid = i;
    }
  }
  int cl = 0;
  int clend = 0;
  if (selectorid > 0)
  {
    cl = natcldirect[selectorid].startcount;
    clend = natcldirect[selectorid].startcount + natcldirect[selectorid].teamscount;
  }
  for (int i = 0; i < teamcount; i++)
  {
    SWSTeam* team = (SWSTeam*)(swshwnd + teambuf + table[i].offsett);
    if (cl < clend)
    {
      //MessageBoxA(NULL, team->name, "Direct",MB_OK);
      if (ownleague)
      {
        if (team->res0 > 1)
          clownteam = true;
      }
      memcpy((void*)(clteams+clteamcount), team, sizeof(SWSTeam));
      clteamcount++;
    }
    cl++;
  }
}

void GetCLQualify(SWSTableInfo* table, int teamcount, bool ownleague)
{
  int selectorid = -1;
  SWSTeam* tteam = (SWSTeam*)(swshwnd + teambuf + table[0].offsett);
  for (int i =0; i < 43; i++)
  {
    if (natclqualify[i].nationid == tteam->nation)
    {
      if (tteam->division == 0)
        selectorid = i;
    }
  }
  int cl = 0;
  int clend = 0;
  if (selectorid > 0)
  {
    cl = natclqualify[selectorid].startcount;
    clend = natclqualify[selectorid].startcount + natclqualify[selectorid].teamscount;
  }
  for (int i = 0; i < teamcount; i++)
  {
    SWSTeam* team = (SWSTeam*)(swshwnd + teambuf + table[i].offsett);
    if (cl < clend)
    {
      if (ownleague == true)
      {
        if (team->res0 > 1)
        {
          clownteam = true;
          // own team always play in cl
          memcpy((void*)(clteams+clteamcount), team, sizeof(SWSTeam));
          clteamcount++;
          clownqualify = true;
        }
      }
      memcpy((void*)(clqualify+clqualifycount), team, sizeof(SWSTeam));
      clqualifycount++;
    }
    cl++;
  }
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
  //qsort(table, *teamcount, sizeof(SWSTableInfo), compdiff);
  qsort(table, *teamcount, sizeof(SWSTableInfo), comppts); 
  //
  CreateDirectory("careerlogs", NULL);
  sprintf(filename, "careerlogs\\%d_%s.txt", season, comp->name);
  FILE* f = fopen(filename, "w");
  fprintf(f, "CompName: %s\n", comp->name);
  fprintf(f, "Teams: %d\n", *teamcount);
  fprintf(f, "team;p;w;d;l;gf;ga;pts\n");
  for (int i = 0; i < *teamcount; i++)
  {
    SWSTeam* team = (SWSTeam*)(swshwnd + teambuf + table[i].offsett);
    //fprintf(f, "File: %02x, No: %02x, GenNo: %04x, %s\n", team->teamfileno, team->number, team->genswosno, team->name);
    fprintf(f, "%s;%d;%d;%d;%d;%d;%d;%d\n", team->name, table[i].matches, 
    table[i].wins, table[i].draws, table[i].lost, table[i].goals_for, table[i].goals_again, table[i].points);
  }
  // Check Champions League Direct
  GetCLDirect(table, *teamcount, true);
  GetCLQualify(table, *teamcount, true);
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
  for (int i = 0; i < *teamcount; i++)
  {
    SWSTeam* team = (SWSTeam*)(swshwnd + teambuf + table[i].offsett);
    //fprintf(f, "File: %02x, No: %02x, GenNo: %04x, %s\n", team->teamfileno, team->number, team->genswosno, team->name);
    fprintf(f, "%s;%d;%d;%d;%d;%d;%d;%d\n", team->name, table[i].matches, 
    table[i].wins, table[i].draws, table[i].lost, table[i].goals_for, table[i].goals_again, table[i].points);
  }
  // Check Champions League Direct
  GetCLDirect(table, *teamcount, false);
  GetCLQualify(table, *teamcount, false);
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
    uint32_t rel = ((uintptr_t)&SaveCurrent) - (adr + 5);
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
    careerstart = false;
    srand(time(NULL));
	}
	return TRUE;
}