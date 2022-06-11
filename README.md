# Overview

This is source code for a C program that reads memory calls from a .trc file before simulating the process of checking the cache, keeping track of the number of misses/hits as well the total cost and waste.

# Input

  **required**
  
      -f $[trace file name] (file path, file must follow the format of the sample .trc files on this repo)
      -s $[cache size in KB] (any number, if not a power of 2, program will round up to the next power of 2)
      -b $[block size in KB] (any number, if not a power of 2, program will round up to the next power of 2)
      -a $[associativity] (any number, if it's too high for the selected cache size it will auto reduce)
      -r $[replacement policy] (RR (round robin) or RND (random) or LRU (least recently used))
