/*----------------------------------------------------------------------------*/
/* Autor:               Jeamy                                                 */
/* Datum:               Mittwoch 24.02.1993                                   */
/* Version:             12.9.2.3.81                                           */
/*----------------------------------------------------------------------------*/

#include <dos.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <graph.h>
#include <conio.h>          

typedef unsigned short USHORT;
typedef unsigned long  ULONG;
 
//#define MAX_TIME 32760                            // == 30 min
//#define MAX_TIME   16380                          // == 15 min
#define MAX_TIME   10920                           // == 10 min
#define BUF_LEN    1024 
    
void                          _nullcheck  ( void );
void  (_cdecl _interrupt _far *vOldInt08) ( void );
void   _cdecl _interrupt _far vNewInt08   ( void );
void  (_cdecl _interrupt _far *vOldInt09) ( void );
void   _cdecl _interrupt _far vNewInt09   ( void );
//extern short                  SJDecrypt   ( char*, short );
static void                   asmGolomb   ( void );
static void                   vFillBuffers( void );

void Beep( int frequency, int duration );
void Sleep( clock_t wait );
 
enum NOTES      /* Enumeration of notes and frequencies     */
{
    C0 = 262, D0 = 296, E0 = 330, F0 = 349, G0 = 392, A0 = 440, B0 = 494,
    C1 = 523, D1 = 587, E1 = 659, F1 = 698, G1 = 784, A1 = 880, B1 = 988,
    EIGHTH = 125, QUARTER = 250, HALF = 500, WHOLE = 1000, END = 0
} song[] =      /* Array initialized to notes of song       */

{
    C1, HALF, G0, HALF, A0, HALF, E0, HALF, F0, HALF, E0, QUARTER,
    D0, QUARTER, C0, WHOLE, END
};
 
static char * pszTest; 
static char * pszTBase; 
static short* psGol;
static short  sCount=0;
static short  sCountR=0;
static short  sMax=0;
static short  sMaxR;
static short  sIndex=4;
static short  sFlag=1;
static ULONG  ulInt08;
static char   cSem=0;
static char   cGolWrite=0;
static char   cGolEsc=0;
static short  sTime=MAX_TIME;
static short  sFile=0;
static FILE * fp, *fpA, * fpSta;
static char * pszLine;    
static char * pszBuffer;
static char   szCurrentStatus[] = { "Status :" };
static char   szCurrentTime[] =   { "Statuszeit:" };
static char   szGolombStat[]=     { "golomb.sta" };
static char   szGolombB1[]=       { "golomb.ba1" };
static char   szGolombB2[]=       { "golomb.ba2" };
static char   szGolombBx[]=       { "golomb.bax" };
static char  *szEscape[]=         { "Escape" };
static char  *szReturn[]=         { "      " };

static  char  szJeamy[]=
{
"(c) JEAMY 12/1993"
};
static  char  szJeamy1[]=
{
"  GOLOMB by JEAMY v.23.12.1993#"
};                 
                    
