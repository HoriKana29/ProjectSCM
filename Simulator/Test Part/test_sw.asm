        lw      0       1       numA    
        sw      0       1       slot1     
        lw      0       2       slot1     
        lw      0       3       addr       
        sw      3       3       1         
        lw      3       4       1       
        halt
numA    .fill   99
slot1   .fill   0
slot2   .fill   0
addr    .fill   slot1
