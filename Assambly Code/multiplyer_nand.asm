        lw      0       2       mcand   # load multiplicand (3)  
        lw      0       3       mplier  # load multiplier (4)
        lw      0       4       mask    # load mask (1)
        lw      0       5       count   # R6 is temp register
                                        
                                        
        add     0       0       1       # initialize result = 0

start   beq     0       5       done    # branch if count is 0

        nand    3       4       6       # and 3 and 4, store in 6
        nand    6       6       6       

        beq     6       0       noAdd   # if the mplier and mask is 0, skip addition
        add     1       2       1       # add multiplicand to result

noAdd   add     2       2       2       # shift multiplicand left
        add     4       4       4       # shift mask left

        lw      0       6       neg1    # load -1 to temp register
        add     5       6       5       # decrement count 

        beq     0       0       start   # repeat

done    halt
                           
mcand   .fill   3
mplier  .fill   4
mask    .fill   1
count   .fill   15
neg1    .fill   -1