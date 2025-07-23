#ifndef TSC_TIMING_H
#define TSC_TIMING_H

#if !defined(__linux__)
    #error "This code can only be compiled on Linux."
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>

typedef uint64_t tsc_t;
typedef uint32_t tsc_half_t;

#if !defined(__x86_64__) && !defined(__i386__) && !defined(_M_X64) && !defined(_M_IX86)
#error "TSC timing functions require x86 or x86_64 architecture"
#endif

#if defined(__GNUC__) || defined(__clang__)
#define TSC_INLINE static inline __attribute__((always_inline))
#elif defined(_MSC_VER)
#define TSC_INLINE static __forceinline
#else
#define TSC_INLINE static inline
#endif

/* Feature detection macros */
#ifdef __RDTSCP__
#define TSC_HAS_RDTSCP 1
#else
#define TSC_HAS_RDTSCP 0
#endif

/* Non-serialized function 
@@@WARNING!

This function might give you inaccuate result due
to Out-Of-Order execution.

*/
TSC_INLINE tsc_t tsc_read(void)
{
    tsc_half_t low, high;
    __asm__ volatile (
        "rdtsc"
        : "=a"(low), "=d"(high)
    );
    return ((tsc_t)high << 32) | low;
}

/* searialization function CPUID+RDTSC ----->start of counter
@@@=== wait for the prev instruction execution ===@@@@
@@@=== then read tsc ===@@@ 
*/


TSC_INLINE tsc_t tsc_read_serialized(void)
{
    tsc_half_t low, high;
    __asm__ volatile (
        "cpuid\n\t"       
        "rdtsc\n\t"        
        : "=a"(low), "=d"(high)
        : "a"(0)           
        : "rbx", "rcx"     
    );
    return ((tsc_t)high << 32) | low;
}

/* searialization function RDTSCP+CPUID ----->end of counter
@@@===  Wait for measured code to finish, then read TSC ===@@@
@@===   prevent any code to be included ===@@@ 
*/

TSC_INLINE tsc_t tsc_read_serialized_end(void)
{
    tsc_half_t low, high;
    __asm__ volatile (
        "rdtscp\n\t"                   
        "mov %%eax, %0\n\t"            
        "mov %%edx, %1\n\t"           
        "cpuid\n\t"                    
        : "=r"(low), "=r"(high)
        :
        : "rax", "rbx", "rcx", "rdx"   
    );
    return ((tsc_t)high << 32) | low;
}

/* Return cycles between the start counter and end coumter */

TSC_INLINE tsc_t tsc_cycles_elapsed(tsc_t start, tsc_t end)
{
    return end - start;
}

TSC_INLINE uint64_t tsc_to_nanoseconds(tsc_t cycles, uint64_t tsc_hz)
{
    return (cycles * 1000000000ULL) / tsc_hz;
}

TSC_INLINE uint64_t tsc_to_microseconds(tsc_t cycles, uint64_t tsc_hz)
{
    return (cycles * 1000000ULL) / tsc_hz;
}

TSC_INLINE uint64_t tsc_to_milliseconds(tsc_t cycles, uint64_t tsc_hz)
{
    return (cycles * 1000ULL) / tsc_hz;
}

#if TSC_HAS_RDTSCP == 0
TSC_INLINE tsc_t tsc_read_serialized_end_fallback(void)
{
    tsc_half_t low, high;
    __asm__ volatile (
        "rdtsc\n\t"
        "mov %%eax, %0\n\t"
        "mov %%edx, %1\n\t"
        "cpuid\n\t"
        : "=r"(low), "=r"(high)
        :
        : "rax", "rbx", "rcx", "rdx"
    );
    return ((tsc_t)high << 32) | low;
}
#endif

/* Non inline function prototype declaration */
   

uint64_t calibrate_tsc_frequency(void);
uint64_t get_cpu_freq_from_proc(void);
uint64_t get_base_freq_from_proc(void);
int tsc_is_supported(void);

/* Macro for measuring function execution time */
#define TSC_MEASURE_FUNCTION(func_call) \
({ \
    tsc_t _start = tsc_read_serialized(); \
    func_call; \
    tsc_t _end = tsc_read_serialized_end(); \
    tsc_t _elapsed = tsc_cycles_elapsed(_start,_end); \
    _elapsed; \
})

#ifdef __cplusplus
}
#endif

#endif 
