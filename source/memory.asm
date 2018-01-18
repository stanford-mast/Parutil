;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Parutil
;   Multi-platform library of parallelized utility functions.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Authored by Samuel Grossman
; Department of Electrical Engineering, Stanford University
; Copyright (c) 2016-2017
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; memory.asm
;   Implementation of internal memory operation functionality.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

INCLUDE registers.inc
INCLUDE scheduler.inc
INCLUDE spindle.inc


_TEXT                                       SEGMENT


; --------- FUNCTIONS ---------------------------------------------------------
; See "memory.h" for documentation.

parutilMemoryCopyAlignedThread              PROC PUBLIC
    ; Save non-volatile registers.
    push                    rbx
    push                    rsi
    push                    rdi
    push                    r11
    push                    r12
    push                    r13
    
    ; Set aside the original parameters.
    mov                     r11,                    r_param1
    mov                     r12,                    r_param2
    mov                     r13,                    r_param3
    
    ; Initialize.
    parutilSchedulerInitStaticChunk
    
    ; Perform the memory copy operation assigned to this thread.
  parutilMemoryCopyAlignedThreadLoop:
    cmp                     rsi,                    rdi
    jge                     parutilMemoryCopyAlignedThreadDone
    
    ; Compute the byte offset of the 64-byte block.
    ; This is equal to the iteration index multiplied by 64, or left-shifted by 6.
    mov                     rcx,                    rsi
    shl                     rcx,                    6
    
    ; Perform the memory-copy operation.
    ; Since there is no locality at all, use non-temporal hints.
    vmovntdqa               ymm0,                   YMMWORD PTR [r12+rcx]
    vmovntdqa               ymm1,                   YMMWORD PTR [r12+rcx+32]
    vmovntdq                YMMWORD PTR [r11+rcx],                          ymm0
    vmovntdq                YMMWORD PTR [r11+rcx+32],                       ymm1
    
    inc                     rsi
    jmp                     parutilMemoryCopyAlignedThreadLoop
  parutilMemoryCopyAlignedThreadDone:
    
    ; Restore non-volatile registers and return.
    pop                     r13
    pop                     r12
    pop                     r11
    pop                     rdi
    pop                     rsi
    pop                     rbx

    ret
parutilMemoryCopyAlignedThread              ENDP

; ---------

parutilMemoryCopyUnalignedThread            PROC PUBLIC
    ; Save non-volatile registers.
    push                    rbx
    push                    rsi
    push                    rdi
    push                    r11
    push                    r12
    push                    r13
    
    ; To translate from 64-byte blocks to 8-byte blocks, multiply the number of blocks by 8.
    shl                     r_param3,               3
    
    ; Set aside the original parameters.
    mov                     r11,                    r_param1
    mov                     r12,                    r_param2
    mov                     r13,                    r_param3
    
    ; Initialize.
    parutilSchedulerInitStaticChunk
    
    ; Perform the memory copy operation assigned to this thread.
  parutilMemoryCopyUnalignedThreadLoop:
    cmp                     rsi,                    rdi
    jge                     parutilMemoryCopyUnalignedThreadDone
    
    ; Compute the byte offset of the 8-byte block.
    ; This is equal to the iteration index multiplied by 8, or left-shifted by 3.
    mov                     rcx,                    rsi
    shl                     rcx,                    3
    
    ; Perform the memory-copy operation, one 64-bit integer at a time.
    mov                     rax,                    QWORD PTR [r12+rcx]
    movnti                  QWORD PTR [r11+rcx],    rax
    
    inc                     rsi
    jmp                     parutilMemoryCopyUnalignedThreadLoop
  parutilMemoryCopyUnalignedThreadDone:
    
    ; Restore non-volatile registers and return.
    pop                     r13
    pop                     r12
    pop                     r11
    pop                     rdi
    pop                     rsi
    pop                     rbx

    ret
parutilMemoryCopyUnalignedThread            ENDP

; ---------

parutilMemorySetAlignedThread               PROC PUBLIC
    ; Save non-volatile registers.
    push                    rbx
    push                    rsi
    push                    rdi
    push                    r11
    push                    r12
    push                    r13
    
    ; Set aside the original parameters.
    mov                     r11,                    r_param1
    mov                     r12,                    r_param2
    mov                     r13,                    r_param3
    
    ; Initialize.
    parutilSchedulerInitStaticChunk
    
    ; Create the 256-bit value to be written to memory.
    vmovq                   xmm0,                   r12
    vpbroadcastq            ymm1,                   xmm0
    
    ; Perform the memory copy operation assigned to this thread.
  parutilMemorySetAlignedThreadLoop:
    cmp                     rsi,                    rdi
    jge                     parutilMemorySetAlignedThreadDone
    
    ; Compute the byte offset of the 64-byte block.
    ; This is equal to the iteration index multiplied by 64, or left-shifted by 6.
    mov                     rcx,                    rsi
    shl                     rcx,                    6
    
    ; Perform the memory-set operation.
    ; Since there is no locality at all, use non-temporal hints.
    vmovntdq                YMMWORD PTR [r11+rcx],                          ymm1
    vmovntdq                YMMWORD PTR [r11+rcx+32],                       ymm1
    
    inc                     rsi
    jmp                     parutilMemorySetAlignedThreadLoop
  parutilMemorySetAlignedThreadDone:
    
    ; Restore non-volatile registers and return.
    pop                     r13
    pop                     r12
    pop                     r11
    pop                     rdi
    pop                     rsi
    pop                     rbx

    ret
parutilMemorySetAlignedThread               ENDP


_TEXT                                       ENDS


END
