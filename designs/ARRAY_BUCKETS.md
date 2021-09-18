# Selected (so far)

- Each arena will have a group of power of 2 buckets
- It will always try to allocate at the end until it runs out of space, then it
  will go from the beginning until it finds a free slot
- Each bucket will have a bitmap for tracking free slots
- >512b allocations will be handled as a special case
 
## Nice to have
 
- Each thread will have its own arena and shared memory will be handled
  on special arenas.
- 
 
## How each arena will be split

- 4 mb Arena

4mb = 1000 * kb pages
4mb = 8b * 20000 + 16b * 20000 + 32b*20000  + 64b * 9000 + 256b * 3000 + 512b * 3000

(The distrubution is somewhat arbitrary atm until I find some evidence on what to use)
