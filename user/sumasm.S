.globl main
.section .data
    fmt:        .string "%d\n"
    err_no_inp: .string "Error: no input\n"
    err_no_sp:  .string "Error: incorrect input (expecting 2 numbers divided by space)\n"
    err_no_2nd: .string "Error: no second number\n"

.section .text
main:
    addi sp, sp, -288
    
    sd ra, 280(sp)
    sd s0, 272(sp)
    sd s1, 264(sp)

    mv s0, sp

    li a1, 256
    mv a0, s0
    call gets
    
    lbu t0, 0(s0)
    beqz t0, err_input

    mv s1, s0

find_space:
    lbu t0, 0(s1)
    beqz t0, err_no_space
    li t1, ' '
    beq t0, t1, found_space
    addi s1, s1, 1
    j find_space

found_space:
    sb zero, 0(s1)
    addi s1, s1, 1

    lbu t0, 0(s1)
    beqz t0, err_no_second

    li t1, '0'
    blt t0, t1, err_no_second
    li t1, '9'
    bgt t0, t1, err_no_second

    mv a0, s0
    call atoi
    mv t0, a0

    mv a0, s1
    call atoi
    mv t1, a0

    add a1, t0, t1
    la a0, fmt
    call printf

    li a0, 0
    call exit

err_input:
    la a1, err_no_inp
    li a2, 16
    j do_write_exit

err_no_space:
    la a1, err_no_sp
    li a2, 62
    j do_write_exit

err_no_second:
    la a1, err_no_2nd
    li a2, 25
    j do_write_exit

do_write_exit:
    li a0, 2
    call write
    li a0, 1
    call exit
