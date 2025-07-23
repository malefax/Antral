#include "../lib/tsc.h"

/* calibrate frequency using intrupt */

uint64_t calibrate_tsc_frequency(void) 
{
    struct timespec start_time, end_time;
    tsc_t start_tsc, end_tsc;
    
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    start_tsc = tsc_read_serialized();
    
    /* Sleep for a measurable duration (100ms) */

    usleep(100000); 
    
    end_tsc = tsc_read_serialized_end();
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    
    uint64_t elapsed_ns = (end_time.tv_sec - start_time.tv_sec) * 1000000000ULL +
                          (end_time.tv_nsec - start_time.tv_nsec);
    tsc_t elapsed_cycles = tsc_cycles_elapsed(start_tsc, end_tsc);
    uint64_t tsc_freq = (elapsed_cycles * 1000000000ULL) / elapsed_ns;
    return tsc_freq;
}

/*Get CPU frequency it it can change dynamically based on modes
like Turbo Mode */

uint64_t get_cpu_freq_from_proc(void) 
{
    FILE *fp;
    char line[256];
    double freq_mhz = 0.0;
    
    fp = fopen("/proc/cpuinfo", "r");
    if (fp == NULL) {
        printf("Error: Cannot open /proc/cpuinfo\n");
        return 0;
    }
    
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "cpu MHz", 7) == 0) {
            char *colon = strchr(line, ':');
            if (colon != NULL) {
                freq_mhz = strtod(colon + 1, NULL);
                break;
            }
        }
    }
    
    fclose(fp);
    
    if (freq_mhz > 0) {

        return (uint64_t)(freq_mhz * 1000000.0);
;
    } else {
    
        return 0;

    }
}

/* Look for model name which often contains base frequency */

uint64_t get_base_freq_from_proc(void) 
{
    FILE *fp;
    char line[256];
    
    fp = fopen("/proc/cpuinfo", "r");
    if (fp == NULL) {
        return 0;
    }
    
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "model name", 10) == 0) {
            printf("CPU Model: %s", line);
            break;
        }
    }
    
    fclose(fp);
    return 0;
}

/* Check for the TSC support */

int tsc_is_supported(void)
{
    uint32_t eax, ebx, ecx, edx;
    
    __asm__ volatile (
        "cpuid"
        : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
        : "a"(1)
    );
    
    if (!(edx & (1 << 4))) {
        return 0;
    }
    
    /* Check for invariant TSC (recommended for timing) */
    __asm__ volatile (
        "cpuid"
        : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
        : "a"(0x80000007)
    );
    
    /* Bit 8 of EDX indicates invariant TSC */
    return (edx & (1 << 8)) ? 1 : 0;
}
