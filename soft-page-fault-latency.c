// Command to compile the program:
// gcc -O0 -o testProj proj1.c -lm
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <math.h>
static inline uint64_t getCycles(void)
{
#if defined(__ARM_ARCH_7A__)
    uint32_t r;
    asm volatile("mrc p15, 0, %0, c9, c13, 0\t\n"
                 : "=r"(r));   /* Read PMCCNTR
                                */
    return ((uint64_t)r) << 6; /* 1 tick = 64
    clocks */
#elif defined(__x86_64__)
    unsigned a, d;
    asm volatile("rdtsc"
                 : "=a"(a), "=d"(d));
    return ((uint64_t)a) | (((uint64_t)d) << 32);
#elif defined(__i386__)
    uint64_t ret;
    asm volatile("rdtsc"
                 : "=A"(ret));
    return ret;
#else
    return 0;
#endif
}
// Function used to grab the amount of miliseconds passed from cpu counter
static inline uint32_t getMillisecondCounter(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (uint32_t)(t.tv_sec * 1000 + t.tv_nsec / 1000000);
}
// Function call used to grab clock speed of current CPU in use
static inline float getClockSpeed(void)
{
    const uint64_t cycles = getCycles();
    const uint32_t millis = getMillisecondCounter();
    int lastResult = 0;
    for (;;)
    {
        int n = 1000000;
        while (--n > 0)
        {
        }
        const uint32_t millisElapsed = getMillisecondCounter() - millis;
        const uint64_t cyclesNow = getCycles();
        if (millisElapsed > 80)
        {
            const float newResult = (((cyclesNow - cycles) / millisElapsed) /
                                     1000.0);
            if (millisElapsed > 500 || (lastResult == newResult && newResult >
                                                                       100))
                return newResult;
            lastResult = newResult;
        }
    }
}
static inline uint64_t rdtsc()
{
    unsigned int eax, edx;
    __asm__ __volatile__("rdtsc"
                         : "=a"(eax), "=d"(edx));
    return ((uint64_t)edx << 32) | eax;
}
int main()
{
    float getClock = getClockSpeed();
    // Dynamically calculates clock speed of current core
    printf("Clock speed: %.0f Hz | %.3f GHz\n", (getClock * 1000000),
           ((double)getClock) / 1000.0);
    // Min, max, sum value initilaizations
    long pagesize = sysconf(_SC_PAGESIZE);
    double minPgFaultSpeed = 2000000;
    double maxPgFaultSpeed = -2;
    double sumSpeed = 0;
    double standDevSum = 0;
    // Main loop body to run 10,000 page faults
    int i;
    for (i = 0; i < 10000; i++)
    {
        // Makes virtual page request using mmap()
        uint8_t *first = mmap(NULL, pagesize, PROT_READ | PROT_WRITE,
                              MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        // Starts timer
        uint64_t start = rdtsc();
        // Generates soft page fault by writing memory to allocated virtual page
        *first = 10;
        uint64_t end = rdtsc();
        // Ends timer, calcualtes time taken given calculated clock speed,
        deallocates memory page int f = munmap(first, pagesize);
        uint64_t elapsed = end - start;
        // Calculates time taken in seconds for page fault latency
        // printf("Elapsed time: %lu cycles\n", elapsed);
        double foundSpeed = (double)(elapsed / (getClock * 1000000));
        // Checks to see if found speed is a min or max
        if (foundSpeed > maxPgFaultSpeed)
            maxPgFaultSpeed = foundSpeed;
        if (foundSpeed < minPgFaultSpeed)
            minPgFaultSpeed = foundSpeed;
        sumSpeed += foundSpeed;
        standDevSum += (foundSpeed * foundSpeed);
        // Prints speed of each fault for debugging
        // if(i<10)
        // printf("Found Speed of operation: %.11f s | %.11f us\n",
foundSpeed, (float)(foundSpeed*1000000));
    }
    // Printing Min and Max Soft Page Fault speeds
    printf("Min found speed of soft page Fault: %.11f s | %.11f us\n",
           minPgFaultSpeed, (float)(minPgFaultSpeed * 1000000));
    printf("Max found speed of soft page Fault: %.11f s | %.11f us\n",
           maxPgFaultSpeed, (float)(maxPgFaultSpeed * 1000000));
    // Calculating average speed and printing in both seconds and microseconds
    double averageSpeed = sumSpeed / 10000.0;
    printf("Average found speed of soft page Fault: %.11f s | %.11f us\n",
           averageSpeed, (float)(averageSpeed * 1000000));
    // Calculating Stadnard deviation and printing
    double standDevDiv = standDevSum / 10000.0;
    double standDev = sqrt(standDevDiv);
printf("Standard Deviation across 10,000 soft page faults: %.11f s | %.11f
us\n", standDev, (float)(standDev*1000000));
return 0;
}