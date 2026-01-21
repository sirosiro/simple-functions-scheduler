/*
  sample04.c - Refactored FIFO Demo for Edge Case Testing

  This sample demonstrates:
    - Using the refactored, type-aware FIFO library.
    - A test scenario that first fills the FIFO until it is full.
    - Then, empties the FIFO until it is empty.
    - This tests the boundary conditions and pointer wrap-around logic.
*/
#include <stdio.h>
#include "sfs.h"
#include "fifo.h"

/* FIFO definitions */
#define FIFO_CAPACITY 8
static long fifo_buffer[FIFO_CAPACITY];
static struct FIFO_cb my_fifo;
static int g_stop_task = 0;

/* --- Test Task --- */
void fifo_test_task(void)
{
  int i;
  long element;

  printf("\n--- Starting FIFO Fill Test ---\n");

  /* 1. Fill the FIFO until it's full */
  for (i = 0; i < FIFO_CAPACITY + 1; i++) {
    element = 100 + i;
    if (FIFO_push(&my_fifo, &element) == 0) {
      printf("Pushed: %ld (count: %u)\n", element, my_fifo.count);
    } else {
      printf("Push failed as expected. FIFO is full. (count: %u)\n", my_fifo.count);
    }
  }

  if (!FIFO_is_full(&my_fifo)) {
      printf("ERROR: FIFO should be full!\n");
  } else {
      printf("SUCCESS: FIFO is full.\n");
  }


  printf("\n--- Starting FIFO Empty Test ---\n");

  /* 2. Empty the FIFO until it's empty */
  for (i = 0; i < FIFO_CAPACITY + 1; i++) {
    if (FIFO_pop(&my_fifo, &element) == 0) {
      printf("Popped: %ld (count: %u)\n", element, my_fifo.count);
    } else {
      printf("Pop failed as expected. FIFO is empty. (count: %u)\n", my_fifo.count);
    }
  }

  if (!FIFO_is_empty(&my_fifo)) {
      printf("ERROR: FIFO should be empty!\n");
  } else {
      printf("SUCCESS: FIFO is empty.\n");
  }
  
  printf("\n--- Test Finished ---\n");
  
  /* 3. Stop the scheduler */
  g_stop_task = 1;
  SFS_kill();
}

/* --- Main Function --- */
int main(void)
{
  printf("--- FIFO Edge Case Test ---\n");

  /* Initialize libraries */
  SFS_initialize();
  FIFO_initialize(&my_fifo, fifo_buffer, FIFO_CAPACITY, FIFO_TYPE_LONG);
  
  if (!FIFO_is_empty(&my_fifo)) {
      printf("ERROR: FIFO should be empty after initialization!\n");
  }

  /* Register the test task */
  SFS_fork("FIFO_TEST", 0, fifo_test_task);

  /* Main loop - dispatch tasks until test is complete */
  while(!g_stop_task) {
    if (SFS_dispatch() == 0) {
        /* Break if no tasks are left to run */
        break;
    }
  }

  return 0;
}
