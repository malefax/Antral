#include <stdio.h>
#include "./lib/tsc.h"

void none();
void none(){
  volatile int sum = 0;
    for(int i = 0; i < 1000; i++) {
        sum += i;
    }
}

int main(void){

if (!tsc_is_supported()) {
          printf("TSC not supported on this system\n");
          return 1;
      }
  /*tsc_t start = tsc_read_serialized();
    
   { youre code block here }

    tsc_t end = tsc_read_serialized_end();
    tsc_t cycles = tsc_cycles_elapsed(start, end);*/

tsc_t cycles = TSC_MEASURE_FUNCTION(none());
printf("TSC  supported on this system\n");
printf("Cycles: %llu\n",(long long unsigned int) cycles);    
uint64_t tsc_freq = calibrate_tsc_frequency();          
uint64_t freq = get_cpu_freq_from_proc();
if (freq >0){

printf("Found CPU frequency: %.2f MHz\n",( freq / 1000000.0));

}

else{

printf("Could not find CPU frequency in /proc/cpuinfo\n");

}

uint64_t time = tsc_to_nanoseconds(cycles, freq);
uint64_t Ttime = tsc_to_nanoseconds(cycles, tsc_freq);
printf("CFTIME (ns): %lu\n", time);
printf("TFTIME (ns): %lu\n", Ttime);
printf("TSC freq: %.2f MHz\n", tsc_freq / 1000000.0);
printf("Difference: %.2f MHz\n", (double)(tsc_freq - freq) / 1000000.0);
return 0;

}
