# Overview

This is source code for a C program that reads memory accesses from a trace file before simulating the process of running those accesses against a cache, keeping track of the number of misses/hits as well the total cost and utilization.

# Usage

  **required arguments**
  
      -f $[trace file name] (file path, file must follow the format of the sample .trc files on this repo)
      -s $[cache size in KB] (integer, if not a power of 2, program will round up to the next power of 2)
      -b $[block size in bytes] (integer, if not a power of 2, program will round up to the next power of 2)
      -a $[associativity] (integer, if it's too high for the selected cache size it will auto reduce)
      -r $[replacement policy] (string, 'RR' (round robin) or 'RND' (random) or 'LRU' (least recently used), has no effect if associativity is 1)

  **sample execution input**
  
    ./sim -f 1.trc -s 128 -b 32 -a 4 -r RR
      
  **sample execution output**
  
    Trace File: 1.trc

    ***** Cache Input Parameters *****
    Cache Size: 128 KB
    Block Size: 32 bytes
    Associativity: 4
    Replacement Policy: Round Robin

    ***** Cache Calculated Values *****

    Total # Blocks: 4096
    Tag Size: 17 bits
    Index Size: 10 bits
    Total # Rows: 1024
    Overhead Size: 9216 bytes
    Implementation Memory Size: 137.0 KB
    Cost: $12.33

    ***** Cache Simulation Results *****

    Total Cache Accesses: 382626
    Cache Hits: 374182
    Cache Misses: 8444
    --- Compulsory Misses: 3997
    --- Conflict Misses: 4447

    ***** ***** Cache Hit & Miss Rate: ***** *****

    Hit Rate: 97.7931%
    Miss Rate: 2.2069%
    CPI: 4.79 Cycles/Instruction
    Unused Cache Space: 3.09 KB / 128.00 KB = 2.42%   Waste: $0.28
    Unused Cache Blocks: 99 / 4096
    
  Use the provided makefile to compile.