void _nullcheck( void )
{
}

  cdecl main( int argc, char *argv[] )
{ 

char   szCount[] =         { "Anzahl der Markierungen : " };
char   szMax[] =           { "Maximale L„nge : " };
char   szGolomb[] =        { "Golomb Lineal :" };
char   szAbst[] =          { "Zu messende Abst„nde :" };
char   szStart[] =         { "Startzeit: " };
char   szWeiter[] =        { "Weiter ? (j/n)" };
char   szUser[] =          { "\nProgramm vorzeitig beendet !" };
char   szDel[] =           { "COPY GOLOMB.BA1 GOLOMB.BAX" };
char   szEnd[] =           { "Endzeit:   " };
char   szGolombErg[]=      { "golomb.erg" };
int          note;
short        sB, sA;
short        sCont=1;
unsigned short usKey;
char         szBuf[5];
time_t       timeStart;
time_t       timeEnd;
time_t       timeh;
struct tm *  tmLocal;
short        sTable[]=     { 1,   3,   6,  11,  17,  25,  34,  44,  55,
                            72,  85, 106, 127, 151, 177, 199, 216, 246,
                           283, 333, 356, 372, 425 };
  
// _clearscreen( _GCLEARSCREEN );
 if ( !(psGol=calloc( 30,2 ))){
   printf("Memory allocation error !");
   return 0;
 } /* endif */       
 if ( !(pszTest=calloc( BUF_LEN,1 ))){
   printf("Memory allocation error !");
   return 0;
 } /* endif */       
 if ( !(pszBuffer=calloc( BUF_LEN,1 ))){
   printf("Memory allocation error !");
   return 0;
 } /* endif */       
 if ( !(pszLine=calloc( BUF_LEN,1 ))) {
   printf("Memory allocation error !");
   return 0;
 } /* endif */       


// SJDecrypt( szJeamy1, strlen(szJeamy1) );
// _settextcolor( 14 );
// _settextposition( 1, 1 );
 printf( "\n%s\n",szJeamy1 );
  
 if ( (fpA=fopen( szGolombB1, "rb" ))) {
   if ( (fread( &sCount, 2, 1, fpA )==1) && sCount) {
     sCont=0;     
     if ( fread( psGol, 2, sCount,  fpA )!=(USHORT)sCount )
       sCont=1;   
     if ( fread( &sFlag, 2, 1, fpA )!=1 )
       sCont=1;   
     if ( fread( &sIndex, 2, 1, fpA )!=1 )
       sCont=1;   
     if ( fread( &sMax, 2, 1, fpA )!=1 )
       sCont=1;   
     fclose ( fpA ); 
     sCountR=sCount;    
     vFillBuffers();
     printf( "\n%s", pszBuffer );
     printf( "\n%s", szWeiter );
      while ( 1 ) {  
       usKey=getch();                                            
       if ( usKey==110 ) {
         printf( szUser );
         exit(-1);
       } /* endif */                                                        
       if ( usKey==106 )
         break;                                                                                     
     } /* endwhile */                                              
     printf( "\nKopiere golomb.ba1 nach golomb.bax" );
     system( szDel ); 
     fpSta = fopen( szGolombStat, "at" );
     time ( &timeh );
     printf( "\n%s", pszLine );
     fprintf( fpSta, "\n%s", pszLine );
     tmLocal = localtime ( &timeh );
     fprintf( fpSta,"\n%s%s", szStart, asctime( tmLocal ) );
     printf( "\n%s", szCurrentStatus );
     fprintf( fpSta,"%s", szCurrentStatus );
     printf( "\n%s", pszBuffer );
     fprintf( fpSta, "\n%s", pszBuffer );
     printf( "\n%s\n", pszLine );
     fprintf( fpSta, "\n%s\n", pszLine );
     fclose ( fpSta );
   } /* endif */
 } /* endif */
                              
 if ( sCont ) {
   printf( "\n%s", pszLine );
   while ( sCont ) {
     printf( "%s", szCount );
     gets( szBuf );
     sCount=atoi(szBuf);
     if ( sCount>1 && sCount <25 )
       sCont=0;
   } /* endwhile */

   sMax=sTable[sCount-2];
   printf( "%s%d\n", szMax, sMax );
   
   psGol[0]=0;
   psGol[1]=1;
   psGol[2]=3;
   if (argc==2 && atoi(argv[1])) 
     psGol[2]=atoi( argv[1] )                         ;
   psGol[3]=sMax;
   psGol[sCount-1]=sMax;
 } /* endif */

 if ( !(pszTBase=calloc( BUF_LEN*sCount,1 ))){
   printf("Memory allocation error !");
   return 0;
 } /* endif */       
 pszTBase+=BUF_LEN*sCount/2;
 fp = fopen( szGolombErg, "at" );
 if ( !sCont ) {
   fprintf( fp, "\n%s", pszLine );
   fprintf( fp, "\n%s%d", szCount, sCount );
   fprintf( fp, "\n%s%d\n", szMax, sMax );
 } /* endif */
 
 sCountR=sCount;    
 sCount=(sCount<<1)-4;
 sMaxR=(sMax>>1)+1;
 vOldInt08 = _dos_getvect( 8 );
 vOldInt09 = _dos_getvect( 9 );
 _dos_setvect( 8, vNewInt08 );
 _dos_setvect( 9, vNewInt09 );
           
 time ( &timeStart );
 tmLocal = localtime ( &timeStart );
 fprintf( fp,"\n%s%s", szStart, asctime( tmLocal ) );
 printf( "\n%s%s", szStart, asctime( tmLocal ) );
   
 asmGolomb( );
   
 time ( &timeEnd );                               
 tmLocal = localtime ( &timeEnd );
 printf( "%s%s", szEnd, asctime( tmLocal ) );
 fprintf( fp, "%s%s", szEnd, asctime( tmLocal ) );
 if ( !cGolEsc ) {
   fprintf( fp, "\n%s", szGolomb  );
   printf( "\n%s", szGolomb );
   vFillBuffers();
   printf( "\n%s", pszBuffer );
   fprintf( fp, "\n%s", pszBuffer );
   sB=0;
   fprintf( fp, "\n\n%s\n", szAbst );
   printf( "\n\n%s\n", szAbst );
   for ( sA=1; sA<sMax+1; sA++ ) {
     if ( pszTest[sA] ) {
       printf( "%4d", sA );
       fprintf( fp, "%4d", sA );
       sB++;
     } /* endif */
     if ( sB==10  ) {
       printf( "\n" );
       fprintf( fp, "\n" );
       sB=0;      
     } /* endif */
   } /* endfor */
   memset(pszLine, 196, 79);
   pszLine[80]=0;
   printf( "\n%s\n", pszLine );
   fprintf( fp, "\n%s\n", pszLine );
   remove ( szGolombB1 );
   remove ( szGolombB2 );
 } /* endif */
 else {         
   printf( szUser );
 } /* endelse */

// _settextcolor( 7 );
// _setbkcolor( 0 );
 free(pszBuffer);
 _dos_setvect( 8, vOldInt08 );
 _dos_setvect( 9, vOldInt09 );
 
    for( note = 0; song[note]; note += 2 )
        Beep( song[note], song[note + 1] );
 
 return 1;
} 

