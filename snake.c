#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <conio.h>
//test git
typedef unsigned int    U32;
typedef unsigned short 	U16;
typedef unsigned char 	U8;
typedef signed int      S32;
typedef signed short    S16;
typedef signed char     S8;

#define SnakeLines   	(30) /* the range of snake runs */
#define SnakeNums   	(30)
#define SumLine 	(SnakeLines+2)
#define SumNum 		(SnakeNums+2)

#define ScoreStep 	(10)
/* key defination */
#define Enter 		(0x0d)
#define Huanhang	(0x0a)
#define Up		(0x48)
#define Left 		(0x4b)
#define Right 		(0x4d)
#define Down 		(0x50)
#define ESC		(27)

/* color defination */
#define Blue      9
#define Green 	  10
#define Red 	  12
#define Yellow    6
#define White     0xF

/* define a FoodIsThereState_E to tell program the food is eaten or not*/
typedef enum
{
  idle,
  itrue, /* not eaten, still there */
  ifalse,/* Oops, food eaten */
  iend,
}FoodIsThereState_E;

/* return a PlayRes_E according to the key you input during playing retro snake*/
typedef enum
{
  YouAreDone=1,
  YouAreExcllent,
  YouAreStillYou,
  WhoYouAre,
}PlayRes_E;

/* define a DotType_E to distinguish different dots in RANG */
typedef enum
{
  DotIdle,
  DotHead,
  DotBody,
  DotTail,
  DotFood,
  DotWall,
  DotEnd,
}DotType_E;

/* dot position defination */
typedef struct 
{
  U8 line;
  U8 num;
}DOT_S;

U16 Score=0;
U16 Snakelen=0;
U16 Bodylen=0;
U8 FirstRun=itrue;
DOT_S Food;
DOT_S Head;
DOT_S Tail;

DOT_S Old2Clear[SnakeLines*SnakeNums]={0};
DOT_S Body[SnakeLines*SnakeNums] = {0};
DotType_E DotType[SumLine+5][SumNum+5]={0};
FoodIsThereState_E FoodIsThereFlag=ifalse;

static void AllInitail(void)
{
  Score = 0;
  Snakelen=0;
  Bodylen=0;
  FoodIsThereFlag=ifalse;

  memset(&Body[0],0x00,sizeof(Body));
  memset(&DotType[0][0],0x00,sizeof(DotType));
}

static void gotoxy(U8 line, U8 num) 
{
  COORD pos = {(num-1)*2,line};
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleCursorPosition(hOut, pos);
}

static void RandomAxy(U8 *line, U8 *num)
{
  srand(time(0));
  *line = rand()%SnakeLines + 2;
  srand(*line);
  *num = rand()%SnakeNums + 2;
}

void SetDotType(U8 line,U8 num,DotType_E type)
{
  DotType[line][num] = type;
}

U8 GetDotType(U8 line,U8 num)
{
  return (U8)DotType[line][num];
}

/* to renew a new head */
void NewHead(U8 line, U8 num)
{
  if ((line >= 2)&&(line <= SnakeLines + 1))
    Head.line = line;
  if ((num >= 2)&&(line <= SnakeNums + 1))
    Head.num = num;
}

/* to renew a new body, add new dot to body list */
void NewBody(U8 line, U8 num)
{
   if ((line >= 2)&&(line <= SnakeLines + 1))
      Body[Bodylen].line = line;
   if ((num >= 2)&&(line <= SnakeNums + 1))
      Body[Bodylen].num = num;
}

U8 GetInput(void)
{
   return (U8)getch();
}

/* only showed in idle range */
static void ShowRandomFood(void)
{
   HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

   for(;;)
   {
      RandomAxy(&Food.line,&Food.num);
      switch(GetDotType(Food.line,Food.num))
      {
      case DotIdle:break;
      default:continue;
      }
      break;
   }
   SetDotType(Food.line,Food.num,DotFood);
   gotoxy(Food.line,Food.num);
   SetConsoleTextAttribute(hOut, Red);	
   printf("¡ï");
   FoodIsThereFlag = itrue;
}

static void ShowInitialSnake(void)
{
   HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

   NewHead(SnakeLines/2,SnakeNums/2);
   SetDotType(Head.line,Head.num,DotHead);
   gotoxy(Head.line,Head.num);
   SetConsoleTextAttribute(hOut, Yellow);	
   printf("¡ñ");
   Tail.line = Head.line;
   Tail.num = Head.num;
   Snakelen = 1;
   Bodylen = 0;
}

