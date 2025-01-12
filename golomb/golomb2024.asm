section .data
    order_prompt db "Enter the order of the Golomb ruler: ", 0
    length_prompt db "Enter the maximum length of the ruler: ", 0
    result_msg db "Golomb ruler: ", 0
    file_msg db "Results written to golomb.txt", 10, 0
    error_msg db "Failed to find a valid Golomb ruler", 10, 0
    input_error_msg db "Invalid input. Please enter a positive integer.", 10, 0
    filename db "golomb.txt", 0
    space db " ", 0
    newline db 10, 0

section .bss
    order resb 4
    max_length resb 4
    ruler resb 400    ; Increased size to support larger rulers
    buffer resb 20    ; Buffer for integer to string conversion
    fd resb 4         ; File descriptor

section .text
    global _start

_start:
    ; Open file for writing
    mov eax, 5        ; sys_open
    mov ebx, filename
    mov ecx, 0o641    ; O_WRONLY | O_CREAT | O_TRUNC, mode 0641
    int 0x80
    mov [fd], eax     ; Save file descriptor

    ; Prompt for order
    mov eax, 4
    mov ebx, 1
    mov ecx, order_prompt
    mov edx, 37
    int 0x80

    ; Read order
    mov eax, 3
    mov ebx, 0
    mov ecx, order
    mov edx, 4
    int 0x80

    ; Convert order to integer
    mov eax, 0
    mov ebx, order
    call atoi
    cmp eax, 0
    jle .input_error
    cmp eax, 100      ; Limit the maximum order to prevent buffer overflow
    jg .input_error
    mov [order], eax

    ; Prompt for maximum length
    mov eax, 4
    mov ebx, 1
    mov ecx, length_prompt
    mov edx, 39
    int 0x80

    ; Read maximum length
    mov eax, 3
    mov ebx, 0
    mov ecx, max_length
    mov edx, 4
    int 0x80

    ; Convert max_length to integer
    mov eax, 0
    mov ebx, max_length
    call atoi
    cmp eax, 0
    jle .input_error
    mov [max_length], eax

    ; Calculate Golomb ruler
    mov eax, [order]
    mov ebx, [max_length]
    mov ecx, ruler
    call calculate_golomb_ruler

    ; Check if a valid ruler was found
    cmp eax, 0
    je .error

    ; Write result to file
    mov eax, 4
    mov ebx, [fd]
    mov ecx, result_msg
    mov edx, 14
    int 0x80

    mov eax, ruler
    mov ebx, [order]
    call write_ruler_to_file

    jmp .close_file

.input_error:
    ; Write input error message to file
    mov eax, 4
    mov ebx, [fd]
    mov ecx, input_error_msg
    mov edx, 46
    int 0x80
    jmp .close_file

.error:
    ; Write error message to file
    mov eax, 4
    mov ebx, [fd]
    mov ecx, error_msg
    mov edx, 37
    int 0x80

.close_file:
    ; Close file
    mov eax, 6        ; sys_close
    mov ebx, [fd]
    int 0x80

    ; Print confirmation message
    mov eax, 4
    mov ebx, 1
    mov ecx, file_msg
    mov edx, 32
    int 0x80

    ; Exit program
    mov eax, 1
    xor ebx, ebx
    int 0x80

; Function to calculate Golomb ruler
; eax: order, ebx: max_length, ecx: ruler address
; Returns: eax: 1 if successful, 0 if failed
calculate_golomb_ruler:
    push ebp
    mov ebp, esp
    push esi
    push edi
    push ebx

    mov esi, ecx      ; esi points to ruler
    xor edi, edi      ; edi is current position
    mov dword [esi], 0 ; First mark is always 0
    mov dword [esi + 4], 1 ; Second mark is always 1

    mov ecx, 2        ; Start from third mark

