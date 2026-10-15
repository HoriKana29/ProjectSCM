        lw      0       3       rem     # load remainder
        lw      0       4       divi    # load divisor

        lw      0       5       one     # R7 = 1 , Temp is R7
        lw      0       2       sp      # stack start at 100                                                

start   nand    4       4       6       # R6 = ~divisor
        lw      0       7       one     # R7 = 1 , Temp is R7     
        add     6       7       6       # R6 = ~divisor + 1 = -divisor

        add     3       6       7       # R7 = remainder - divisor
        lw      0       1       sm      # R1 = SIGN_MASK (in build phase)
        nand    7       7       6       # R6 = ~R7
        nand    7       1       7       # R7 = ~SIGN_MASK
        nand    7       7       7       # R7 = R7(original) & SIGN_MASK
        beq     7       0       nonneg  # if R7 >= 0
        beq     0       0       reve

nonneg  sw      2       4       0       # store divisor in stack
        lw      0       7       one
        add     2       7       2       # sp address + 1
        sw      2       5       0       # store Weight in stack
        add     2       7       2       # sp address + 1
        
        add     4       4       4       # divisor * 2
        add     5       5       5       # weight * 2

        beq     0       0       start   # loop until remainder - divisor < 0


reve    add     0       0       1       # set R1 to be quotient

next    lw      0       7       sp
        beq     2       7       done    # if stack empty done
        lw      0       7       negOne
        add     2       7       2       # sp address - 1
        lw      2       5       0       # restore weight
        add     2       7       2       # sp address - 1
        lw      2       4       0       # restore divisor

        nand    4       4       6       # R6 = ~divisor
        lw      0       7       one     # R7 = 1 , Temp is R7     
        add     6       7       6       # R6 = ~divisor + 1 = -divisor

        add     3       6       7       # R7 = remainder - divisor
        lw      0       6       sm      # R6 = SIGN_MASK
        nand    7       6       7       # R7 = ~R7
        nand    6       6       6       # R6 = ~SIGN_MASK
        nand    7       7       7       # R7 = R7 & SIGN_MASK
        beq     7       0       colec   # R7 >= 0
        beq     0       0       next

colec   nand    4       4       6       # R6 = ~divisor (need to calculate again)
        lw      0       7       one
        add     6       7       6       # R6 = -divisor

        add     3       6       7       # R7 = remainder - divisor

        add     7       0       3       # R3 = new remainder
        add     1       5       1       # R1 = quotient + weight -> final product if stack empty
        beq     0       0       next

done    halt
                            
rem     .fill       3
divi    .fill       4
one     .fill       1
negOne  .fill       -1
sp      .fill       100
sm      .fill   -2147483648