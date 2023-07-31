// Compile Command: g++ proj2.c -I/usr/local/lib -o proj2 -lpapi
// Library Export: export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
// Space config: ulimit -s unlimited
// Run: ./proj2
#include <stdio.h>
#include <stdlib.h>
#include "papi.h"
#define ROWS 4096 // do not change
#define COLS 512  // do not change
typedef struct
{
    unsigned long long toAccess;  // use this. do not change
    unsigned long long ignore[7]; // ignore this. do not change
} elementOf2DArray;
void countAccess(int row, int col, elementOf2DArray (&myArray)[ROWS][COLS]);
void countAccessRev(int row, int col, elementOf2DArray (&myArray)[ROWS][COLS]);
void part1A_1B(elementOf2DArray (&myArray)[ROWS][COLS])
{

    unsigned long long sum1A = 0;
    int retval = 0, EventSet = PAPI_NULL;
    unsigned long long count = 0, sum = 0;
    long long values[2];
    const int eventlist[] = {PAPI_L2_DCA, PAPI_L2_DCM};
    retval = PAPI_library_init(PAPI_VER_CURRENT);
    if (retval != PAPI_VER_CURRENT)
    {
        fprintf(stderr, "Error initializing PAPI! %s\n", PAPI_strerror(retval));
    }
    if ((retval = PAPI_create_eventset(&EventSet)) != PAPI_OK)
        fprintf(stderr, "Error creating EventSet! %s\n",
                PAPI_strerror(retval));
    if ((retval = PAPI_add_event(EventSet, eventlist[0])) != PAPI_OK)
        printf("Error importing PAPI_L2_DCA\n");
    if ((retval = PAPI_add_event(EventSet, eventlist[1])) != PAPI_OK)
        printf("Error importing PAPI_L2_DCM\n");
    if ((retval = PAPI_start(EventSet)) != PAPI_OK)
        printf("PAPI Stop Error\n");
    // Code to be instrumented
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            sum1A = sum1A + myArray[r][c].toAccess;
        }
    }
    if ((retval = PAPI_stop(EventSet, values)) != PAPI_OK)
        printf("PAPI Stop Error\n");
    printf("\n __________________Answer 1A__________________ \
n");
    printf("Sum1A: %llu\n", sum1A);
    printf("L2 DCA: %llu L2 DCM: %llu\n", values[0], values[1]);

    unsigned long long sum1B = 0;
    int retvalB = 0, EventSetB = PAPI_NULL;
    unsigned long long countB = 0, sumB = 0;
    long long valuesB[2];
    const int eventlistB[] = {PAPI_L2_DCA, PAPI_L2_DCM};
    retvalB = PAPI_library_init(PAPI_VER_CURRENT);
    if (retvalB != PAPI_VER_CURRENT)
    {
        fprintf(stderr, "Error initializing PAPI! %s\n", PAPI_strerror(retvalB));
    }
    if ((retvalB = PAPI_create_eventset(&EventSetB)) != PAPI_OK)
        fprintf(stderr, "Error creating EventSet! %s\n",
                PAPI_strerror(retvalB));
    if ((retvalB = PAPI_add_event(EventSetB, eventlistB[0])) != PAPI_OK)
        printf("Error importing PAPI_L2_DCA\n");
    if ((retvalB = PAPI_add_event(EventSetB, eventlistB[1])) != PAPI_OK)
        printf("Error importing PAPI_L2_DCM\n");
    if ((retvalB = PAPI_start(EventSetB)) != PAPI_OK)
        printf("PAPI Stop Error\n");
    // Code to be instrumented
    //
    for (int c = 0; c < COLS; c++)
    {
        for (int r = 0; r < ROWS; r++)
        {
            sum1B = sum1B + myArray[r][c].toAccess;
        }
    }
    if ((retvalB = PAPI_stop(EventSetB, valuesB)) != PAPI_OK)
        printf("PAPI Stop Error\n");
    printf("\n __________________Answer 1B__________________ \n");
    printf("Sum1B: %llu\n", sum1B);
    printf("L2 DCA: %llu L2 DCM: %llu\n", valuesB[0], valuesB[1]);
 
    printf("\n________________Conceptual Answer____________________\n");
