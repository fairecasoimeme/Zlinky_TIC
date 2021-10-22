#==========================================================================
# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2015, Freescale Semiconductor, Inc.
# Copyright 2016-2017, 2020 NXP
# All rights reserved.
#
#
# MODULE:      Power Manager
#
# COMPONENT:   pwrm_setjmp.s
#
# DESCRIPTION: Setjmp / longjmp for power manager warm start with memory held
#              for JN518x family
#
#==========================================================================

.cpu cortex-m4
.syntax unified

.section ".text.setjmp","x",%progbits
.align 2
.thumb
.thumb_func
.globl PWR_setjmp
.type PWR_setjmp, %function
PWR_setjmp:
    mov ip, sp
    stmea a1!, {v1-v7, fp, ip, lr}
    mov a1, #0
    bx lr
.size PWR_setjmp,.-PWR_setjmp

.section ".text.longjmp","x",%progbits
.align 2
.thumb
.thumb_func
.globl PWR_longjmp
.type PWR_longjmp, %function
PWR_longjmp:
    ldmfd a1!, {v1-v7, fp, ip, lr}
    mov sp, ip
    movs a1, a2
    it eq
    moveq a1, #1
    bx lr

.size PWR_longjmp,.-PWR_longjmp