.loop:
    cmp ecx, eax
    jae .success

    inc edi
    cmp edi, [esp]    ; Compare with max_length (stored in pushed ebx)
    ja .backtrack

    mov [esi + ecx * 4], edi
    
    push ecx
    push eax
    call is_valid_ruler
    pop eax
    pop ecx

    test al, al
    jnz .next_mark

.backtrack:
    dec ecx
    cmp ecx, 1
    jbe .failure
    mov edi, [esi + ecx * 4]
    jmp .loop

.next_mark:
    inc ecx
    jmp .loop

.success:
    mov eax, 1
    jmp .done

.failure:
    xor eax, eax

.done:
    pop ebx
    pop edi
    pop esi
    mov esp, ebp
    pop ebp
    ret

; Function to check if current ruler is valid
; esi: ruler, ecx: current size
is_valid_ruler:
    push ebp
    mov ebp, esp
    push esi
    push edi
    push ebx

    mov edi, 1        ; Start from second mark

.outer_loop:
    cmp edi, ecx
    jae .valid

    mov edx, 0        ; Start from first mark
.inner_loop:
    cmp edx, edi
    jae .next_outer

    mov eax, [esi + edi * 4]
    sub eax, [esi + edx * 4]  ; Calculate difference

    push ecx
    push edi
    xor edi, edi      ; Start checking from beginning
.check_unique:
    cmp edi, edx
    jae .unique

    mov ebx, [esi + edi * 4 + 4]
    sub ebx, [esi + edi * 4]
    cmp eax, ebx
    je .not_unique

    inc edi
    jmp .check_unique

.unique:
    pop edi
    pop ecx
    inc edx
    jmp .inner_loop

.not_unique:
    pop edi
    pop ecx
    xor eax, eax      ; Return false
    jmp .done

.next_outer:
    inc edi
    jmp .outer_loop

.valid:
    mov eax, 1        ; Return true

.done:
    pop ebx
    pop edi
    pop esi
    mov esp, ebp
    pop ebp
    ret

; Function to write the ruler to file
; eax: ruler address, ebx: ruler size
write_ruler_to_file:
    push ebp
    mov ebp, esp
    push esi
    push edi

    mov esi, eax
    xor edi, edi

.write_loop:
    cmp edi, ebx
    jae .done

    mov eax, [esi + edi * 4]
    call int_to_str

    push eax          ; Save string length
    mov eax, 4
    mov ebx, [fd]
    mov ecx, buffer
    pop edx           ; Restore string length
    int 0x80

    mov eax, 4
    mov ebx, [fd]
    mov ecx, space
    mov edx, 1
    int 0x80

    inc edi
    jmp .write_loop

.done:
    mov eax, 4
    mov ebx, [fd]
    mov ecx, newline
    mov edx, 1
    int 0x80

    pop edi
    pop esi
    mov esp, ebp
    pop ebp
    ret

; Function to convert integer to string
; eax: integer to convert
; Returns: eax: length of string
int_to_str:
    push ebp
    mov ebp, esp
    push esi
    push edi

    mov esi, buffer
    add esi, 19
    mov byte [esi], 0
    mov ebx, 10
    xor ecx, ecx      ; String length counter

.convert_loop:
    xor edx, edx
    div ebx
    add dl, '0'
    dec esi
    mov [esi], dl
    inc ecx
    test eax, eax
    jnz .convert_loop

    ; Move the string to the beginning of the buffer
    mov edi, buffer
    mov eax, ecx
.move_loop:
    mov dl, [esi]
    mov [edi], dl
    inc esi
    inc edi
    loop .move_loop

    mov eax, ecx      ; Return length of string

    pop edi
    pop esi
    mov esp, ebp
    pop ebp
    ret

; Function to convert ASCII to integer
atoi:
    xor eax, eax
.top:
    movzx ecx, byte [ebx]
    inc ebx
    cmp ecx, '0'
    jb .done
    cmp ecx, '9'
    ja .done
    sub ecx, '0'
    imul eax, 10
    add eax, ecx
    jmp .top
.done:
    ret