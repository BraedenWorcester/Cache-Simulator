# Overview

This is source code for a C program that reads memory calls from a .trc file before simulating the process of checking the cache, keeping track of the number of misses/hits as well the total cost and waste.

# Input

  **required**
  
      -f $[trace file name] (file must follow the format of the sample .trc files on this repo)
      -s $[cache size in KB] (if not a power of 2, program will round up to the next power of 2)
      -b $[block size in KB] (if not a power of 2, program will round up to the next power of 2)
      -a $[associativity] (any number)
      -r $[replacement policy] (RR (round robin) or RND (random) or LRU (least recently used))
      
_optional_

      -x
