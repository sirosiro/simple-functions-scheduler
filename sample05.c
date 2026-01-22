/*
  sample05.c - Ring Buffer Library Test using SFS

  This sample demonstrates:
    - Using the Ring Buffer library within an SFS task.
    - Verifying read/write operations, overwrite behavior, and peek functionality.
    - Testing dependency injection with custom copy functions.
*/
#include <stdio.h>
#include <string.h> /* For dependency injection test and verification */
#include "sfs.h"
#include "ring_buffer.h"

#define TEST_BUFFER_SIZE 10

/* Global control for main loop */
static int g_stop_task = 0;

/* Ring Buffer Resources */
static unsigned char buffer_mem[TEST_BUFFER_SIZE];
static ring_buffer_t rb_instance;
static rb_handle_t rb_handle = &rb_instance;

/* Custom copy function using standard memcpy, for testing injection */
static void my_memcpy_copy(void* dest, const void* src, rb_size_t len) {
    memcpy(dest, src, len);
}

/* Helper to print buffer content */
static void print_buffer_content(const char* label, const unsigned char* buf, rb_size_t len) {
    rb_size_t i;
    printf("%s: \"", label);
    for (i = 0; i < len; ++i) {
        if (buf[i] >= ' ' && buf[i] <= '~') { /* Printable ASCII */
            printf("%c", buf[i]);
        } else { /* Non-printable */
            printf("[%02X]", buf[i]);
        }
    }
    printf("\"\n");
}

