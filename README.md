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

### Single thread reusing memory on a 300k of iterations 500 allocations
    This will use the free stack data structure heavily for reusing memory
    
**gmalloc**

    RSS Max Mem usage: Mem usage: 0.000000 MB

    Time (mean ± σ):      3.283 s ±  0.040 s    [User: 3.279 s, System: 0.002 s]
    Range (min … max):    3.242 s …  3.390 s    10 runs

     7,652,446,176      branches:u                #    2.343 G/sec                  
           913,403      branch-misses:u           #    0.01% of all branches        
                 0      context-switches:u        #    0.000 /sec                   
                65      page-faults:u             #   19.904 /sec                   
          3,265.66 msec task-clock:u              #    1.000 CPUs utilized          
    11,315,543,351      cycles:u                  #    3.465 GHz                    
    35,106,235,884      instructions:u            #    3.10  insn per cycle         
            10,878      cache-references:u        #    3.331 K/sec                  
             4,576      cache-misses:u            #   42.067 % of all cache refs    


**malloc (for comparison)**
    Mem usage: 0.000000 MB 

    Time (mean ± σ):      2.740 s ±  0.037 s    [User: 2.737 s, System: 0.002 s]
    Range (min … max):    2.700 s …  2.824 s    10 runs

     6,970,260,652      branches:u                #    2.529 G/sec                  
           936,031      branch-misses:u           #    0.01% of all branches        
                 0      context-switches:u        #    0.000 /sec                   
                65      page-faults:u             #   23.581 /sec                   
          2,756.50 msec task-clock:u              #    1.000 CPUs utilized          
     9,483,908,726      cycles:u                  #    3.441 GHz                    
    31,493,100,934      instructions:u            #    3.32  insn per cycle         
            28,261      cache-references:u        #   10.252 K/sec                  
            10,944      cache-misses:u            #   38.725 % of all cache refs    

