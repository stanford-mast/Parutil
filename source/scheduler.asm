;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Parutil
;   Multi-platform library of parallelized utility functions.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Authored by Samuel Grossman
; Department of Electrical Engineering, Stanford University
; Copyright (c) 2016-2017
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; scheduler.asm
;   Implementation of thread scheduling assistance functionality.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

INCLUDE registers.inc
INCLUDE scheduler.inc
INCLUDE spindle.inc


_TEXT                                       SEGMENT


; --------- CONSTANTS ---------------------------------------------------------
; See "parutil.h" for documentation.

SParutilStaticSchedule_startUnit            EQU         0
SParutilStaticSchedule_endUnit              EQU         8
SParutilStaticSchedule_increment            EQU         16


; --------- MACROS ------------------------------------------------------------

; Writes static schedule information already-loaded into scheduling registers into the SParutilStaticSchedule structure at the specified address.
; Parameters:
;    - r_schedule holds the address of the SParutilStaticSchedule structure to loaded
parutilSchedulerStaticWriteSchedule         MACRO r_schedule
    mov                     QWORD PTR [r_schedule + SParutilStaticSchedule_startUnit],              rsi
    mov                     QWORD PTR [r_schedule + SParutilStaticSchedule_endUnit],                rdi
    mov                     QWORD PTR [r_schedule + SParutilStaticSchedule_increment],              rbx
ENDM


; --------- INTERNAL FUNCTIONS ------------------------------------------------
; See "scheduler.c" for documentation.

parutilSchedulerStaticChunkedInternal       PROC PUBLIC
    ; Save non-volatile registers.
    push                    rbx
    push                    rsi
    push                    rdi
    push                    r12
    push                    r13
    
    ; Set aside the original parameters.
    mov                     r12,                    r64_param2
    mov                     r13,                    r64_param1
    
    ; Get scheduling information and write it to the output data structure.
    parutilSchedulerInitStaticChunk
    parutilSchedulerStaticWriteSchedule             r12
    
    ; Restore non-volatile registers and return.
    pop                     r13
    pop                     r12
    pop                     rdi
    pop                     rsi
    pop                     rbx

    ret
parutilSchedulerStaticChunkedInternal       ENDP


_TEXT                                       ENDS


END
