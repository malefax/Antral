------------------------------------------------------------------------------                  
Introduction                                                                                     
------------------------------------------------------------------------------               
Antral is a High-Precision Timing Library which uses TSC register to cali-
brate execution timing of your actual code. While it’s obviously not perfectly 
precise—since there's an interrupt every 100ms while calculating frequency—it 
still provides a much more accurate estimate!

------------------------------------------------------------------------------
Pipeline Execution
------------------------------------------------------------------------------
Time:  1    2    3    4    5
Inst1: [Fetch][Decode][Execute][Write]
Inst2:       [FetcDecode][Execute][Write]h][Decode][Execute][Write]
Inst3:              [Fetch][Decode][Execute][Write]

Like in early computing days like each instruction went through a cycle of

                  [Fetch] -> [Decode] -> [Execute] -> [Write]
                  
but in modern processor,Multiple instructions execute simultaneously in diffe- 
rent stages! CPU predicts what instructions will run next and starts executing 
them early and Without serialization, you might run into a new problem which 
is called just a fancy term Out-Of-Order Execution well Modern CPUs don't exec-
cute instructions in the order you write them. They use several optimization 
techniques!

-----------------------------------------------------------------------------
Out-Of-Order Exection
-----------------------------------------------------------------------------
What’s going on inside the CPU ?  

Your code:        What CPU might actually do:
instruction A  →  instruction C (starts first)
instruction B  →  instruction A (starts second) 
instruction C  →  instruction B (starts last)

Now lets
Instruction A -> read_tsc();
Instruction B -> expensive_function(); 

What you wrote:
============================================================================
start = read_tsc();       You think this happens first
expensive_function();     You think this happens second  
end = read_tsc();         You think this happens third
============================================================================
What CPu might actually do:

read_tsc();               Starts
expensive_function();     Starts before first read_tsc() finishes
read_tsc();               Starts before expensive_function() finishes
============================================================================
Ahh! we got a problem now in order to caluclate the execution time of our
expensive_function(); we have to wait for all instruction execution in 
order to do this we have to perform Serialization!

----------------------------------------------------------------------------
Serialization
----------------------------------------------------------------------------
A serialized instruction is an instruction that forces the CPU to complete 
all previous instructions before executing the serialized instruction, and 
prevents any subsequent instructions from starting until the
serialized instruction completes!
============================================================================
Without Serialization!

CPU Timeline:
Time: 1    2    3    4    5    6    7    8
      |----expensive_function()------------|
      |rdtsc|                        |rdtsc|
      ↑                              ↑
   start=100                      end=102
   
Measured: 2 cycles (WRONG! Should be ~7 cycles)
============================================================================
WIth Serialization!

CPU Timeline:
Time: 1    2    3    4    5    6    7    8
      |cpuid|rdtsc|---expensive_function()---|rdtscp|cpuid|
             ↑                               ↑
          start=102                       end=107
          
Measured: 5 cycles (CORRECT!)

============================================================================
EXAMPLE
----------------------------------------------------------------------------
$ make
gcc -Wall -Wextra -O2 -c ./src/tsc.c -o tsc.o
gcc -Wall -Wextra -O2 main.c tsc.o -o main
----------------------------------------------------------------------------
$ time ./main
TSC  supported on this system
Cycles: 14790
Found CPU frequency: 800.00 MHz
CFTIME (ns): 18487
TFTIME (ns): 6162
TSC freq: 2399.98 MHz
Difference: 1599.98 MHz

real	0m0.103s
user	0m0.000s
sys	0m0.002s
---------------------------------------------------------------------------







   
  
