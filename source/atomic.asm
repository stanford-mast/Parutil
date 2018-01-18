;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Parutil
;   Multi-platform library of parallelized utility functions.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Authored by Samuel Grossman
; Department of Electrical Engineering, Stanford University
; Copyright (c) 2016-2017
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; atomic.asm
;   Implementation of functions for performing atomic operations.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

INCLUDE registers.inc


_TEXT                                           SEGMENT


; --------- FUNCTIONS ---------------------------------------------------------
; See "parutil.h" for documentation.

parutilAtomicAdd8                               PROC PUBLIC
    lock add                BYTE PTR [r64_param1],  r8_param2
    ret
parutilAtomicAdd8                               ENDP

; ---------

parutilAtomicAdd16                              PROC PUBLIC
    lock add                WORD PTR [r64_param1],  r16_param2
    ret
parutilAtomicAdd16                              ENDP

; ---------

parutilAtomicAdd32                              PROC PUBLIC
    lock add                DWORD PTR [r64_param1], r32_param2
    ret
parutilAtomicAdd32                              ENDP

; ---------

parutilAtomicAdd64                              PROC PUBLIC
    lock add                QWORD PTR [r64_param1], r64_param2
    ret
parutilAtomicAdd64                              ENDP

; ---------

parutilAtomicExchange8                          PROC PUBLIC
    mov                     r8_retval,              r8_param2
    xchg                    BYTE PTR [r64_param1],  r8_retval
    ret
parutilAtomicExchange8                          ENDP

; ---------

parutilAtomicExchange16                         PROC PUBLIC
    mov                     r16_retval,             r16_param2
    xchg                    WORD PTR [r64_param1],  r16_retval
    ret
parutilAtomicExchange16                         ENDP

; ---------

parutilAtomicExchange32                         PROC PUBLIC
    mov                     r32_retval,             r32_param2
    xchg                    DWORD PTR [r64_param1], r32_retval
    ret
parutilAtomicExchange32                         ENDP

; ---------

parutilAtomicExchange64                         PROC PUBLIC
    mov                     r64_retval,             r64_param2
    xchg                    QWORD PTR [r64_param1], r64_retval
    ret
parutilAtomicExchange64                         ENDP

; ---------

parutilAtomicExchangeAdd8                       PROC PUBLIC
    mov                     r8_retval,              r8_param2
    lock xadd               BYTE PTR [r64_param1],  r8_retval
    ret
parutilAtomicExchangeAdd8                       ENDP

; ---------

parutilAtomicExchangeAdd16                      PROC PUBLIC
    mov                     r16_retval,             r16_param2
    lock xadd               WORD PTR [r64_param1],  r16_retval
    ret
parutilAtomicExchangeAdd16                      ENDP

; ---------

parutilAtomicExchangeAdd32                      PROC PUBLIC
    mov                     r32_retval,             r32_param2
    lock xadd               DWORD PTR [r64_param1], r32_retval
    ret
parutilAtomicExchangeAdd32                      ENDP

; ---------

parutilAtomicExchangeAdd64                      PROC PUBLIC
    mov                     r64_retval,             r64_param2
    lock xadd               QWORD PTR [r64_param1], r64_retval
    ret
parutilAtomicExchangeAdd64                      ENDP


_TEXT                                           ENDS


END