/* --- Test Task --- */
void ring_buffer_test_task(void) {
    unsigned char read_buf[TEST_BUFFER_SIZE + 5]; /* Slightly larger for safety */
    rb_size_t bytes_written, bytes_read;
    const char* test_data1 = "Hello";
    const char* test_data2 = "World";
    const char* overflow_data = "ABC";
    const char* custom_data = "Inject";

    printf("\n--- Ring Buffer Test Task Started ---\n\n");

    /* --- Test Case 1: Basic Write and Read (No overwrite, default copy) --- */
    printf("Test Case 1: Basic Write and Read (No overwrite, default copy)\n");
    rb_init(rb_handle, buffer_mem, TEST_BUFFER_SIZE, RB_FALSE, NULL, NULL);
    printf("Buffer initialized. Size: %u, Used: %u, Free: %u\n", rb_handle->size, rb_get_used_space(rb_handle), rb_get_free_space(rb_handle));

    bytes_written = rb_write(rb_handle, test_data1, strlen(test_data1));
    printf("Wrote \"%s\" (%u bytes). Actual: %u bytes.\n", test_data1, (unsigned int)strlen(test_data1), bytes_written);
    
    bytes_written = rb_write(rb_handle, test_data2, strlen(test_data2));
    printf("Wrote \"%s\" (%u bytes). Actual: %u bytes.\n", test_data2, (unsigned int)strlen(test_data2), bytes_written);

    /* Buffer should now be full */
    if (rb_get_used_space(rb_handle) != TEST_BUFFER_SIZE || rb_get_free_space(rb_handle) != 0 || !rb_handle->is_full) {
        printf("ERROR: Buffer not reported as full correctly!\n");
    } else {
        printf("Buffer is full as expected.\n");
    }

    /* Read first part */
    memset(read_buf, 0, sizeof(read_buf));
    bytes_read = rb_read(rb_handle, read_buf, 5);
    printf("Read %u bytes. ", bytes_read);
    print_buffer_content("Read data", read_buf, bytes_read);

    /* Read second part */
    memset(read_buf, 0, sizeof(read_buf));
    bytes_read = rb_read(rb_handle, read_buf, 5);
    printf("Read %u bytes. ", bytes_read);
    print_buffer_content("Read data", read_buf, bytes_read);

    if (rb_get_used_space(rb_handle) == 0 && !rb_handle->is_full) {
        printf("Test Case 1 PASSED.\n\n");
    } else {
        printf("ERROR: Buffer not empty after read!\n\n");
    }


    /* --- Test Case 2: Buffer Full (No Overwrite) --- */
    printf("Test Case 2: Buffer Full (No Overwrite)\n");
    rb_init(rb_handle, buffer_mem, TEST_BUFFER_SIZE, RB_FALSE, NULL, NULL);
    rb_write(rb_handle, "0123456789", 10);
    
    bytes_written = rb_write(rb_handle, overflow_data, strlen(overflow_data));
    if (bytes_written == 0) {
        printf("Buffer correctly rejected overflow write.\n");
        printf("Test Case 2 PASSED.\n\n");
    } else {
        printf("ERROR: Overflow write should have failed!\n\n");
    }


    /* --- Test Case 3: Buffer Full (With Overwrite) --- */
    printf("Test Case 3: Buffer Full (With Overwrite)\n");
    rb_init(rb_handle, buffer_mem, TEST_BUFFER_SIZE, RB_TRUE, NULL, NULL);
    rb_write(rb_handle, "0123456789", 10);

    /* Overwrite first 3 bytes (012) with ABC */
    bytes_written = rb_write(rb_handle, "ABC", 3);
    printf("Wrote \"ABC\" with overwrite. Actual: %u bytes.\n", bytes_written);

    memset(read_buf, 0, sizeof(read_buf));
    bytes_read = rb_read(rb_handle, read_buf, 10);
    printf("Read %u bytes. ", bytes_read);
    print_buffer_content("Read data", read_buf, bytes_read);
    
    if (strncmp((char*)read_buf, "3456789ABC", 10) == 0) {
        printf("Test Case 3 PASSED.\n\n");
    } else {
        printf("ERROR: Overwrite read data mismatch!\n\n");
    }


    /* --- Test Case 4: Peek Functionality --- */
    printf("Test Case 4: Peek Functionality\n");
    rb_init(rb_handle, buffer_mem, TEST_BUFFER_SIZE, RB_FALSE, NULL, NULL);
    rb_write(rb_handle, "DataForPeek", 10);

    memset(read_buf, 0, sizeof(read_buf));
    bytes_read = rb_peek(rb_handle, read_buf, 5);
    printf("Peeked %u bytes. ", bytes_read);
    print_buffer_content("Peeked data", read_buf, bytes_read);

    if (rb_get_used_space(rb_handle) == 10) {
        printf("Used space unchanged after peek.\n");
        printf("Test Case 4 PASSED.\n\n");
    } else {
        printf("ERROR: Peek changed used space!\n\n");
    }


    /* --- Test Case 5: Custom Copy Functions (Dependency Injection) --- */
    printf("Test Case 5: Custom Copy Functions\n");
    rb_init(rb_handle, buffer_mem, TEST_BUFFER_SIZE, RB_FALSE, my_memcpy_copy, my_memcpy_copy);

    bytes_written = rb_write(rb_handle, custom_data, strlen(custom_data));
    printf("Wrote \"%s\" using custom copy.\n", custom_data);

    memset(read_buf, 0, sizeof(read_buf));
    bytes_read = rb_read(rb_handle, read_buf, 6);
    printf("Read %u bytes using custom copy. ", bytes_read);
    print_buffer_content("Read data", read_buf, bytes_read);

    if (strncmp((char*)read_buf, "Inject", 6) == 0) {
        printf("Test Case 5 PASSED.\n\n");
    } else {
        printf("ERROR: Custom copy data mismatch!\n\n");
    }

    printf("--- All Tests Completed ---\n");
    
    /* Signal main loop to exit */
    g_stop_task = 1;
    SFS_kill();
}

/* --- Main Function --- */
int main(void) {
    SFS_initialize();
    
    /* Register the test task */
    SFS_fork("RB_TEST", 0, ring_buffer_test_task);
    
    while (!g_stop_task) {
        if (SFS_dispatch() == 0) {
            break;
        }
    }
    return 0;
}
