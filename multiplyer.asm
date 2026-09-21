        lw      0       1       mplier  # load multiplier (4)
        lw      0       2       mcand   # load multiplicand (3)  
        lw      0       3       Accu
        lw      0       4       Qminus1
        lw      0       5       Count
        lw      0       7       one

start   beq     0       5       done    # branch if count is 0

        nand    1       7       6
        nand    6       6       6       # and with 1 to check multiplier's LSB with 1

        beq     6       0       Q0_is_0   # if Q0 is 0
        beq     4       0       case10    # if 10, go to case 10
        beq     0       0       shift     # if 11, go to shift

Q0_is_0 beq     4       0       shift    # 00, go to shift
        add     3       2       3        # A = A + M
        beq     0       0       shift

case10  nand    2       2       6        # ~M
        add     6       7       6        # ~M + 1
        add     3       6       3        # A = A - M
        beq     0       0       shift
shift   
        beq     0       0       start   # repeat

done    halt
                           
mcand   .fill   3
mplier  .fill   4
Accu    .fill   0
Qminus1 .fill   0
Count   .fill   15
neg1    .fill   -1
one     .fill   1