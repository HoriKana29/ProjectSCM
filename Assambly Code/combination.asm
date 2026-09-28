        lw      0       1       n           # load n in Combination(n,r) (3)
        lw      0       4       r           # load r in Combination(n,r) (1)
        lw      0       6       func        # load function address (50)
        lw      0       2       sp          # load stack pointer (100) , R5 is temporary register
                                            
        jalr    6       7                   # jump to function address and link return address in R7

        halt

comb    sw      2       1       0           # store n in stack
        lw      0       5       one
        add     2       5       2           # sp address + 1
        sw      2       4       0           # store r in stack
        add     2       5       2           # sp address + 1
        sw      2       7       0           # store return address in stack
        add     2       5       2           # sp address + 1 (point next)

        beq     0       4       base        # if(r==0)
        beq     1       4       base        # if(n==r)

        lw      0       5       neg1
        add     1       5       1           # n - 1
        add     4       5       4           # r - 1

        lw      0       6       func
        jalr    6       7                   # call combination(n-1,r-1)

        lw      0       5       one
        sw      2       3       0           # store result in stack
        add     2       5       2           # sp address + 1 (point next)

        lw      2       4       -3

        lw      0       6       func
        jalr    6       7                   # call combination(n-1,r)

        lw      0       5       neg1
        add     2       5       2           # sp address - 1 (to Result1)
        lw      2       5       0           # R5 = result of combination(n-1,r)
        add     3       5       3           # sum of results

        lw      0       5       neg1
        add     2       5       2           # sp address - 1 (to return address)

        lw      2       7       0           # R7 = return address at sp

        add     2       5       2           # sp address - 1 (to r)
        lw      2       4       0           # restore r

        add     2       5       2           # sp address - 1 (to n)
        lw      2       1       0           # restore n

        jalr    7       6                   # return from recurcive case

        
base    lw      0       5       one
        add     0       5       3           # R3 = 1

        lw      0       5       neg1
        add     2       5       2           # sp address - 1 (to return address)

        lw      2       7       0           # R7 = return address at sp
        
        add     2       5       2           # sp address - 1 (to r)
        lw      2       4       0           # restore r

        add     2       5       2           # sp address - 1 (to n)
        lw      2       1       0           # restore n


        jalr    7       6                   # return from base case
        

done    halt
                           
n           .fill   3
r           .fill   1
func        .fill   comb
sp          .fill   100
one         .fill   1
neg1        .fill   -1