// Command to compile the program
// gcc proj4.c -o proj4_out -fopenmp -lm
// To Run:
//  ./proj4_out
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>
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
void func1(int *p, int *o)
{
    unsigned long long size = 31 * 1024ULL * 1024ULL;
    for (int i = 0; i < size; i += 1)
    {
        o[i] = sqrt(p[i]);
    }
}
void func1_pol1(int *p, int *o)
{
    unsigned long long size = 31 * 1024ULL * 1024ULL;
    omp_set_num_threads(28);
#pragma omp parallel for schedule(static, 1)
    for (int i = 0; i < size; i += 1)
    {
        o[i] = sqrt(p[i]);
    }
}
void func1_pol2(int *p, int *o)
{
    unsigned long long size = 31 * 1024ULL * 1024ULL;
    omp_set_num_threads(28);
#pragma omp parallel for schedule(static, 1024)
    for (int i = 0; i < size; i += 1)
    {
        o[i] = sqrt(p[i]);
    }
}
void func1_pol3(int *p, int *o)
{
    unsigned long long size = 31 * 1024ULL * 1024ULL;
    omp_set_num_threads(28);
#pragma omp parallel for schedule(dynamic, 1)
    for (int i = 0; i < size; i += 1)
    {
        o[i] = sqrt(p[i]);
    }
}
void func1_pol4(int *p, int *o)
{
    unsigned long long size = 31 * 1024ULL * 1024ULL;
    omp_set_num_threads(28);
#pragma omp parallel for schedule(dynamic, 1024)
    for (int i = 0; i < size; i += 1)
    {
        o[i] = sqrt(p[i]);
    }
}
void func3(int *p)
{
    unsigned long long size = 31 * 1024ULL * 1024ULL, square_root_product = 0;
    for (int i = 0; i < size; i += 1)
    {
        square_root_product *= sqrt(p[i]);
    }
}
void func3_pol1(int *p)
{
    unsigned long long size = 31 * 1024ULL * 1024ULL, square_root_product = 0;
    omp_set_num_threads(28);
#pragma omp parallel for schedule(static, 1) reduction(+ : square_root_product)
    for (int i = 0; i < size; i += 1)
    {
        square_root_product *= sqrt(p[i]);
    }
}
void func3_pol2(int *p)
{
    unsigned long long size = 31 * 1024ULL * 1024ULL, square_root_product = 0;
    omp_set_num_threads(28);
#pragma omp parallel for schedule(static, 1024) reduction(+ : square_root_product)
    for (int i = 0; i < size; i += 1)
    {
        square_root_product *= sqrt(p[i]);
    }
}
void func3_pol3(int *p)
{
    unsigned long long size = 31 * 1024ULL * 1024ULL, square_root_product = 0;
    omp_set_num_threads(28);
#pragma omp parallel for schedule(dynamic, 1) reduction(+ : square_root_product)
    for (int i = 0; i < size; i += 1)
    {
        square_root_product *= sqrt(p[i]);
    }
}
void func3_pol4(int *p)
{
    unsigned long long size = 31 * 1024ULL * 1024ULL, square_root_product = 0;
    omp_set_num_threads(28);
#pragma omp parallel for schedule(dynamic, 1024) reduction(+ : square_root_product)
    for (int i = 0; i < size; i += 1)
    {
        square_root_product *= sqrt(p[i]);
    }
}
int main(int argc, char **argv)
{
    int *p, *o;
    unsigned long long size = 31 * 1024ULL * 1024ULL;
    printf("Starting program for Assignment 4\n");
    p = (int *)malloc(sizeof(int) * size);
    o = (int *)malloc(sizeof(int) * size);
    for (unsigned long long i = 0; i < size; i++)
    {
        p[i] = 1 * rand();
    }
    printf("Testing Speedups for Function 1: \n");
    float getClock = getClockSpeed();
    uint64_t start = rdtsc();
    omp_set_num_threads(1);
    func1(p, o);
    uint64_t end = rdtsc();
    uint64_t elapsed = end - start;
    double foundSpeed = (double)(elapsed / (getClock * 1000000));
    float baseSpeed = (float)(foundSpeed * 1000000);
    float core_speed_ups[] = {0, 0, 0, 0};
    float core_speeds[] = {0, 0, 0, 0};
    for (int y = 0; y < 4; y++)
    {
        float speed_2T_cum = 0;
        for (int j = 0; j < 10; j++)
        {
            start = rdtsc();
            if (y == 0)
                func1_pol1(p, o);
            else if (y == 1)
                func1_pol2(p, o);
            else if (y == 2)
                func1_pol3(p, o);
            else
                func1_pol4(p, o);
            end = rdtsc();
            elapsed = end - start;
            foundSpeed = (double)(elapsed / (getClock * 1000000));
            float foundSpeedMicroSec = (float)(foundSpeed * 1000000);
            speed_2T_cum = speed_2T_cum + foundSpeedMicroSec;
        }
        float avg_2T_speed = speed_2T_cum / 10.0;
        core_speed_ups[y] = baseSpeed / avg_2T_speed;
        core_speeds[y] = avg_2T_speed;
    }
printf("Part 1.a) Policy 1: %.4f, Policy 2: %.4f, Policy 3: %.4f, Policy 4: %.4f.
\n", core_speed_ups[0],core_speed_ups[1],core_speed_ups[2],core_speed_ups[3]);
printf("For Part 1, the policy that produces the highest speedup for function1 of
the four is policy 2, the even, coarse policy. It produces on average about a 22.5
times speedup over the single core execution of function1. The second fastest
policy is policy 4 the dynamic coarse policy having a speed of about 22 times
making the fastest policy, policy 2, about 1.025 times faster over the second
fastest, policy 4.\n");
printf("Speedup of Policy 2 (fastest) over Policy 4 (second fastest) : %.4f\n\n",
core_speeds[3]/core_speeds[1]);
printf("Testing Speedups for Function 3: \n");
getClock = getClockSpeed();
start = rdtsc();
omp_set_num_threads(1);
func3(p);
end = rdtsc();
elapsed = end - start;
foundSpeed = (double)(elapsed/(getClock*1000000));
baseSpeed = (float)(foundSpeed*1000000);
float core_speed_ups2[] = {0, 0, 0, 0};
float core_speeds2[] = {0, 0, 0, 0};
for(int y = 0; y < 4; y++){
        float speed_2T_cum = 0;
        for (int j = 0; j < 10; j++)
        {
            start = rdtsc();
            if (y == 0)
                func3_pol1(p);
            else if (y == 1)
                func3_pol2(p);
            else if (y == 2)
                func3_pol3(p);
            else
                func3_pol4(p);
            end = rdtsc();
            elapsed = end - start;
            foundSpeed = (double)(elapsed / (getClock * 1000000));
            float foundSpeedMicroSec = (float)(foundSpeed * 1000000);
            speed_2T_cum = speed_2T_cum + foundSpeedMicroSec;
        }
        float avg_2T_speed = speed_2T_cum / 10.0;
        core_speed_ups2[y] = baseSpeed / avg_2T_speed;
        core_speeds2[y] = avg_2T_speed;
}
printf("Part 2.b) Policy 1: %.4f, Policy 2: %.4f, Policy 3: %.4f, Policy 4: %.4f.
\n", core_speed_ups2[0],core_speed_ups2[1],core_speed_ups2[2],core_speed_ups2[3]);
printf("For Part 2, the policy that produces the highest speedup for function 3
of the four is policy 2, the even, coarse policy. It produces on average about a
20.0 times speedup over the single core execution of function1. The second fastest
policy is policy 4 the dynamic coarse policy having a speedup of about 19.7 times
making the fastest policy, policy 2, 1.002 times faster in its speedup over the
second fastest, policy 4.\n");
printf("Speedup of Policy 2 (fastest) over Policy 4 (second fastest) : %.4f\n\n",
core_speeds2[3]/core_speeds2[1]);
printf("For Part 3.C, Policy 2, the static coarse grained policy seems to
outperform the other policies that we have in the assignment in both functions. The
only policy that seems to compete with it is policy 4 the dynamic coarse grained
policy. We know that the static policy would in the case where the instructions
take the same amount of time would have a better performance than dynamic. This is
because the need for dynamic switching is eliminated and the overhead of
dyanmically switching now is added to the execution time. So from this we can
understand why policy 2 did better than policy 4.\n\n");
printf("In the case of fine grained over coarse grained it seems as if the coarse
grained policies, policy 2 and 4 performed much better than policies 1 and 3 by an
order of magnitude. The coaese grained policies in this case we would know would do
better than the fine grained becuase of how paralellizable the loops are in the
functions. The instructions can be performed independently of one another making
the load balancing even decreasing the need for thread switching for performance.
Becuase there isn't as much a need for thread switching and the fine grain policies
force these thread switching between iterations the context switching overhead
created by the fine grained policy signifigantly worsens the performance of the
functions.\n\n");
printf("To summarize, Policy 2 performs the best because: First it is static,
eliminating the overhead added by a dynamic policy which would only be needed in
the case where threads take different amounts of time, which doesnt happen here.
And second Becuase it is coarse grained in its iteration interleaving which reduces
signifigantly the amount of overhead added by context switching between iterations
that will all generally take about the same amount of time to execute.\n\n");
printf("For reference all of this was build and tested on the rlogin environment,
there were issues with the CRC wendesday afternnon when I was putting together the
code and testing so the TA told me that I should continue work on the rlogin
network. Every once in a while these values for speedup do change and come out
differently but generally they match the order of speedup described above\n\n");
free(p);
free(o);
return 0;
}