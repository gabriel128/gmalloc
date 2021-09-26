# Arena buckets

- greater than 512b allocations will be handled as a special case
  probably just calling malloc as a fallback

### How each arena will be split

- Bucket sizes are divisible by 8 to ensure alignment
- Each Arena contains 1 bucket type
- 10 pages per arena 
  with 6 bucket kinds it means that you'll get 
  5120 8bytes buckets
  2560 16bytes buckets
  1280 32bytes buckets
  ....

- Arenas are created on-demand (i.e. on the first gmalloc call for that bucket)
- Each bucket gets a collection of Arenas
- When an Arena size reaches zero it's unmmaped and the pages are returned to the OS


###  Memory layout

Each bucket arena will be split evenly by bucket sizes

## FreeBucket Stack
- Per Arena DS 
- On each allocation it will try to pop a free slot from the stack. 
  If empty it will return the last used arena slot+1
- On each `gfree` it will push the address of the 
  free value to the stack

## Nice to have
 
- Each thread will have its own set of arenas to reduce cache 
  false sharing and lock contention
 
