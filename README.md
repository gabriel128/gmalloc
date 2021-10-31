# GMalloc (Gabe Malloc)

GMalloc is a special allocator that aims to be memory efficient for 
some memory sizes. In those cases it reduces fragmentation by using special buckets

## Usage

Import the library dynamically or statically as you please. And use `gmalloc` and `gfree` as you 
would use `calloc` and `free`

## Special Buckets creation [WIP]

Given a struct `X` we can create a spcial bucket for it with the following macro
```
typedef struct X {
 ...
} X;

GMALLOC_BUCKET(X)
```
if the memory is multiple of 8 bytes, it will use one of the predefined buckets

## Features

- Reduced metadata on Allocations
- Zero fragmentation on exact bucket sizes
- Aligns memory on 8bytes
- Designed with cache efficiency in mind
- Lock free by using thread local arenas
- Avoids false sharing by using thread local arenas
- Blazingly fastish 
- [WIP] Allows configuring special buckets
- Handles allocations up to 512 bytes and then it fallbacks to malloc

## How it works through the memory layout

### Thread local metadata

Each thread contains a metadata data structure that looks like the following:

```
GMetadata = [ Arenarray for allocs of 8 bytes | Arenarray for allocs of 16 bytes | ... | Arenarray for allocs of 512 bytes]
```

[WIP] Special allocations will live in a different metadata with ordered allocations

```
GSpecialMetadata = [ Arenarray for allocs 24 bytes | ... | Arenarray for bucket 240 bytes]
```


### Arenarray (cool name aye!)

As the name suggest an Arenarray consists of an array of arenas for certain bucket. Each arena sizes are defined here: https://github.com/gabriel128/gmalloc/blob/main/src/arenarray.c#L7
The idea is that the higher the index, the higher the memory used by the Arena (it's aprox a linear grow of 10x to avoid 
too many system calls). It also uses a bit of extra memory to keep track of which arenas are full. Mostly for cache efficency 
when using high amounts of memory. 
The original design used Arenas as a linked list (it was also elegant with recursion) but finding a free slots with high memory 
usage was slow, mostly because it was using the cache poorly (checked with `perf record -e LLC-load-misses` and the L1 variant). 

It looks like the following

```
Arenarray = [ ArenaItem {full :: bool, arena :: Arena},  ..., ArenaItem {full :: bool, arena :: Arena}]
```

### Arena

