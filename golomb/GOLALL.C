section .data
    prompt_ordnung db 'Ordnung: ', 0
    prompt_laenge db 'Länge: ', 0
    prompt_ergebnis db 'Ergebnis: ', 0
    filename_prefix db 'Golomb_', 0

section .bss
    ordnung resb 10
    laenge resb 10
    ergebnis resb 100
    filename resb 100

section .text
    global _start

_start:
    ; Prompt für die Ordnung
    mov eax, 4
    mov ebx, 1
    mov ecx, prompt_ordnung
    mov edx, 9
    int 0x80

    ; Lese die Ordnung
    mov eax, 3
    mov ebx, 0
    mov ecx, ordnung
    mov edx, 10
    int 0x80

    ; Prompt für die Länge
    mov eax, 4
    mov ebx, 1
    mov ecx, prompt_laenge
    mov edx, 8
    int 0x80

    ; Lese die Länge
    mov eax, 3
    mov ebx, 0
    mov ecx, laenge
    mov edx, 10
    int 0x80

    ; Berechne die Golomb-Linie
    mov ecx, ordnung
    mov edx, laenge
    call golomb_berechnen

    ; Prompt für das Ergebnis
    mov eax, 4
    mov ebx, 1
    mov ecx, prompt_ergebnis
    mov edx, 10
    int 0x80

    ; Schreibe das Ergebnis in die Konsole
    mov eax, 4
    mov ebx, 1
    mov ecx, ergebnis
    mov edx, 100
    int 0x80

    ; Erstelle den Dateinamen
    mov eax, filename_prefix
    mov ebx, laenge
    call erstelle_dateinamen

    ; Schreibe das Ergebnis in eine Datei
    mov eax, 8
    mov ebx, 0x400
    mov ecx, ergebnis
    mov edx, 100
    int 0x80

    ; Beende das Programm
    mov eax, 1
    xor ebx, ebx
    int 0x80

golomb_berechnen:
    ; Berechne die Golomb-Linie
    mov eax, 1
    mov [ergebnis], eax

    mov ecx, [ordnung]
    dec ecx
    jz .ende

.schleife:
    mov eax, [ergebnis]
    add eax, [ergebnis + ecx - 1]
    mov [ergebnis + ecx], eax
    loop .schleife

.ende:
    ret

erstelle_dateinamen:
    ; Erstelle den Dateinamen
    mov eax, filename_prefix
    mov ebx, laenge
    mov ecx, filename
    call strcpy

    ret

strcpy:
    ; Kopiere eine Zeichenkette
    mov eax, ecx
    mov ebx, edx
    mov ecx, eax
    mov edx, ebx
    mov eax, 0

.schleife:
    mov bl, [ecx]
    mov [edx], bl
    inc ecx
    inc edx
    cmp bl, 0
    jne .schleife

    ret