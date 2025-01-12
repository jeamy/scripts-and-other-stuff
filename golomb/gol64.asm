section .data
    ; Hier würden globale Variablen deklariert werden
    sMax: dq 0
    sCount: dq 0
    sMaxR: dq 0

section .bss
    psGol: resq 100  ; Annahme: Maximal 100 Einträge
    pszTest: resb 1024  ; Annahme: 1024 Bytes für den Test-String

section .text
global asmGolomb

asmGolomb:
    push rbp
    push rax
    push rbx
    push rcx
    push rdx

    mov rsi, [rel psGol]
    mov rdi, [rel pszTest]
    mov rbx, [rel sIndex]
    cld
    jmp short start

_sub:
    mov [rsi + rbx * 2], ax
    sub rbx, 1  ; In 64-Bit verwenden wir volle Register

start:
    inc word [rsi + rbx * 2]
    mov ax, [rel sMax]
    cmp [rsi + rbx * 2], ax
    jae _sub

    mov rcx, [rel sMaxR]
    mov rdx, rdi
    xor rax, rax
    rep stosw
    mov rdi, rdx
    xor rdx, rdx

Add2:
    mov rcx, rdx
    mov rbp, rdx
    mov ax, [rsi + rbp * 2]

Add1:
    add rcx, 1  ; In 64-Bit inkrementieren wir um 1, nicht um 2
    mov rbp, rcx
    mov bp, [rsi + rbp * 2]
    sub bp, ax
    xor byte [rdi + rbp], 1
    jz start

    cmp rcx, rbx
    jbe Add1

    add rdx, 1  ; In 64-Bit inkrementieren wir um 1, nicht um 2
    cmp rdx, rbx
    jbe Add2

    cmp rbx, [rel sCount]
    jz Endgol

    add rbx, 1  ; In 64-Bit inkrementieren wir um 1, nicht um 2
    mov ax, [rsi + rbx * 2]
    mov [rsi + rbx * 2 + 2], ax
    mov ax, [rsi + rbx * 2 - 2]
    sub ax, [rsi + rbx * 2]
    jns start
    neg ax
    cmp ax, [rel sMax]
    jb start
    mov [rel sMax], ax
    jmp start

Endgol:
    pop rdx
    pop rcx
    pop rbx
    pop rax
    pop rbp
    ret

write:
    mov [rel sIndex], rbx
    ret