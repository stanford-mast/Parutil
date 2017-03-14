;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Paratool
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
INCLUDE spindle.inc


_TEXT                                       SEGMENT


; --------- MACROS ------------------------------------------------------------

paratoolMemoryOpInitializeThread            MACRO
    ; Extract thread information useful as loop controls, assigning chunks to each thread round-robin.
    ; Number of iterations is equal to the number of 64-byte blocks, passed in as r_param3 and held in r13.
    ; Formulas:
    ;    assignment  = #iterations / #total_threads
    ;    addon       = #iterations % #total_threads < global_thread_id ? 1 : 0
    ;    prev_addons = min(#iterations % #total_threads, global_thread_id)
    ;
    ;    base (rsi)  = (assignment * global_thread_id) + prev_addons
    ;    inc         = 1
    ;    max  (rdi)  = base + assignment + addon - 1
    
    ; First, perform the unsigned division by setting rdx:rax = #iterations and dividing by #total_threads.
    ; Afterwards, rax contains the quotient ("assignment" in the formulas above) and rdx contains the remainder.
    mov                     rax,                    r13
    xor                     rdx,                    rdx
    xor                     rcx,                    rcx
    spindleAsmHelperGetLocalThreadCount             ecx
    div                     rcx
    
    ; To calculate other values using total_threads, extract it to rcx.
    ; This can be used directly to obtain "addon" (rbx) and "prev_addons" (rsi).
    spindleAsmHelperGetLocalThreadID                ecx
    xor                     rbx,                    rbx
    mov                     rsi,                    rdx
    mov                     rdi,                    0000000000000001h
    cmp                     rcx,                    rdx
    cmovl                   rbx,                    rdi
    cmovl                   rsi,                    rcx
    
    ; Create some partial values using the calculated quantities.
    ;    rsi (base) = prev_addons - this was done above, rdi (max) = assignment + addon - 1.
    ; Note that because we are using "jge" below and not "jg", we skip the -1, since "jge" requires that rdi be (last index to process + 1).
    mov                     rdi,                    rax
    add                     rdi,                    rbx
    
    ; Perform multiplication of assignment * total_threads, result in rax.
    ; Use the result to add to rsi and figure out "base", then add to rdi to get "max".
    mul                     rcx
    add                     rsi,                    rax
    add                     rdi,                    rsi
ENDM

; --------- FUNCTIONS ---------------------------------------------------------
; See "memorycopy.h" for documentation.

paratoolMemoryCopyAlignedThread             PROC PUBLIC
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
    paratoolMemoryOpInitializeThread
    
    ; Perform the memory copy operation assigned to this thread.
  paratoolMemoryCopyAlignedThreadLoop:
    cmp                     rsi,                    rdi
    jge                     paratoolMemoryCopyAlignedThreadDone
    
    ; Compute the byte offset of the 64-byte block.
    ; This is equal to the iteration index multiplied by 64, or left-shifted by 6.
    mov                     rcx,                    rsi
    shl                     rcx,                    6
    
    ; Perform the memory-copy operation.
    ; Since there is no locality at all, use non-temporal hints.
    vmovntdqa               ymm0,                   YMMWORD PTR [r11+rcx]
    vmovntdqa               ymm1,                   YMMWORD PTR [r11+rcx+32]
    vmovntdq                YMMWORD PTR [r12+rcx],                          ymm0
    vmovntdq                YMMWORD PTR [r12+rcx+32],                       ymm1
    
    inc                     rsi
    jmp                     paratoolMemoryCopyAlignedThreadLoop
  paratoolMemoryCopyAlignedThreadDone:
    
    ; Restore non-volatile registers and return.
    pop                     r13
    pop                     r12
    pop                     r11
    pop                     rdi
    pop                     rsi
    pop                     rbx

    ret
paratoolMemoryCopyAlignedThread             ENDP

; ---------

paratoolMemoryCopyUnalignedThread           PROC PUBLIC
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
    paratoolMemoryOpInitializeThread
    
    ; Perform the memory copy operation assigned to this thread.
  paratoolMemoryCopyUnalignedThreadLoop:
    cmp                     rsi,                    rdi
    jge                     paratoolMemoryCopyUnalignedThreadDone
    
    ; Compute the byte offset of the 8-byte block.
    ; This is equal to the iteration index multiplied by 8, or left-shifted by 3.
    mov                     rcx,                    rsi
    shl                     rcx,                    3
    
    ; Perform the memory-copy operation, one 64-bit integer at a time.
    mov                     rax,                    QWORD PTR [r11+rcx]
    movnti                  QWORD PTR [r12+rcx],    rax
    
    inc                     rsi
    jmp                     paratoolMemoryCopyUnalignedThreadLoop
  paratoolMemoryCopyUnalignedThreadDone:
    
    ; Restore non-volatile registers and return.
    pop                     r13
    pop                     r12
    pop                     r11
    pop                     rdi
    pop                     rsi
    pop                     rbx

    ret
paratoolMemoryCopyUnalignedThread           ENDP

; ---------

paratoolMemorySetAlignedThread              PROC PUBLIC
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
    paratoolMemoryOpInitializeThread
    
    ; Create the 256-bit value to be written to memory.
    vmovq                   xmm0,                   r12
    vpbroadcastq            ymm1,                   xmm0
    
    ; Perform the memory copy operation assigned to this thread.
  paratoolMemorySetAlignedThreadLoop:
    cmp                     rsi,                    rdi
    jge                     paratoolMemorySetAlignedThreadDone
    
    ; Compute the byte offset of the 64-byte block.
    ; This is equal to the iteration index multiplied by 64, or left-shifted by 6.
    mov                     rcx,                    rsi
    shl                     rcx,                    6
    
    ; Perform the memory-set operation.
    ; Since there is no locality at all, use non-temporal hints.
    vmovntdq                YMMWORD PTR [r11+rcx],                          ymm1
    vmovntdq                YMMWORD PTR [r11+rcx+32],                       ymm1
    
    inc                     rsi
    jmp                     paratoolMemorySetAlignedThreadLoop
  paratoolMemorySetAlignedThreadDone:
    
    ; Restore non-volatile registers and return.
    pop                     r13
    pop                     r12
    pop                     r11
    pop                     rdi
    pop                     rsi
    pop                     rbx

    ret
paratoolMemorySetAlignedThread              ENDP


_TEXT                                       ENDS


END
