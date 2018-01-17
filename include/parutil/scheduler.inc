;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Parutil
;   Multi-platform library of parallelized utility functions.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Authored by Samuel Grossman
; Department of Electrical Engineering, Stanford University
; Copyright (c) 2016-2017
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; scheduler.inc
;   Macros for implementing various types of thread scheduling mechanisms.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

INCLUDE registers.inc
INCLUDE spindle.inc


_TEXT                                       SEGMENT


; --------- MACROS ------------------------------------------------------------

; Implements a static chunked scheduler.
; Given a total number of units of work, divides evenly by the number of threads in the Spindle task.
; Assigns one chunk to each thread, and takes care of load balancing in case of an uneven division.
; Clobbers: rax, rbx, rcx, rdx
; Parameters:
;    - r13 holds the number of units of work
; Returns:
;    - rsi holds the first iteration index
;    - rdi holds the last iteration index
parutilSchedulerInitStaticChunk            MACRO
    ; Extract thread information to determine chunk parameters and assign chunks to each thread round-robin.
    ; Formulas:
    ;    assignment  = #units / #total_threads
    ;    addon       = #units % #total_threads < thread_id ? 1 : 0
    ;    prev_addons = min(#units % #total_threads, thread_id)
    ;
    ;    base (rsi)  = (assignment * thread_id) + prev_addons
    ;    inc         = 1
    ;    max  (rdi)  = base + assignment + addon - 1
    
    ; First, perform the unsigned division by setting rdx:rax = #units and dividing by #total_threads.
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
