	 .MODEL small		
	 .DATA
psGol	 DW ?
pszTest  DW ?
sIndex   DW 4
pszTBase DW ?
sMaxR    DW 7
sMax     DW 85
sCount   DW 12
cGolWrite DB 1
         .CODE

start1:
         PUSH  BP
         PUSH  AX
         PUSH  BX
         PUSH  CX
         PUSH  DX
         MOV   SI,psGol
         MOV   DI,pszTest
         MOV   BX,sIndex
         CLD
startBase:         
         MOV   DI,pszTBase
         MOV   CX,sMaxR
         XOR   AX,AX
         REP   STOSW
         MOV   DI,pszTBase
         MOV   DX,0    
 Add4:              
         MOV   CX,DX
         MOV   BP,DX
         MOV   AX,[SI+BP]
 Add3:             
         ADD   CX,2
         CMP   CX,BX
         JNE   Add5
         ADD   CX,2
 Add5:        
         MOV   BP,CX
         MOV   BP,[SI+BP]
         SUB   BP,AX
         XOR   [DI+BP],BYTE PTR 1
                    
         CMP   CX,BX
         JBE   Add3
         
         ADD   DX,2
         CMP   DX,BX
         JB    Add4
         JMP   SHORT start
 _sub:
         SUB   pszTBase,AX
         DEC   pszTBase
         MOV   [SI+BX],AX
         SUB   BX,2            
start:         
         INC   WORD PTR [SI+BX]
         MOV   AX,sMax
         CMP   [SI+BX],AX
         JAE   _sub
         
         MOV   SI,pszTBase
         MOV   DI,pszTest
         MOV   CX,sMaxR
         REP   MOVSW
         MOV   DI,pszTest
         MOV   SI,psGol
 Add1:
         MOV   AX,[SI+BX]
         MOV   BP,CX                         
         MOV   BP,[SI+BP]
         SUB   AX,BP
         MOV   BP,AX
         XOR   [DI+BP],BYTE PTR 1
         JZ    start 
         
         ADD   CX,2
         CMP   CX,BX
         JB    Add1
         
         MOV   AX,[SI+BX]
         ADD   CX,2
         MOV   BP,CX                         
         MOV   BP,[SI+BP]
         SUB   BP,AX
         XOR   [DI+BP],BYTE PTR 1
         JZ    start 
         
         CMP   BX,sCount
         JZ    Endgol
         
         XOR   AL,AL
         MOV   CX,sMax
         ADD   pszTBase,CX
         INC   pszTBase
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
         JNZ   notStartBase
         JMP   NEAR PTR startBase
 notStartBase:        
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
 golout:
         END   start1
 