void asmGolomb( void )
{
// default c-compiler code (small model):        
// ENTER xxxx,x 
// PUSH DI
// PUSH SI 

time_t      timeh;
struct tm * tmh;
 
 _asm {
         PUSHA  
         CLD
         MOV   SI,psGol 
         MOV   BX,sIndex
startBase:                
         MOV   DX,pszTBase
         MOV   DI,DX
         MOV   CX,sMaxR
         XOR   AX,AX
         REP   STOSW
         MOV   DI,DX
         MOV   AX,BX
         MOV   DX,0    
 Add4:           
         MOV   BX,DX    
         MOV   CX,[SI+BX]
 Add3:             
         ADD   BX,2
         CMP   BX,AX
         JE    Add3
         
         MOV   BP,[SI+BX]
         SUB   BP,CX
         XOR   [DI+BP],BYTE PTR 1
                    
         CMP   BX,AX
         JBE   Add3
         
         ADD   DX,2
         CMP   DX,AX
         JB    Add4
         
         MOV   BX,AX
         JMP   SHORT start
 _sub:        
         INC   AX
         SUB   pszTBase,AX
         DEC   AX
         MOV   [SI+BX],AX
         SUB   BX,2            
start:         
         INC   WORD PTR [SI+BX]
         MOV   AX,sMax
         CMP   [SI+BX],AX
         JAE   _sub
         
         MOV   SI,pszTBase
         MOV   DX,pszTest
         MOV   DI,DX
         MOV   CX,sMaxR
         REP   MOVSW
         MOV   DI,DX
         MOV   SI,psGol
         MOV   DX,[SI+BX]
 Add1:
         MOV   BP,CX                         
         MOV   AX,[SI+BP]
         MOV   BP,DX
         SUB   BP,AX
         XOR   [DI+BP],BYTE PTR 1
         JZ    start 
         
         ADD   CX,2
         CMP   CX,BX
         JB    Add1
         
         ADD   CX,2
         MOV   BP,CX                         
         MOV   BP,[SI+BP]
         SUB   BP,DX
         XOR   [DI+BP],BYTE PTR 1
         JZ    start 
         
         CMP   BX,sCount
         JZ    Endgol
         
         MOV   AX,sMax
         MOV   [SI+BX+4],AX
         INC   AX
         ADD   pszTBase,AX
         DEC   AX
         MOV   CX,AX
         XOR   AL,AL
         MOV   DX,DI
         INC   DI
         REPNZ SCASB
         SUB   DI,DX
         SUB   DI,2
         MOV   AX,[SI+BX]
         ADD   AX,DI
         ADD   BX,2
         MOV   [SI+BX],AX
         MOV   DI,DX
         AND   BYTE PTR cGolWrite,1
         JNZ   write
         JMP   NEAR PTR startBase
 Endgol:
         JMP   NEAR PTR golout
 write:
         MOV   sIndex,BX
 } /* endasm */    

 if ( !sFile ) 
   fpA = fopen( szGolombB1, "wb" );
 else  
   fpA = fopen( szGolombB2, "wb" );
 sFile=!sFile;
 fwrite( &sCountR, 2, 1, fpA );
 fwrite( psGol, 2, sCountR,  fpA );
 fwrite( &sFlag, 2, 1, fpA );
 fwrite( &sIndex, 2, 1, fpA );
 fwrite( &sMax, 2, 1, fpA );
 fclose ( fpA );
 fpSta = fopen( szGolombStat, "at" );
 time ( &timeh );
 tmh = localtime ( &timeh );
 vFillBuffers();
 printf( "%s", pszLine );
 fprintf( fpSta, "%s", pszLine );
 printf( "\n%s%s", szCurrentTime, asctime( tmh ) );
 fprintf( fpSta,"\n%s%s", szCurrentTime, asctime( tmh ) );
 printf( "%s", szCurrentStatus );
 fprintf( fpSta,"%s", szCurrentStatus );
 printf( "\n%s", pszBuffer );
 fprintf( fpSta, "\n%s", pszBuffer );
 printf( "\n%s\n", pszLine );
 fprintf( fpSta, "\n%s\n", pszLine );
 fclose ( fpSta );
 cGolWrite=0;
 
 _asm {       
      MOV   BX,sIndex
      CMP   cGolEsc,1
      JZ    golout                                             
      JMP   NEAR PTR startBase
 golout:
      POPA
 } /* endasm */      
                
// default c-compiler code (small model):        
// POP DI
// POP SI
// LEAVE
// RETF

}         

