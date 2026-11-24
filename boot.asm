[bits 16]
[org 0x7c00]

KERNEL_OFFSET equ 0x1000 ; Адрес в памяти, куда загрузим ядро

start:
    mov [BOOT_DRIVE], dl ; Запоминаем номер диска, с которого загрузились

    ; Настраиваем стек
    mov bp, 0x9000
    mov sp, bp

    ; Читаем ядро с диска (15 секторов) в память по адресу 0x1000
    mov bx, KERNEL_OFFSET
    mov dh, 15
    mov dl, [BOOT_DRIVE]
    call disk_load

    ; Переходим в 32-битный защищенный режим
    cli                     ; Отключаем прерывания BIOS
    lgdt [gdt_descriptor]   ; Загружаем таблицу дескрипторов GDT
    mov eax, cr0
    or eax, 0x1             ; Включаем бит защищенного режима
    mov cr0, eax
    jmp CODE_SEG:init_pm    ; Дальний прыжок для очистки конвейера процессора

; Функция чтения секторов с диска через BIOS
disk_load:
    push dx
    mov ah, 0x02 ; Функция чтения BIOS
    mov al, dh   ; Количество секторов
    mov ch, 0x00 ; Цилиндр 0
    mov dh, 0x00 ; Головка 0
    mov cl, 0x02 ; Начинаем со 2-го сектора (1-й — сам загрузчик)
    int 0x13     ; Вызов прерывания диска
    pop dx
    ret

; Таблица GDT (описывает всю память как доступную для ядра)
gdt_start:
gdt_null:
    dd 0x0
    dd 0x0
gdt_code:
    dw 0xffff, 0x0
    db 0x0, 10011010b, 11001111b, 0x0
gdt_data:
    dw 0xffff, 0x0
    db 0x0, 10010010b, 11001111b, 0x0
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

[bits 32]
init_pm:
    ; Настраиваем сегментные регистры для 32-битного режима
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x90000
    mov esp, ebp

    ; Передаем управление нашему ядру на C!
    call KERNEL_OFFSET
    jmp $

BOOT_DRIVE db 0

; Добиваем файл нулями до 510 байт и ставим сигнатуру загрузочного сектора
times 510 - ($ - $$) db 0
dw 0xaa55
