#ifndef RING_BUFFER_H
#define RING_BUFFER_H

/*
 * Note: This library avoids stdint.h, stddef.h, and stdbool.h to comply with the
 * "no standard library" principle for maximum portability on bare-metal systems.
 * Custom types are defined below.
 */

/* Custom size type, equivalent to size_t */
typedef unsigned int rb_size_t;

/* Custom boolean type */
typedef unsigned char rb_bool;
#define RB_TRUE 1
#define RB_FALSE 0

/* Forward declaration of the main struct */
struct ring_buffer_s;

/**
 * @brief Handle for a ring buffer instance. It's a pointer to the main control struct.
 */
typedef struct ring_buffer_s* rb_handle_t;

/**
 * @brief Function pointer type for custom data copy operations.
 * @param dest Pointer to the destination memory.
 * @param src Pointer to the source memory.
 * @param len Number of bytes to copy.
 */
typedef void (*rb_copy_func_t)(void* dest, const void* src, rb_size_t len);

/**
 * @brief The main control structure for a ring buffer instance.
 * @note The user of the library is responsible for allocating memory for this struct.
 */
typedef struct ring_buffer_s {
    unsigned char* buffer;      /* Pointer to the user-provided buffer memory */
    rb_size_t size;             /* Total size of the buffer */
    rb_size_t head;             /* Write index */
    rb_size_t tail;             /* Read index */
    rb_bool is_full;            /* Flag to distinguish between empty and full states */
    rb_bool overwrite_on_full;  /* Flag to allow overwriting old data when full */
    rb_copy_func_t read_from_ring; /* Function to copy data from the ring buffer */
    rb_copy_func_t write_to_ring;  /* Function to copy data to the ring buffer */
} ring_buffer_t;


/**
 * @brief Initializes a ring buffer instance.
 *
 * @param handle Pointer to a user-allocated ring_buffer_t struct.
 * @param buffer Pointer to the memory to be used as a buffer.
 * @param size The size of the provided buffer in bytes.
 * @param overwrite_on_full If RB_TRUE, allows writing to overwrite the oldest data when the buffer is full.
 * @param read_func Optional custom function to copy data from the buffer. If NULL, a default implementation is used.
 * @param write_func Optional custom function to copy data to the buffer. If NULL, a default implementation is used.
 * @return RB_TRUE on success, RB_FALSE on failure (e.g., null pointers).
 */
rb_bool rb_init(rb_handle_t handle, void* buffer, rb_size_t size, rb_bool overwrite_on_full, rb_copy_func_t read_func, rb_copy_func_t write_func);

/**
 * @brief Writes data to the ring buffer.
 *
 * @param handle The handle of the ring buffer instance.
 * @param data Pointer to the data to be written.
 * @param length The number of bytes to write.
 * @return The number of bytes actually written. If overwrite_on_full is false and the buffer is full, returns 0.
 */
rb_size_t rb_write(rb_handle_t handle, const void* data, rb_size_t length);

/**
 * @brief Reads data from the ring buffer. Data read is removed from the buffer.
 *
 * @param handle The handle of the ring buffer instance.
 * @param buffer Pointer to a buffer where the read data will be stored.
 * @param length The maximum number of bytes to read.
 * @return The number of bytes actually read.
 */
rb_size_t rb_read(rb_handle_t handle, void* buffer, rb_size_t length);

/**
 * @brief Reads data from the ring buffer without removing it.
 *
 * @param handle The handle of the ring buffer instance.
 * @param buffer Pointer to a buffer where the peeked data will be stored.
 * @param length The maximum number of bytes to peek.
 * @return The number of bytes actually peeked.
 */
rb_size_t rb_peek(rb_handle_t handle, void* buffer, rb_size_t length);

/**
 * @brief Gets the amount of free space in the buffer.
 *
 * @param handle The handle of the ring buffer instance.
 * @return The free space in bytes.
 */
rb_size_t rb_get_free_space(rb_handle_t handle);

/**
 * @brief Gets the amount of used space in the buffer.
 *
 * @param handle The handle of the ring buffer instance.
 * @return The used space in bytes.
 */
rb_size_t rb_get_used_space(rb_handle_t handle);


#endif /* RING_BUFFER_H */