void ShowCityWall(void)
{
   U8 linetemp=0;
   U8 numtemp=0;
   HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

   for(linetemp=1;linetemp<=SumLine;linetemp++)
   {
      for(numtemp=1;numtemp<=SumNum;numtemp++)
      {
         if ((numtemp==1)||(numtemp==SumNum)||(linetemp==1)||(linetemp==SumLine))
         {
            SetConsoleTextAttribute(hOut, Red);
            gotoxy(linetemp,numtemp);
            printf("¡õ");
            SetDotType(linetemp,numtemp,DotWall);
         }
         else
         {
            SetConsoleTextAttribute(hOut, Green);
            gotoxy(linetemp,numtemp);
            printf("¡ö");
            SetDotType(linetemp,numtemp,DotIdle);
         }
      }
      printf("\r\n");
   }
}

/* food eaten, trans food to head */
void Food2Head(U8 foodline,U8 foodnum)
{
  hOut = GetStdHandle(STD_OUTPUT_HANDLE);

  NewHead(foodline,foodnum);
  gotoxy(foodline, foodnum);
  SetConsoleTextAttribute(hOut, Yellow);	
  printf("¡ñ");
  SetDotType(foodline,foodnum,DotHead);
  Snakelen++;
}

/* food eaten, trans head to body */
void Head2Body(U8 headline,U8 headnum)
{
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

  NewBody(headline,headnum);
  gotoxy(headline, headnum);
  SetConsoleTextAttribute(hOut, Yellow);	
  printf("¡ð");
  SetDotType(headline,headnum,DotBody);
  Bodylen++;
}

void IdeHead2Body(U8 headline,U8 headnum)
{
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

  gotoxy(headline, headnum);
  SetConsoleTextAttribute(hOut, Yellow);	
  printf("¡ð");
  SetDotType(headline,headnum,DotBody);
}

/* idle eaten, trans idle to head */
void Idle2Head(U8 Idleline,U8 Idlenum)
{
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  NewHead(Idleline,Idlenum);
  gotoxy(Idleline,Idlenum);
  SetConsoleTextAttribute(hOut, Yellow);	
  printf("¡ñ");	
  SetDotType(Idleline,Idlenum,DotHead);
}

/* trans dot to idle */
void Dot2Idle(U8 line,U8 num)
{
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

  gotoxy(line, num);
  SetConsoleTextAttribute(hOut, Green);
  printf("¡ö");
  SetDotType(line,num,DotIdle);
}

/* idle eaten, update body */
void IdleUpdateBody(void)
{
  U8 i;
	
  if (0 == Bodylen) return;
	
  for(i=0;i<Bodylen-1;i++)
  {
    Body[i].line = Body[i+1].line;
    Body[i].num = Body[i+1].num;
    SetDotType(Body[i].line,Body[i].num,DotBody);
  }

  Body[Bodylen-1].line = Head.line;
  Body[Bodylen-1].num = Head.num;
  SetDotType(Body[Bodylen-1].line,Body[Bodylen-1].num,DotBody);
}

U8 ShowGameOver(void)
{
  U8 key;

  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute(hOut, (Yellow<<4)|White);
  gotoxy(SumLine/2,SumNum/2 - 6);
  printf("  YOU LOST! PRESS:          ");
  SetConsoleTextAttribute(hOut, (Yellow<<4)|White);
  gotoxy(SumLine/2+1,SumNum/2 - 6);
  printf("  ESC -- exit               ");
  SetConsoleTextAttribute(hOut, (Yellow<<4)|White);
  gotoxy(SumLine/2+2,SumNum/2 - 6);
  printf("  Other Key -- Continue     ");

  gotoxy(SumLine+2,SumNum/2-3+6);
  key = GetInput();
  if (key == ESC)
    return 1;
  else
    return 2;
}

