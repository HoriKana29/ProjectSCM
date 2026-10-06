        lw      0       1       one         
        lw      0       2       one         
        lw      0       6       two         
        beq     0       1       skip1       
        add     3       1       3          
skip1   beq     1       2       loop        
        add     4       1       4           
loop    add     5       1       5       
        beq     5       6       done        
        beq     0       0       loop        
done    halt
one     .fill   1
two     .fill   2