![Untitled - Frame 1](https://user-images.githubusercontent.com/2847315/139574829-4daa2595-e7cf-4c57-8c49-e3966bf3fef0.jpg)


This is the main datastructure were the allocations are stored. It looks like the following:

```
Arena = { ArenaHeader, [MemBlock] }
```

where 

```
ArenaHeader = { FreeStack, metadata... }
```
and 

```
MemBlock = { ArenaHeader*, allocated_data }
```

### FreeStack

The FreeStack is a stack data structure that stores memory freed by `gfree` and there is one per Arena. 
Since the bucket is a fixed number on an arena, the size is also fixed and no free slots coalescing is needed as
in other allocators.

### MemBlock

A MemBlock is stored in each allocation and a pointer to allocated_data is what `gmalloc` returns. 

## Benchmarks

These are not by any means exhaustive and compares gmalloc on the special cases
where it was designed for. Malloc is a general allocator, gmalloc is a special one

### Single thread 3GB of sequential allocations

This gives an aprox. measurment on how it performs with lots of allocations

**gmalloc**
```
  Max Mem used RSS: 2812.351562 MB

  Time (mean ± σ):      3.583 s ±  0.012 s    [User: 2.752 s, System: 0.825 s]
  
  Range (min … max):    3.568 s …  3.600 s    10 runs

     6,642,470,688      branches:u                #    1.848 G/sec                  
             8,636      branch-misses:u           #    0.00% of all branches        
                 0      context-switches:u        #    0.000 /sec                   
           781,322      page-faults:u             #  217.423 K/sec                  
          3,593.56 msec task-clock:u              #    1.000 CPUs utilized          
     9,118,889,135      cycles:u                  #    2.538 GHz                    
    27,055,127,985      instructions:u            #    2.97  insn per cycle         
        15,619,451      cache-references:u        #    4.347 M/sec                  
         4,501,970      cache-misses:u            #   28.823 % of all cache refs    

       3.594594854 seconds time elapsed

       2.780703000 seconds user
       0.808085000 seconds sys
```
**malloc (for comparison)**
```
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
```

### Multi threaded allocations

**gmalloc**
```
  Max Mem usage: 1598.914062 MB

  Time (mean ± σ):      1.041 s ±  0.677 s    [User: 5.282 s, System: 1.165 s]
  Range (min … max):    0.626 s …  2.298 s    10 runs

     3,737,110,560      branches:u                #  973.900 M/sec                  
            20,756      branch-misses:u           #    0.00% of all branches        
                 0      context-switches:u        #    0.000 /sec                   
           419,968      page-faults:u             #  109.445 K/sec                  
          3,837.26 msec task-clock:u              #    6.344 CPUs utilized          
     9,963,053,759      cycles:u                  #    2.596 GHz                    
    15,942,304,423      instructions:u            #    1.60  insn per cycle         
        15,879,418      cache-references:u        #    4.138 M/sec                  
         3,842,544      cache-misses:u            #   24.198 % of all cache refs
```

**malloc (for comparison)**
```
  Time (mean ± σ):      3.135 s ±  0.058 s    [User: 6.610 s, System: 5.676 s]
  Range (min … max):    3.077 s …  3.246 s    10 runs

     5,018,800,029      branches:u                #  207.924 M/sec                  
         3,216,338      branch-misses:u           #    0.06% of all branches        
                 0      context-switches:u        #    0.000 /sec                   
           801,184      page-faults:u             #   33.192 K/sec                  
         24,137.63 msec task-clock:u              #    2.499 CPUs utilized          
    16,671,359,044      cycles:u                  #    0.691 GHz                    
    24,795,451,867      instructions:u            #    1.49  insn per cycle         
        54,106,631      cache-references:u        #    2.242 M/sec                  
         8,759,277      cache-misses:u            #   16.189 % of all cache refs
```
### Single thread reusing memory on a 300k of iterations 500 allocations
    This will use the free stack data structure heavily for reusing memory
    
**gmalloc**
```
    RSS Max Mem usage: Mem usage: 0.000000 MB

    Time (mean ± σ):      9.854 s ±  0.077 s    [User: 9.845 s, System: 0.002 s]
    Range (min … max):    9.776 s … 10.040 s    10 runs

    24,507,049,876      branches:u                #    2.497 G/sec                  
         2,840,412      branch-misses:u           #    0.01% of all branches        
                 0      context-switches:u        #    0.000 /sec                   
                67      page-faults:u             #    6.825 /sec                   
          9,816.41 msec task-clock:u              #    1.000 CPUs utilized          
    34,060,823,202      cycles:u                  #    3.470 GHz                    
   110,518,240,256      instructions:u            #    3.24  insn per cycle         
            25,597      cache-references:u        #    2.608 K/sec                  
             4,532      cache-misses:u            #   17.705 % of all cache refs    
```


**malloc (for comparison)**
```
    Mem usage: 0.000000 MB 

    Time (mean ± σ):      9.131 s ±  0.230 s    [User: 9.125 s, System: 0.001 s]
    Range (min … max):    8.964 s …  9.759 s    10 runs

    23,234,062,740      branches:u                #    2.592 G/sec                  
         3,219,039      branch-misses:u           #    0.01% of all branches        
                 0      context-switches:u        #    0.000 /sec                   
                68      page-faults:u             #    7.585 /sec                   
          8,965.38 msec task-clock:u              #    1.000 CPUs utilized          
    31,074,310,316      cycles:u                  #    3.466 GHz                    
   104,976,303,894      instructions:u            #    3.38  insn per cycle         
            30,881      cache-references:u        #    3.444 K/sec                  
             5,340      cache-misses:u            #   17.292 % of all cache refs    

```
### License

MIT License

Copyright (c) 2021 

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
