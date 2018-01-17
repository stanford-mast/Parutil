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
r_param1                                    TEXTEQU     <rcx>
r_param2                                    TEXTEQU     <rdx>
r_param3                                    TEXTEQU     <r8>
r_param4                                    TEXTEQU     <r9>
r_retval                                    TEXTEQU     <rax>

; 64-bit caller-saved scratch registers
r_scratch1                                  TEXTEQU     <r10>
r_scratch2                                  TEXTEQU     <r11>

; 64-bit callee-saved registers
r_savescr1                                  TEXTEQU     <rbx>
r_savescr2                                  TEXTEQU     <r12>
r_savescr3                                  TEXTEQU     <r13>
r_savescr4                                  TEXTEQU     <r14>
r_savescr5                                  TEXTEQU     <r15>

; 32-bit parameter and return value registers
e_param1                                    TEXTEQU     <ecx>
e_param2                                    TEXTEQU     <edx>
e_param3                                    TEXTEQU     <r8d>
e_param4                                    TEXTEQU     <r9d>
e_retval                                    TEXTEQU     <eax>

; 32-bit caller-saved scratch registers
e_scratch1                                  TEXTEQU     <r10d>
e_scratch2                                  TEXTEQU     <r11d>

; 32-bit callee-saved registers
e_savescr1                                  TEXTEQU     <ebx>
e_savescr2                                  TEXTEQU     <r12d>
e_savescr3                                  TEXTEQU     <r13d>
e_savescr4                                  TEXTEQU     <r14d>
e_savescr5                                  TEXTEQU     <r15d>
ENDIF

IFDEF PARUTIL_LINUX
; 64-bit parameter and return value registers
r_param1                                    TEXTEQU     <rdi>
r_param2                                    TEXTEQU     <rsi>
r_param3                                    TEXTEQU     <rdx>
r_param4                                    TEXTEQU     <rcx>
r_retval                                    TEXTEQU     <rax>

; 64-bit caller-saved scratch registers
r_scratch1                                  TEXTEQU     <r8>
r_scratch2                                  TEXTEQU     <r9>

; 64-bit callee-saved registers
r_savescr1                                  TEXTEQU     <rbx>
r_savescr2                                  TEXTEQU     <r12>
r_savescr3                                  TEXTEQU     <r13>
r_savescr4                                  TEXTEQU     <r14>
r_savescr5                                  TEXTEQU     <r15>

; 32-bit parameter and return value registers
e_param1                                    TEXTEQU     <edi>
e_param2                                    TEXTEQU     <esi>
e_param3                                    TEXTEQU     <edx>
e_param4                                    TEXTEQU     <ecx>
e_retval                                    TEXTEQU     <eax>

; 32-bit caller-saved scratch registers
e_scratch1                                  TEXTEQU     <r8d>
e_scratch2                                  TEXTEQU     <r9d>

; 32-bit callee-saved registers
e_savescr1                                  TEXTEQU     <ebx>
e_savescr2                                  TEXTEQU     <r12d>
e_savescr3                                  TEXTEQU     <r13d>
e_savescr4                                  TEXTEQU     <r14d>
e_savescr5                                  TEXTEQU     <r15d>
ENDIF


ENDIF ;__PARUTIL_REGISTERS_INC
