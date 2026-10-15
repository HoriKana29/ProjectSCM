        lw      0       1       numA        # addr = reg0 + 7 = 7, mem[7] = 5   -reg1
        lw      0       2       numB        # addr = reg0 + 8 = 8, mem[8] = 7   -reg2
        lw      0       3       numC        # addr = reg0 + 9 = 9, mem[9] = -3  -reg3
        add     1       2       4           # reg1 + reg2 = 5 + 7
        add     1       3       5           # reg1 + reg3 = 5 + (-3)
        add     1       1       1           # reg1 + reg1 = 5 + 5
        halt                                # Stop
numA    .fill   5
numB    .fill   7
numC    .fill   -3