#pragma optimize("t",on)

void   _cdecl _interrupt _far vNewInt08  ( void )
 {
 
 --sTime ? (cSem=1):(cSem=0);   
 if( cSem )
   _chain_intr( vOldInt08 );
 else {  
   (*vOldInt08)();     
   cSem=1;                  
   sTime=MAX_TIME;
   cGolWrite=1;
 } /* endelse */
}
                  
void   _cdecl _interrupt _far vNewInt09  ( void )
{         
     
           
  _asm {              
    push  si
    push  ax
    in    al,0x60
    cmp   al,1                                      //ESC
    je    escint9
    cmp   al,0x1C                                   //Ret
    je    retint9
    jmp   outint91
  
  escint9:
    mov   cGolEsc,1   
    MOV   SI,szEscape
    jmp   outint9
  
  retint9:  
    mov   cGolEsc,0  
    MOV   SI,szReturn
  
  outint9: 
    push  es
    push  di
    mov   ax,0xb800
    mov   es,ax
    MOV   DI,148
    MOV   CX,6
    REP   MOVSW
    pop   di
    pop   es
  outint91:
    pop   ax
    pop   si
  } /* endasm */  
  (*vOldInt09)();          

}

void vFillBuffers( void ) 
{     
short sL;       
short sA;       
  
 memset(pszBuffer, 0, BUF_LEN);
 for( sA=0; sA<sCountR; sA++ )
   sprintf( &pszBuffer[sA*4], "%4d", psGol[sA] );
 sL=strlen(pszBuffer);               
 if ( sL>=80 )
   sL=79;       
 memset( pszLine, 196, sL );
 pszLine[sL]=0;
}

void Beep( int frequency, int duration )
{
    int control;            
 
    /* If frequency is 0, Beep doesn't try to make a sound. It
     * just sleeps for the duration.
     */
    if( frequency )
    {
        /* 75 is about the shortest reliable duration of a sound. */
        if( duration < 75 )
            duration = 75;
 
        /* Prepare timer by sending 10111100 to port 43. */
        outp( 0x43, 0xb6 );
 
        /* Divide input frequency by timer ticks per second and
         * write (byte by byte) to timer.
         */
        frequency = (unsigned)(1193180L / frequency);
        outp( 0x42, (char)frequency );
        outp( 0x42, (char)(frequency >> 8) );
 
        /* Save speaker control byte. */
        control = inp( 0x61 );
 
        /* Turn on the speaker (with bits 0 and 1). */
        outp( 0x61, control | 0x3 );
    }
 
    Sleep( (clock_t)duration );
 
    /* Turn speaker back on if necessary. */
    if( frequency )
        outp( 0x61, control );
 
}
 
/* Pauses for a specified number of microseconds. */
void Sleep( clock_t wait )
{
    clock_t goal;
 
    goal = wait + clock();
    while( goal > clock() );

}
