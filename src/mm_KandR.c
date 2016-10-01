/*
  ebako / MasterMind
    ver 1.0         質問の最適化無し(テーブルの若いものから選ぶ)
                                    (第一質問もいい加減のまま)
    ver 1.1 - 1.3   バグ解消版
    ver 1.4         少しだけ高速化
    ver 1.5         テーブル情報を表示
    ver 1.5.rewrite ソース起こし & 再コメント
                    (消去法あるいは「篩」 8色 = 8進数を利用して permutation)
                    (bit pattern を使うとメモリ節約可能)

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

#include <stdio.h>
#include <stdlib.h>

typedef char byte;
typedef unsigned int word;

#define FALSE ((byte)(-1))              /* 不正解の篩フラグ (名前変えるべき) */
#define MAXLEN 5                        /* 5桁 (固定) */
#define MAXCOL 8                        /* 8色 = 8進数 (固定) */
#define MAXTBL 0100000                  /* (1 << (3 * MAXLEN)) */
#define RIGHTANS (MAXLEN * 0x0100)      /* 応答は 16進数 2桁 正解は 0x0500 */

int main(int, char **);
void Master(byte *);
word CheckAns(byte *, byte *);
void Adr2Str(word, byte *);
byte Adr2Chr(word, int);
byte *MakeTable(void);
void FirstStr(byte *);
void NextStr(byte *, byte *);
word Reaction(byte *);
word RemakeTable(byte *, byte *, word);
void PrintC(byte *);

int main(ac, av)
int ac;
char *av[];
{
  byte ans[MAXLEN + 1];
  Master(ans);
  ans[MAXLEN] = '\0';
  fprintf(stdout, "The answer is [%s]\n", ans);
  return 0;
}

/*
  solve mastermaind
*/
void Master(ans)
byte ans[];
{
  byte *table;
  byte ques[MAXLEN];
  word reac, count;
  int i;
  table = MakeTable();
  FirstStr(ques);
  while((reac = Reaction(ques)) != RIGHTANS){
    switch(count = RemakeTable(table, ques, reac)){
    case 0:
      fputs("There is no possible answer.\n", stdout);
      break;
    case 1:
      fputs("The answer should be ", stdout);
      break;
    default:
      fprintf(stdout, "There are %d candidates in my mind.\n", count);
      break;
    }
    NextStr(table, ques);
  }
  for(i = 0; i < MAXLEN; i++) ans[i] = ques[i];
}

/*
  質問文字列を答文字列と比較
  上位バイト: 位置正解数, 下位バイト: 色正解数
*/
word CheckAns(ques, ans)
byte ques[], ans[];
{
  word high, low;
  byte cond[MAXLEN];
  int i, j;
  for(i = 0; i < MAXLEN; i++)
    if(ans[i] == ques[i]) cond[i] = 1;
    else                  cond[i] = 0;
  for(i = 0; i < MAXLEN; i++)
    if(cond[i] != 1)
      for(j = 0; j < MAXLEN; j++){
        if(cond[j] == 0)
          if(ans[i] == ques[j]){ cond[j] = 2; break; }
      }
  high = low = 0;
  for(i = 0; i < MAXLEN; i++)
    switch(cond[i]){
    case 1: high++; break;
    case 2: low++; break;
    default: break;
    }
  return high * 0x0100 + low;
}

/*
  テーブルのアドレスを質問文字列に変換
*/
void Adr2Str(tbladr, ques)
word tbladr;
byte ques[];
{
  int i;
  for(i = 0; i < MAXLEN; i++) ques[i] = Adr2Chr(tbladr, i);
}

/*
  テーブルのアドレスからn桁目を分離
*/
byte Adr2Chr(tbladr, num)
word tbladr;
int num;
{
  int shift;
  shift = 3 * (MAXLEN - 1 - num);
  return (byte)((tbladr >> shift) & 0x07) + '0';
}

/*
  テーブルを作製して初期化
*/
byte *MakeTable()
{
  static byte table[MAXTBL];
  word tbladr;
  for(tbladr = 0; tbladr < MAXTBL; tbladr++) table[tbladr] = 0;
  return table;
}

/*
  最初の質問文字列を考える(とりあえず12345)
*/
void FirstStr(ques)
byte ques[];
{
  Adr2Str(012345, ques);
}

/*
  2番目以降の質問文字列を考える
*/
void NextStr(table, ques)
byte table[];
byte ques[];
{
  word tbladr;
  for(tbladr = 0; tbladr < MAXTBL; tbladr++)
    if(table[tbladr] != FALSE){
      Adr2Str(tbladr, ques);
      break;
    }
}

/*
  ヒントをもらう
*/
word Reaction(ques)
byte ques[];
{
  word reac;
  PrintC(ques);
  fputs(" -? (PPCC) \a", stdout);
  fscanf(stdin, "%x", &reac);
  return reac;
}

/*
  テーブルを更新
*/
word RemakeTable(table, ques, reac)
byte table[];
byte ques[];
word reac;
{
  word tbladr, count;
  byte ans[MAXLEN];
  for(tbladr = count = 0; tbladr < MAXTBL; tbladr++)
    if(table[tbladr] != FALSE){
      Adr2Str(tbladr, ans);
/*{ PrintC(ans); fprintf(stdout, " %04x\n", CheckAns(ques, ans)); }*/
      if(CheckAns(ques, ans) != reac) table[tbladr] = FALSE;
      else                            count++;
    }
  return count;
}

void PrintC(ques)
byte ques[];
{
  int i;
  for(i = 0; i < MAXLEN; i++) fputc(ques[i], stdout);
}
