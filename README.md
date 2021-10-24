## GMalloc (Gabe Malloc)

WIP

If you are curious about what's going on with this atm check the designs folder

- Cache efficient 
- Lock free by using thread local arenas

## Benchmarks

### Single thread 3GB of sequential allocations

This gives an aprox. measurment on how it performs with lots of allocations

**gmalloc**
  Max Mem used RSS: 2812.339844 MB 

  Time (mean ± σ):      4.293 s ±  0.029 s    [User: 3.481 s, System: 0.806 s]
  Range (min … max):    4.271 s …  4.368 s    10 runs


     9,042,476,450      branches:u                #    2.091 G/sec                  
            71,357      branch-misses:u           #    0.00% of all branches        
                 0      context-switches:u        #    0.000 /sec                   
           781,331      page-faults:u             #  180.662 K/sec                  
          4,324.82 msec task-clock:u              #    1.000 CPUs utilized          
    11,694,806,927      cycles:u                  #    2.704 GHz                    
    33,055,159,085      instructions:u            #    2.83  insn per cycle         
        16,932,150      cache-references:u        #    3.915 M/sec                  
         5,221,871      cache-misses:u            #   30.840 % of all cache refs

**malloc (for comparison)**
  Max Mem used RSS: 3815.847656 MB 

  Time (mean ± σ):      4.499 s ±  0.032 s    [User: 3.546 s, System: 0.944 s]
  Range (min … max):    4.464 s …  4.546 s    10 runs

     7,801,882,502      branches:u                #    1.738 G/sec                  
           121,434      branch-misses:u           #    0.00% of all branches        
                 0      context-switches:u        #    0.000 /sec                   
           976,629      page-faults:u             #  217.508 K/sec                  
          4,490.09 msec task-clock:u              #    1.000 CPUs utilized          
    11,509,580,130      cycles:u                  #    2.563 GHz                    
    35,405,005,497      instructions:u            #    3.08  insn per cycle         
        31,493,856      cache-references:u        #    7.014 M/sec                  
        12,795,588      cache-misses:u            #   40.629 % of all cache refs    


### Multi threaded 3GB of allocations

**gmalloc**

  Max Mem usage: 1640.605469 MB

  Time (mean ± σ):      1.871 s ±  0.007 s    [User: 6.092 s, System: 3.218 s]
  Range (min … max):    1.860 s …  1.885 s    10 runs

     4,376,001,710      branches:u                #  469.779 M/sec                  
         1,547,218      branch-misses:u           #    0.04% of all branches        
                 0      context-switches:u        #    0.000 /sec                   
           440,951      page-faults:u             #   47.338 K/sec                  
          9,315.03 msec task-clock:u              #    4.091 CPUs utilized          
    15,396,096,578      cycles:u                  #    1.653 GHz                    
    17,320,466,820      instructions:u            #    1.12  insn per cycle         
       146,675,063      cache-references:u        #   15.746 M/sec                  
        39,958,271      cache-misses:u            #   27.243 % of all cache refs    

**malloc (for comparison)**

  Time (mean ± σ):      4.637 s ±  0.021 s    [User: 8.743 s, System: 8.086 s]
  Range (min … max):    4.594 s …  4.666 s    10 runs

     5,031,288,703      branches:u                #  312.707 M/sec                  
         3,571,604      branch-misses:u           #    0.07% of all branches        
                 0      context-switches:u        #    0.000 /sec                   
           822,271      page-faults:u             #   51.106 K/sec                  
         16,089.45 msec task-clock:u              #    2.743 CPUs utilized          
    18,738,719,147      cycles:u                  #    1.165 GHz                    
    24,850,299,476      instructions:u            #    1.33  insn per cycle         
       149,810,589      cache-references:u        #    9.311 M/sec                  
         4,828,378      cache-misses:u            #    3.223 % of all cache refs    (this is nuts! still, it's slower)

### Single thread 3GB of mix of allocations and freeing, trying to emulate something similar to a real application
    This will use the free stack data structure for reusing memory
    
**gmalloc**

    RSS Max Mem usage: 759.210938 MB

    Time (mean ± σ):      3.369 s ±  0.055 s    [User: 3.157 s, System: 0.210 s]
    Range (min … max):    3.337 s …  3.485 s    10 runs

     8,249,264,225      branches:u                #    2.475 G/sec                  
           497,289      branch-misses:u           #    0.01% of all branches        
                 0      context-switches:u        #    0.000 /sec                   
           195,384      page-faults:u             #   58.628 K/sec                  
          3,332.61 msec task-clock:u              #    1.000 CPUs utilized          
    10,721,740,619      cycles:u                  #    3.217 GHz                    
    30,449,884,450      instructions:u            #    2.84  insn per cycle         
           914,920      cache-references:u        #  274.535 K/sec                  
            25,202      cache-misses:u            #    2.755 % of all cache refs

**malloc (for comparison)**
    RSS Max Mem usage: 759.179688 MB

    Time (mean ± σ):      1.706 s ±  0.004 s    [User: 1.488 s, System: 0.216 s]
    Range (min … max):    1.700 s …  1.713 s    10 runs

     4,000,257,324      branches:u                #    2.337 G/sec                  
           109,255      branch-misses:u           #    0.00% of all branches        
                 0      context-switches:u        #    0.000 /sec                   
           195,388      page-faults:u             #  114.171 K/sec                  
          1,711.37 msec task-clock:u              #    1.000 CPUs utilized          
     5,046,017,714      cycles:u                  #    2.949 GHz                    
    17,500,512,166      instructions:u            #    3.47  insn per cycle         
         1,129,714      cache-references:u        #  660.124 K/sec                  
             8,130      cache-misses:u            #    0.720 % of all cache refs
