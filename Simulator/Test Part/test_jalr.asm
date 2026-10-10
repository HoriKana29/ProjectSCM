        lw      0       1       target      
        jalr    1       2                  
        lw      0       3       mark      
land    lw      0       4       bad       
        jalr    4       4                
        lw      0       5       mark    
        halt
wrong   lw      0       6       mark      
        halt
target  .fill   land
bad     .fill   wrong
mark    .fill   77
