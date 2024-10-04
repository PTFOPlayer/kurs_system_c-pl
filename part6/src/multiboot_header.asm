section .multiboot_header
header_start:
    ; liczba magiczna - odpowiada za wykorzystanie systemu multiboot 2
    dd 0xe85250d6                
    ; architektura 0 - tryb protected i386
    dd 0                         
    ; długość headera - używana przez multiboot do sprawdzenia sumy kontrolnej
    dd header_end - header_start 
    
    ; suma kontrolna - informuje bootloader o poprawnym działaniu
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    ; opcjonalne tagi (inocjacja FPU lub SSE)

    ; tagi końcowe
    dw 0    ; type
    dw 0    ; flags
    dd 8    ; size
header_end: