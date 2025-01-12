section .data
    ordnung db 11 ; Ordnung der Golomb-Linie
    laenge db 72 ; Länge der Golomb-Linie

section .bss
    golomb resb 100 ; Speicher für die Golomb-Linie

section .text
    global _start

_start:
    ; Berechne die Golomb-Linie
    mov eax, ordnung
    mov ebx, laenge
    call golomb_berechnen

    ; Ausgabe der Golomb-Linie
    mov eax, 4
    mov ebx, 1
    mov ecx, golomb
    mov edx, 100
    int 0x80

    ; Beende das Programm
    mov eax, 1
    xor ebx, ebx
    int 0x80

golomb_berechnen:
    ; Berechne die Golomb-Linie
    mov ecx, eax ; Ordnung
    mov edx, ebx ; Länge
    mov eax, 1 ; Anfangswert für die Golomb-Linie

.schleife:
    ; Berechne den nächsten Wert der Golomb-Linie
    mov ebx, eax
    add ebx, eax ; ebx = eax + eax
    mov [golomb + ecx - 1], ebx ; Speichere den Wert in der Golomb-Linie
    inc ecx ; Inkrementiere die Ordnung
    loop .schleife

    ret