printf("The number of cache accesses in A is signifigantly smaller than B becuase
when accessing elements of an\n array in row major order the sequential accesses
have a spatial locality due to how adjacent array elements are stored\n which
increases the success of the TLB. Because of how the C compiler works, elements
accessed sequentially by row\n are separated by the size of a single element while
elements accessed by columns then are separated by the size on\n an enire row
giving it almost no spatial locality. This lack of spatial locality makes for a
higher miss rate of the\n TLB because sequeantial columns arent stored in the same
addresses causing a higher access rate\n to the L2 cache unfortunately.\n");
}
void part2(elementOf2DArray (&myArray)[ROWS][COLS])
{

printf("_____PART 2: Beginning gradual increase to evaluate TLB_____\n");
for (int i = 1; i < 400; i += 5)
{
    countAccessRev(1, i, myArray);
}

//
printf("It appears that at around 256 entries that the TLB seems to start
missing\n The access rate begins to jump really rapidly after then indicating its
not being used as much .\n");
}
void part3(elementOf2DArray (&myArray)[ROWS][COLS])
{

for (int i = 1; i < 200; i += 5)
{
    countAccess(i, i, myArray);
}

//
printf("It appears that at around 1000KB of space the miss rate begins
to rapidly jump about doubling its miss rate\n over a few iterations. This would
make it seem like the size would have to around that big.\n My educated guess
because the sizes comes in powers of two would be that I would have a 1024KB size
L2 cache.\n");
}
int main()
{ // Do not change anything in main
// Allocating and initializing a 2D array.
elementOf2DArray array[ROWS][COLS];
for (int i = 0; i < ROWS; i++)
{
    for (int j = 0; j < COLS; j++)
    {
        // printf("debug\n");
        array[i][j].toAccess = i * j;
    }
}
part1A_1B(array);
part2(array);
part3(array);
return 0;
}
void countAccess(int row, int col, elementOf2DArray (&myArray)[ROWS][COLS])
{
unsigned long long sum1B = 0;
int retvalB = 0, EventSetB = PAPI_NULL;
unsigned long long countB = 0, sumB = 0;
long long valuesB[2];
const int eventlistB[] = {PAPI_L2_DCA, PAPI_L2_DCM};
retvalB = PAPI_library_init(PAPI_VER_CURRENT);
if (retvalB != PAPI_VER_CURRENT)
{
    fprintf(stderr, "Error initializing PAPI! %s\n", PAPI_strerror(retvalB));
}
if ((retvalB = PAPI_create_eventset(&EventSetB)) != PAPI_OK)
    fprintf(stderr, "Error creating EventSet! %s\n",
            PAPI_strerror(retvalB));
if ((retvalB = PAPI_add_event(EventSetB, eventlistB[0])) != PAPI_OK)
    printf("Error importing PAPI_L2_DCA\n");
if ((retvalB = PAPI_add_event(EventSetB, eventlistB[1])) != PAPI_OK)
    printf("Error importing PAPI_L2_DCM\n");
if ((retvalB = PAPI_start(EventSetB)) != PAPI_OK)
    printf("PAPI Stop Error\n");
// Code to be instrumented
//
for (int r = 0; r < row; r++)
{
    for (int c = 0; c < col; c++)
    {
        sum1B = sum1B + myArray[r][c].toAccess;
    }
}
long long int size = ((row * col) * 64) / 1000;
printf("Num ROWS: %d Num COLS: %d Size: %llu KB ", row, col, size);
printf("FIRST CALL | L2 DCA: %llu L2 DCM: %llu\n", valuesB[0], valuesB[1]);
if ((retvalB = PAPI_stop(EventSetB, valuesB)) != PAPI_OK)
    printf("PAPI Stop Error\n");
}
void countAccessRev(int row, int col, elementOf2DArray (&myArray)[ROWS][COLS])
{
unsigned long long sum1B = 0;
int retvalB = 0, EventSetB = PAPI_NULL;
unsigned long long countB = 0, sumB = 0;
long long valuesB[2];
const int eventlistB[] = {PAPI_L2_DCA, PAPI_L2_DCM};
retvalB = PAPI_library_init(PAPI_VER_CURRENT);
if (retvalB != PAPI_VER_CURRENT)
{
    fprintf(stderr, "Error initializing PAPI! %s\n", PAPI_strerror(retvalB));
}
if ((retvalB = PAPI_create_eventset(&EventSetB)) != PAPI_OK)
    fprintf(stderr, "Error creating EventSet! %s\n",
            PAPI_strerror(retvalB));
if ((retvalB = PAPI_add_event(EventSetB, eventlistB[0])) != PAPI_OK)
    printf("Error importing PAPI_L2_DCA\n");
if ((retvalB = PAPI_add_event(EventSetB, eventlistB[1])) != PAPI_OK)
    printf("Error importing PAPI_L2_DCM\n");
if ((retvalB = PAPI_start(EventSetB)) != PAPI_OK)
    printf("PAPI Stop Error\n");
// Code to be instrumented
//
for (int c = 0; c < col; c++)
{
    for (int r = 0; r < row; r++)
    {
        sum1B = sum1B + myArray[r][c].toAccess;
    }
}
printf("Num ROWS: %d Num COLS: %d ", row, col);
printf("FIRST CALL | L2 DCA: %llu L2 DCM: %llu\n", valuesB[0], valuesB[1]);
if ((retvalB = PAPI_stop(EventSetB, valuesB)) != PAPI_OK)
    printf("PAPI Stop Error\n");
}