;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Parutil
;   Multi-platform library of parallelized utility functions.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Authored by Samuel Grossman
; Department of Electrical Engineering, Stanford University
; Copyright (c) 2016-2017
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; registers.inc
;   Name mappings for registers used throughout this library.
;   Abstracts away platform-specific calling conventions.
;   Otherwise improves readability of assembly code.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

IFNDEF __PARUTIL_REGISTERS_INC
__PARUTIL_REGISTERS_INC EQU 1


; --------- PLATFORM-SPECIFIC FUNCTION CALLING CONVENTIONS --------------------
; Windows and Linux use different registers for integer parameters and return values.
; These definitions abstract this away.

IFDEF PARUTIL_WINDOWS
; 64-bit parameter and return value registers
r64_param1                                  TEXTEQU     <rcx>
r64_param2                                  TEXTEQU     <rdx>
r64_param3                                  TEXTEQU     <r8>
r64_param4                                  TEXTEQU     <r9>
r64_retval                                  TEXTEQU     <rax>

; 64-bit caller-saved scratch registers
r64_scratch1                                TEXTEQU     <r10>
r64_scratch2                                TEXTEQU     <r11>

; 64-bit callee-saved registers
r64_savescr1                                TEXTEQU     <rbx>
r64_savescr2                                TEXTEQU     <r12>
r64_savescr3                                TEXTEQU     <r13>
r64_savescr4                                TEXTEQU     <r14>
r64_savescr5                                TEXTEQU     <r15>

; 32-bit parameter and return value registers
r32_param1                                  TEXTEQU     <ecx>
r32_param2                                  TEXTEQU     <edx>
r32_param3                                  TEXTEQU     <r8d>
r32_param4                                  TEXTEQU     <r9d>
r32_retval                                  TEXTEQU     <eax>

; 32-bit caller-saved scratch registers
r32_scratch1                                TEXTEQU     <r10d>
r32_scratch2                                TEXTEQU     <r11d>

; 32-bit callee-saved registers
r32_savescr1                                TEXTEQU     <ebx>
r32_savescr2                                TEXTEQU     <r12d>
r32_savescr3                                TEXTEQU     <r13d>
r32_savescr4                                TEXTEQU     <r14d>
r32_savescr5                                TEXTEQU     <r15d>

; 16-bit parameter and return value registers (can be used as caller-saved scratch registers)
r16_param1                                  TEXTEQU     <cx>
r16_param2                                  TEXTEQU     <dx>
r16_param3                                  TEXTEQU     <r8w>
r16_param4                                  TEXTEQU     <r9w>
r16_retval                                  TEXTEQU     <ax>

; 16-bit caller-saved scratch registers
r16_scratch1                                TEXTEQU     <r10w>
r16_scratch2                                TEXTEQU     <r11w>

; 16-bit callee-saved registers
r16_savescr1                                TEXTEQU     <bx>
r16_savescr2                                TEXTEQU     <r12w>
r16_savescr3                                TEXTEQU     <r13w>
r16_savescr4                                TEXTEQU     <r14w>
r16_savescr5                                TEXTEQU     <r15w>

; 8-bit parameter and return value registers (can be used as caller-saved scratch registers)
r8_param1                                   TEXTEQU     <cl>
r8_param2                                   TEXTEQU     <dl>
r8_param3                                   TEXTEQU     <r8b>
r8_param4                                   TEXTEQU     <r9b>
r8_retval                                   TEXTEQU     <al>

; 8-bit caller-saved scratch registers
r8_scratch1                                 TEXTEQU     <r10b>
r8_scratch2                                 TEXTEQU     <r11b>

; 8-bit callee-saved registers
r8_savescr1                                 TEXTEQU     <bl>
r8_savescr2                                 TEXTEQU     <r12b>
r8_savescr3                                 TEXTEQU     <r13b>
r8_savescr4                                 TEXTEQU     <r14b>
r8_savescr5                                 TEXTEQU     <r15b>
ENDIF

IFDEF PARUTIL_LINUX
; 64-bit parameter and return value registers
r64_param1                                  TEXTEQU     <rdi>
r64_param2                                  TEXTEQU     <rsi>
r64_param3                                  TEXTEQU     <rdx>
r64_param4                                  TEXTEQU     <rcx>
r64_retval                                  TEXTEQU     <rax>

; 64-bit caller-saved scratch registers
r64_scratch1                                TEXTEQU     <r8>
r64_scratch2                                TEXTEQU     <r9>

; 64-bit callee-saved registers
r64_savescr1                                TEXTEQU     <rbx>
r64_savescr2                                TEXTEQU     <r12>
r64_savescr3                                TEXTEQU     <r13>
r64_savescr4                                TEXTEQU     <r14>
r64_savescr5                                TEXTEQU     <r15>

; 32-bit parameter and return value registers
r32_param1                                  TEXTEQU     <edi>
r32_param2                                  TEXTEQU     <esi>
r32_param3                                  TEXTEQU     <edx>
r32_param4                                  TEXTEQU     <ecx>
r32_retval                                  TEXTEQU     <eax>

; 32-bit caller-saved scratch registers
r32_scratch1                                TEXTEQU     <r8d>
r32_scratch2                                TEXTEQU     <r9d>

; 32-bit callee-saved registers
r32_savescr1                                TEXTEQU     <ebx>
r32_savescr2                                TEXTEQU     <r12d>
r32_savescr3                                TEXTEQU     <r13d>
r32_savescr4                                TEXTEQU     <r14d>
r32_savescr5                                TEXTEQU     <r15d>

; 16-bit parameter and return value registers (can be used as caller-saved scratch registers)
r16_param1                                  TEXTEQU     <di>
r16_param2                                  TEXTEQU     <si>
r16_param3                                  TEXTEQU     <dx>
r16_param4                                  TEXTEQU     <cx>
r16_retval                                  TEXTEQU     <ax>

; 16-bit caller-saved scratch registers
r16_scratch1                                TEXTEQU     <r8w>
r16_scratch2                                TEXTEQU     <r9w>

; 16-bit callee-saved registers
r16_savescr1                                TEXTEQU     <bx>
r16_savescr2                                TEXTEQU     <r12w>
r16_savescr3                                TEXTEQU     <r13w>
r16_savescr4                                TEXTEQU     <r14w>
r16_savescr5                                TEXTEQU     <r15w>

; 8-bit parameter and return value registers (can be used as caller-saved scratch registers)
r8_param1                                   TEXTEQU     <dil>
r8_param2                                   TEXTEQU     <sil>
r8_param3                                   TEXTEQU     <dl>
r8_param4                                   TEXTEQU     <cl>
r8_retval                                   TEXTEQU     <al>

; 8-bit caller-saved scratch registers
r8_scratch1                                 TEXTEQU     <r8b>
r8_scratch2                                 TEXTEQU     <r9b>

; 8-bit callee-saved registers
r8_savescr1                                 TEXTEQU     <bl>
r8_savescr2                                 TEXTEQU     <r12b>
r8_savescr3                                 TEXTEQU     <r13b>
r8_savescr4                                 TEXTEQU     <r14b>
r8_savescr5                                 TEXTEQU     <r15b>
ENDIF


ENDIF ;__PARUTIL_REGISTERS_INC
