/*
  mastermind.c

  (when 01765)
  12345 -? (PPCC) 0101
  There are 4880 candidates in my mind.
  00015 -? (PPCC) 0201
  There are 172 candidates in my mind.
  00141 -? (PPCC) 0101
  There are 30 candidates in my mind.
  01555 -? (PPCC) 0300
  There are 5 candidates in my mind.
  01665 -? (PPCC) 0400
  There are 2 candidates in my mind.
  01675 -? (PPCC) 0302
  The answer should be 01765 -? (PPCC) 0500
  The answer is [01765]
*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char byte;
typedef unsigned int uint;

#define NAA ((byte)(-1)) // 不正解の篩フラグ Not An Answer
#define MAXLEN 5 // 5桁 (固定)
#define MAXCOL 8 // 8色 = 8進数 (固定)
#define MAXTBL ((uint)1 << (3 * MAXLEN)) // 3 = log2(8)
#define RIGHTANS (MAXLEN * 0x0100) // 応答は 16進数 2桁 正解は 0x0500
#define BUFLEN 4

uint Reaction(byte *ques)
{
  byte buf[BUFLEN + 2]; // 改行 + '\0'
  uint reac, last;
  fprintf(stdout, "%.*s -? (PPCC) \a", MAXLEN, ques);
  while(1){
    if(!fgets(buf, sizeof(buf), stdin)) return RIGHTANS;
    last = strlen(buf) - 1;
    if(buf[last] == 0x0a || buf[last] == 0x0d) buf[last] = '\0';
    if(strlen(buf) != BUFLEN){
      fflush(stdin); fputs("(Enter just 4 characters.) \a", stderr); continue;
    }
    if(buf[0] != '0' || buf[2] != '0'){ fputs("(0H0B) \a", stderr); continue; }
    if(buf[1] < '0' || buf[1] > '7' || buf[3] < '0' || buf[3] > '7'){
      fputs("(H or B must be a number 0-7.) \a", stderr); continue;
    }
    sscanf(buf, "%x", &reac);
    if(reac < 0 || reac > RIGHTANS){
      fprintf(stderr, "(PPCC must be in 0000-%04x.) \a", RIGHTANS); continue;
    }
    if((reac >> 8) + (reac & 0x00FF) > MAXLEN){
      fputs("(PP + CC must be in 00-05.) \a", stderr); continue;
    }
    break;
  }
  return reac;
}

uint CheckAns(byte *ques, byte *ans)
{
  byte cond[MAXLEN];
  int i, j;
  uint high = 0, low = 0;
  for(i = 0; i < sizeof(cond); i++) cond[i] = ans[i] == ques[i] ? 1 : 0;
  for(i = 0; i < sizeof(cond); i++)
    if(cond[i] != 1)
      for(j = 0; j < sizeof(cond); j++)
        if(!cond[j] && ans[i] == ques[j]){ cond[j] = 2; break; }
  for(i = 0; i < sizeof(cond); i++)
    if(cond[i] == 1) high++; else if(cond[i] == 2) low++;
  return high * 0x0100 + low;
}

byte *Adr2Str(uint tbladr, byte *ques)
{
  int i;
  for(i = 0; i < MAXLEN; i++)
    ques[i] = (byte)((tbladr >> (3 * (MAXLEN - 1 - i))) & 0x07) + '0';
  return ques;
}

uint RemakeTable(byte *table, byte *ques, uint reac)
{
  uint tbladr, count;
  byte ans[MAXLEN];
  for(tbladr = count = 0; tbladr < MAXTBL; tbladr++)
    if(table[tbladr] != NAA)
      if(CheckAns(ques, Adr2Str(tbladr, ans)) != reac) table[tbladr] = NAA;
      else count++;
  return count;
}

int FirstQuestion(void)
{
  volatile int i;
  srand((unsigned)time(NULL));
  for(i = rand() % MAXLEN; i-- >= 0;){} // dummy loop
  return rand();
}

int main(int ac, char **av)
{
  byte ques[MAXLEN];
  byte table[MAXTBL];
  uint reac, count;
  int tbladr = FirstQuestion();
  memset(table, 0, sizeof(table));
  while((reac = Reaction(Adr2Str(tbladr % sizeof(table), ques))) != RIGHTANS){
    switch(count = RemakeTable(table, ques, reac)){
    case 0: fputs("There is no possible answer.\n", stdout); break;
    case 1: fputs("The answer should be ", stdout); break;
    default: fprintf(stdout, "There are %d candidates in my mind.\n", count);
    }
    for(tbladr = sizeof(table); --tbladr >= 0;) if(table[tbladr] != NAA) break;
  }
  fprintf(stdout, "The answer is [%.*s]\n", sizeof(ques), ques);
  return 0;
}