U8 mainloop(U8 key)
{
  U8 i=0;

  switch(key)
  {
  case Right:
    switch(GetDotType(Head.line,Head.num+1))
    {
    case DotWall:
    case DotBody:
      return YouAreDone;
    case DotFood:
      Head2Body(Head.line,Head.num);
      Food2Head(Food.line,Food.num);

      FoodIsThereFlag = ifalse;
      Score += ScoreStep;
      return YouAreExcllent;
    case DotIdle:
      IdeHead2Body(Head.line,Head.num);
      if (1 == Snakelen)
         Dot2Idle(Head.line,Head.num);
      else
         Dot2Idle(Body[0].line,Body[0].num);

      IdleUpdateBody();
      Idle2Head(Head.line,Head.num+1);				
      return YouAreStillYou;
    default:
      return WhoYouAre;	
    }
    break;
  case Down:
    switch(GetDotType(Head.line+1,Head.num))
    {
    case DotWall:					
    case DotBody:
      return YouAreDone;
    case DotFood:
      Head2Body(Head.line,Head.num);
      Food2Head(Food.line,Food.num);
      FoodIsThereFlag = ifalse;
      Score += ScoreStep;
      return YouAreExcllent;
    case DotIdle:
      IdeHead2Body(Head.line,Head.num);
      if (1 == Snakelen)
        Dot2Idle(Head.line,Head.num);
      else
        Dot2Idle(Body[0].line,Body[0].num);
      
      IdleUpdateBody();
      Idle2Head(Head.line+1,Head.num);		
      return YouAreStillYou;
    default:
      return WhoYouAre;	
    }
    break;			
  case Up:
    switch(GetDotType(Head.line-1,Head.num))
    {
    case DotWall:
    case DotBody:
      return YouAreDone;
    case DotFood:
      Head2Body(Head.line,Head.num);
      Food2Head(Food.line,Food.num);
      FoodIsThereFlag = ifalse;
      Score += ScoreStep;
      return YouAreExcllent;
    case DotIdle:
      IdeHead2Body(Head.line,Head.num);
      if (1 == Snakelen)
        Dot2Idle(Head.line,Head.num);
      else
        Dot2Idle(Body[0].line,Body[0].num);

      IdleUpdateBody();
      Idle2Head(Head.line-1,Head.num);				
      return YouAreStillYou;
    default:
      return WhoYouAre;	
    }
    break;	
  case Left:
    switch(GetDotType(Head.line,Head.num-1))
    {
      case DotWall:
      case DotBody:
        return YouAreDone;
      case DotFood:
        Head2Body(Head.line,Head.num);
        Food2Head(Food.line,Food.num);
        FoodIsThereFlag = ifalse;
        Score += ScoreStep;
        return YouAreExcllent;
      case DotIdle:
        IdeHead2Body(Head.line,Head.num);
        if (1 == Snakelen)
          Dot2Idle(Head.line,Head.num);
        else
          Dot2Idle(Body[0].line,Body[0].num);

        IdleUpdateBody();
        Idle2Head(Head.line,Head.num-1);				
        return YouAreStillYou;
      default:
        return WhoYouAre; 
      }
      break;	
    default:
      return WhoYouAre;
    }
}

int main(void)
{
   U8 key=0;
   U8 res=0;
   HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

   loop:for(;;)
   {
      AllInitail();
      ShowCityWall();

      SetConsoleTextAttribute(hOut, Red);
      gotoxy(SumLine+2,2);
      printf("Author: Tuocan Dong");
      gotoxy(SumLine+3,2);
      printf("For more information:");
      gotoxy(SumLine+5,2);
      printf("http://www.alphadamo.com");
      gotoxy(SumLine+4,2);
      printf("Email:dongtuoc@yeah.net");

      if (FirstRun == itrue)
      {
         SetConsoleTextAttribute(hOut, Red);
         gotoxy(SumLine+2,16);
         printf("PRESS ENTER TO START");

         for(;;)
         {
            key = GetInput();
            if ((key==Enter)||(key==Huanhang))
               break;
         }
      }
   
      gotoxy(SumLine+2,16);
      printf("                    ");
      SetConsoleTextAttribute(hOut, Red);
      gotoxy(SumLine+2,16);
      printf("GAME RUNNING");

      ShowInitialSnake();

      SetConsoleTextAttribute(hOut, Red);
      gotoxy(SumLine+3,16);
      printf("Your Score:");
      gotoxy(SumLine+4,16);
      printf("Your  Goal:");
      SetConsoleTextAttribute(hOut, Yellow);
      gotoxy(SumLine+4,16 +6);
      printf("%4d",(SumLine*SumNum*ScoreStep-1500));

      for(;;)
      {
         SetConsoleTextAttribute(hOut, Yellow);
         gotoxy(SumLine+3,16+6);
         printf("%04d",Score);
 
         if (FoodIsThereFlag == ifalse)
            ShowRandomFood();

         gotoxy(SumLine+2,16+6);
         key = GetInput();

         res = mainloop(key);
         switch(res)
         {
            case YouAreDone:
               key = ShowGameOver();
               if (key == 1)//esc
               {
                  return 0;
               }
               else if (key == 2)
               {
                  FirstRun = ifalse;
                  goto loop;
               }
               break;
            case YouAreExcllent:
            case YouAreStillYou:
            case WhoYouAre:
            default:
               break;
         }
      }
   }
}
