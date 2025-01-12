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

static char * pszTest; 
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

static  char  szJeamy[]=
{
"(c) JEAMY 07/1991"
};
static  char  szJeamy1[]=
{
"  GOLOMB by JEAMY#"
};                 
                    
void _nullcheck( void )
{
}

cdecl main ( )
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
						   
 //_clearscreen( _GCLEARSCREEN );
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

 //SJDecrypt( szJeamy1, strlen(szJeamy1) );
 //_settextcolor( 14 );
 //_settextposition( 1, 1 );
 //_outtext( szJeamy1 );
  
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
   psGol[3]=sMax;
   psGol[sCount-1]=sMax;
 } /* endif */

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

 //_settextcolor( 7 );
 //_setbkcolor( 0 );
 free(pszBuffer);
 _dos_setvect( 8, vOldInt08 );
 _dos_setvect( 9, vOldInt09 );
 
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
         PUSH  BP
         PUSH  AX
         PUSH  BX
         PUSH  CX
         PUSH  DX
         MOV   SI,psGol
         MOV   DI,pszTest
         MOV   BX,sIndex
         CLD
         JMP   SHORT start
 _sub:
         MOV   [SI+BX],AX
         SUB   BX,2
 start:                
         INC   WORD PTR [SI+BX]
         MOV   AX,sMax
         CMP   [SI+BX],AX
         JAE   _sub
         
         MOV   CX,sMaxR
         MOV   DX,DI
         XOR   AX,AX
         REP   STOSW
         MOV   DI,DX
 Add2:
         ADD   CX,2
         MOV   DX,0FFFEh               
 Add1:
         ADD   DX,2                          
         MOV   BP,DX                         
         MOV   AX,[SI+BP]
         ADD   BP,CX
         MOV   BP,[SI+BP]
         SUB   BP,AX
         XOR   [DI+BP],BYTE PTR 1
         JZ    start 
         
         MOV   AX,CX
         ADD   AX,DX
         CMP   AX,BX
         JBE   Add1

         CMP   CX,BX
         JBE   Add2
                     
         CMP   BX,sCount
         JZ    Endgol
         
         XOR   AL,AL
         MOV   CX,sMax
         MOV   DX,DI
         INC   DI
         REPNZ SCASB
         SUB   DI,DX
         SUB   DI,2
         ADD   BX,2
         MOV   AX,[SI+BX]
         MOV   [SI+BX+2],AX
         MOV   AX,[SI+BX-2]
         ADD   AX,DI
         MOV   [SI+BX],AX
         MOV   DI,DX
         AND   BYTE PTR cGolWrite,1
         JZ    start               
         JMP   NEAR PTR write
 Endgol:
         POP   DX
         POP   CX
         POP   BX
         POP   AX
         POP   BP
         JMP   golout
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
      JZ    golend                                             
      JMP   NEAR PTR start
 golend:
      POP   DX
      POP   CX
      POP   BX
      POP   AX
      POP   BP
 golout:
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
    push ax
    in   al,0x60
    cmp  al,1                                      //ESC
    je   escint9
    cmp  al,0x1C                                   //Ret
    je   retint9
    jmp  outint9
  
  escint9:                                                   
    mov  cGolEsc,1  
    push es
    mov  ax,0xb800
    mov  es,ax
    mov  es:[148],'E'
    mov  es:[149],17h
    mov  es:[150],'S'
    mov  es:[151],17h
    mov  es:[152],'C'
    mov  es:[153],17h
    mov  es:[154],'A'
    mov  es:[155],17h
    mov  es:[156],'P'
    mov  es:[157],17h
    mov  es:[158],'E'
    mov  es:[159],17h
    pop  es     
    jmp  outint9
  retint9:  
    mov  cGolEsc,0  
    push es
    mov  ax,0xb800
    mov  es,ax
    mov  es:[148],' '
    mov  es:[149],7h
    mov  es:[150],' '
    mov  es:[151],7h
    mov  es:[152],' '
    mov  es:[153],7h
    mov  es:[154],' '       
    mov  es:[155],7h
    mov  es:[156],' '
    mov  es:[157],7h
    mov  es:[158],' '
    mov  es:[159],7h
    pop  es     
  outint9: 
    pop   ax
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
