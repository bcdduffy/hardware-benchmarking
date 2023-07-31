// Command to compile the program
// gcc proj3.c -o proj3_out -fopenmp -lm
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <stdint.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
static inline uint64_t getCycles(void)
{
#if defined(__ARM_ARCH_7A__)
    uint32_t r;
    asm volatile("mrc p15, 0, %0, c9, c13, 0\t\n"
                 : "=r"(r));
    return ((uint64_t)r) << 6;
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
static inline uint32_t getMillisecondCounter(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (uint32_t)(t.tv_sec * 1000 + t.tv_nsec / 1000000);
}
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
int val = 3;
int w1, w2;
double time2;
void func1(int *p, int *o, int numCores)
{
    unsigned long long size = 31 * 1024ULL * 1024ULL;
    omp_set_num_threads(numCores);
#pragma omp parallel for
    for (int i = 0; i < size; i += 1)
    {
        o[i] = sqrt(p[i]);
    }
}
void func2(int *p, int *o, int numCores)
{
    unsigned long long size = 4 * 1024ULL * 1024ULL;
    omp_set_num_threads(numCores);
#pragma omp parallel for
    for (int i = 0; i < size; i += 1)
    {
        o[i] = sqrt(p[i]);
    }
}
int main(int argc, char **argv)
{
    int *p, *o;
    unsigned long long size = 31 * 1024ULL * 1024ULL;
    printf("Starting program for Assignment 3\n");
    p = (int *)malloc(sizeof(int) * size);
    o = (int *)malloc(sizeof(int) * size);
    for (unsigned long long i = 0; i < size; i++)
    {
        p[i] = 1 * rand();
    }
    printf("Testing Speedups for Function 1: \n");
    float getClock = getClockSpeed();
    uint64_t start = rdtsc();
    func1(p, o, 1);
    uint64_t end = rdtsc();
    uint64_t elapsed = end - start;
    double foundSpeed = (double)(elapsed / (getClock * 1000000));
    float baseSpeed = (float)(foundSpeed * 1000000);
    int coreNum[] = {2, 4, 8, 16, 28};
    float core_speed_ups[] = {0, 0, 0, 0, 0};
    for (int y = 0; y < 5; y++)
    {
        float speed_2T_cum = 0;
        for (int j = 0; j < 10; j++)
        {
            start = rdtsc();
            omp_set_num_threads(coreNum[y]);
            func1(p, o, coreNum[y]);
            end = rdtsc();
            elapsed = end - start;
            foundSpeed = (double)(elapsed / (getClock * 1000000));
            float foundSpeedMicroSec = (float)(foundSpeed * 1000000);
            speed_2T_cum = speed_2T_cum + foundSpeedMicroSec;
        }
        float avg_2T_speed = speed_2T_cum / 10.0;
        core_speed_ups[y] = baseSpeed / avg_2T_speed;
    }
    printf("Part 1.a) 2T: %.4f, 4T: %.4f, 8T: %.4f, 16T: %.4f, 28T: %.4f. \n",
           core_speed_ups[0], core_speed_ups[1], core_speed_ups[2], core_speed_ups[3], core_speed_ ups[4]);
    printf("Testing Speedups for Function 2: \n");
    getClock = getClockSpeed();
    start = rdtsc();
    func2(p, o, 1);
    end = rdtsc();
    elapsed = end - start;
    foundSpeed = (double)(elapsed / (getClock * 1000000));
    baseSpeed = (float)(foundSpeed * 1000000);
    int coreNum2[] = {2, 4, 8, 16, 28};
    float core_speed_ups2[] = {0, 0, 0, 0, 0};
    for (int y = 0; y < 5; y++)
    {
        float speed_2T_cum = 0;
        for (int j = 0; j < 10; j++)
        {
            start = rdtsc();
            omp_set_num_threads(coreNum2[y]);
            func2(p, o, coreNum2[y]);
            end = rdtsc();
            elapsed = end - start;
            foundSpeed = (double)(elapsed / (getClock * 1000000));
            float foundSpeedMicroSec = (float)(foundSpeed * 1000000);
            speed_2T_cum = speed_2T_cum + foundSpeedMicroSec;
        }
        float avg_2T_speed = speed_2T_cum / 10.0;
        core_speed_ups2[y] = baseSpeed / avg_2T_speed;
    }
    printf("Part 2.b) 2T: %.4f, 4T: %.4f, 8T: %.4f, 16T: %.4f, 28T: %.4f. \n",
           core_speed_ups2[0], core_speed_ups2[1], core_speed_ups2[2], core_speed_ups2[3], core_sp eed_ups2[4]);
printf("Part 3.c) For which of the two functions can the multiprocessor achieve
higher speedup? Explain your answer: Of the two functions it appeared that funciton
1 was able to achieve the higher speedup. Looking at the values taken for the two
funcitons as more cores were added they both started off with a relatively similar
speedup at first with two times speedup for two threads, then a four times speedup
for four cores and an eight times speedup for eight cores. They begin to diverge
after the eight core mark where function 1 has a 14 times speedup on 16 cores over
the 12 times speedup of function 2 and a 22 tiemes speedup over the 16 times
speedup of fucntion 2 on 28 cores. By the time both are at 28 cores the speedup of
funciton 1 appears to be about 40 percent faster than its counterpart. This would
make me conclude that of the two functions the one with the higher speedup would be
the first. The cause of this generally comes from the difference in size of the two
for loops and arrays.");
free(p);
free(o);
return 0;
}