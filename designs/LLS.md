## Discarded design 

Why?
 - This uses linked lists which is cache inefficient
 - Coalescing is hard
 - Appending free lists is slow and even if we append on the head it will mean that  
   the head won't be on the cache since it will change all the time 

  Allocation memory                  end
  1. Starts

  |    free_space        |
  ^
  |free_head|

  2. Several allocations (Nothing has been freed)

  | head1 | memory1 | padding ||  head2 | memory2 | padding ||       free_space        |
                                                            ^
                                                        | free_head |

  3. Memory1 has been freed

  | free            ||  head2 | memory2 | padding ||         free_space        |
  ^                                                ^
  |                                            | free_head | free_slot|
  |                                                  |
  |                                                  |
   --------------------------------------------------

  4. New memory allocation
  | free         |  h-m-p2 |  h-m-p3 | free_space |
  ^                                  ^
  |                              | free_head | free_slot|
  |                                                |
  |                                                |
   ------------------------------------------------

  5. Allocation and No more space in the free head

  | h-mp-5 | free |  h-m-p2 |  h-m-p3 | free_space |
           ^                          ^
           |                       | free_head | free_slot|
           |                                        |
           |                                        |
            ---------------------------------------

  6.a Mem2 is freed.

  | h-mp-5 | free |  free   |  h-m-p3 | free_space |
           ^      ^                   ^
           |      |                | free_head | free_slot | free_slot |
           |      |                                 |          |
           |      |                                 |          |
            ---------------------------------------            |
                  |                                            |
                   --------------------------------------------

  6.b Free slots coalesce. We can figure the address out by:
      free_slot->free_mem_region + free_slot->size == free_slot->next->free_mem_region-1

  | h-mp-5 |      free      |  h-m-p3 | free_space |
           ^                          ^
           |                      | free_head | free_slot |
           |                                       |
           |                                       |
            ---------------------------------------

